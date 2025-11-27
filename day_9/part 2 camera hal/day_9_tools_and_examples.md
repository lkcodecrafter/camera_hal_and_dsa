# Day 9 Addendum — Trace Checklist Script, Mock Perfetto Capture, Driver IOCTL Stub & User-Space Submit

This document contains three deliverables you requested:

1. **Trace analysis checklist script** (bash) — runnable on a host with `adb` and `grep`/`awk`/`sed` available.
2. **Mock perfetto capture with annotated expected events** — a textual mock trace and guidance to recognize events in real captures.
3. **Driver IOCTL stub + user-space submit example** — kernel-style ioctl handler pseudocode (C) and user-space submit example (C) showing how HAL/CHI/CAMX might call into the kernel driver.

---

# 1) Trace Analysis Checklist Script (Bash)

Save this script as `camera_trace_checklist.sh` on your workstation. Make it executable (`chmod +x camera_trace_checklist.sh`) and run it while reproducing the bug.

```bash
#!/usr/bin/env bash
# camera_trace_checklist.sh
# Usage: ./camera_trace_checklist.sh <frame_number (optional)>

set -euo pipefail
FRAME=${1-}
OUTDIR=/data/local/tmp/camera_trace_check
mkdir -p $OUTDIR

echo "[+] Collecting camera dumpsys & logs..."
adb root || true
adb shell "dumpsys media.camera > /data/local/tmp/camera_dumps.txt"
adb pull /data/local/tmp/camera_dumps.txt $OUTDIR/

echo "[+] Capturing kernel trace snapshot (requires root/tracing enabled)"
adb shell "cat /sys/kernel/debug/tracing/trace" > $OUTDIR/trace_raw.txt || echo "failed to read kernel trace"

echo "[+] Collecting logcat (camera tags)"
adb logcat -b main -d -s CameraService:V CAMX:V Camx:V CamxResult:V > $OUTDIR/logcat_camera.txt

# Perfetto capture (if available) - generates a .pb, needs perfetto binary present on device
echo "[+] Attempting perfetto capture (10s) - requires perfetto on device"
adb shell "perfetto -c /data/misc/perfetto-configs/camera_config.pbtx -o /data/misc/perfetto-traces/camera.pb" || echo "perfetto capture failed or not available"
adb pull /data/misc/perfetto-traces/camera.pb $OUTDIR/ 2>/dev/null || true

# Basic filtering helpers
function grep_frame() {
    local f=$1
    echo "\n[+] Filtering traces for frame=${f} (trace_raw.txt)"
    grep -n "frame[: =]${f}\b" $OUTDIR/trace_raw.txt || grep -n "frame_number.*${f}" $OUTDIR/trace_raw.txt || echo "No explicit frame tag found"
}

if [[ -n "$FRAME" ]]; then
    grep_frame $FRAME
fi

# Look for common CAMX tracepoints
echo "\n[+] Searching for CAMX tracepoints (enqueue, node.start, ioctl, irq, result)"
grep -nE "camx.*(enqueue|node.start|node.finish|ioctl|irq|result|return)" $OUTDIR/trace_raw.txt || echo "No camx tracepoints found"

# Check for long gaps between node.start -> node.finish
echo "\n[+] Detecting long node durations (>50ms heuristic)"
awk '/node.start/ {s[$6]=$1} /node.finish/ { if (s[$6]) { dur=$1 - s[$6]; if (dur>0.05) print "Long node:", $6, "duration(s)=", dur; delete s[$6] } }' $OUTDIR/trace_raw.txt || true

# Check for unsignalled fences
echo "\n[+] Looking for fence wait timeouts or unsignalled fence patterns"
grep -n "fence.*timeout\|unsignalled\|wait.*timeout" $OUTDIR/logcat_camera.txt || echo "No fence timeouts in logcat"

# Quick summary
echo "\n[+] Summary files in $OUTDIR"
ls -lh $OUTDIR || true

echo "\n[+] Done. Next steps:\n - Open perfetto UI for camera.pb (if available)\n - Use frame number to follow event chain: enqueue -> node.start -> ioctl.submit -> irq -> node.finish -> result.return\n"
```

**Notes & Usage:**
- The script attempts to run perfetto capture; if not present on the device, it will skip gracefully.
- The `awk` detection for long node duration is heuristic: perfetto timestamps are usually in seconds with fractional seconds in the trace dump — adjust thresholds for your device.
- Run the script immediately after reproducing the problem so the trace contains the failing frame.

---

# 2) Mock Perfetto Capture (Annotated Expected Events)

Below is a **textual mock** of a perfetto-style event sequence for a single frame (frame 123). Use this as a checklist to map events in your real trace. Timestamps are illustrative (seconds).

```
# Mock perfetto trace (frame=123)

0.120000  camx.request.enqueue        frame=123  req_id=0x7ff
0.120050  camx.node.start             node=IFE    frame=123
0.120120  camx.node.finish            node=IFE    frame=123
0.120130  camx.node.start             node=BPS    frame=123
0.120250  camx.ioctl.submit           frame=123  ioctl_id=42
0.120260  camx.node.start             node=IPE    frame=123
0.120300  irq.hw.complete             irq_id=55   frame=123
0.120310  camx.ioctl.complete         ioctl_id=42 frame=123
0.120320  camx.node.finish            node=BPS    frame=123
0.120350  camx.node.finish            node=IPE    frame=123
0.120360  camx.result.return          frame=123  metadata_size=512
0.120370  hal3.send_result            frame=123
0.120380  app.callback.onImage        frame=123

# Annotated notes:
# - Enqueue -> IFE start should be very fast (<<1ms)
# - BPS and IPE are where heavy work occurs; check duration for each
# - ioctl.submit should be shortly before irq.hw.complete (hardware exec time)
# - If camx.ioctl.complete is much later than irq.hw.complete, driver may be spending time in copy/pin or error states
# - Long gap between camx.node.start and camx.node.finish indicates the node is the bottleneck
```

## How to use this mock with a real perfetto trace
1. Open perfetto trace in UI.
2. Find `camx.request.enqueue` for your frame number.
3. Trace the flow rightwards: node lanes will show `node.start` and `node.finish` slices.
4. Pay attention to `ioctl` submit/complete events and IRQ durations.
5. Look for unexpected large gaps or repeated resubmits.

---

# 3) Driver IOCTL Stub + User-Space Submit Example

Below are two pieces:

A. **Kernel driver-style IOCTL handler** (pseudocode/C) — simplified and annotated.

B. **User-space submit example** (C) — how HAL/CHI/CAMX may call into the kernel driver using `ioctl()` and `mmap()` / `dma-buf` handles.

> These are illustrative: real drivers include many more checks (security, pinning, scatter-gather lists, cache maintenance, VMA handling, error codes).

---

## A) Kernel IOCTL Handler (C, pseudocode)

```c
// kernel_stub.c (pseudo)
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define CAMX_IOCTL_SUBMIT _IOW('C', 1, struct camx_submit_req)
#define CAMX_IOCTL_WAIT   _IOR('C', 2, int)

struct camx_buffer_desc {
    uint64_t dma_addr; // physical DMA address
    uint32_t len;
    uint32_t handle; // user-space handle or dma-buf fd mapping
};

struct camx_submit_req {
    uint32_t frame_number;
    uint32_t num_buffers;
    struct camx_buffer_desc bufs[4];
    uint64_t metadata_ptr; // user-space pointer to metadata (if required)
};

static long camx_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
    case CAMX_IOCTL_SUBMIT: {
        struct camx_submit_req req;
        if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
            return -EFAULT;

        pr_info("camx: submit frame %u, buffers=%u\n", req.frame_number, req.num_buffers);

        // 1) Pin buffers (dma_buf get/attach/map)
        // 2) Build HW descriptor (registers, SG lists)
        // 3) Kick DMA/ISP via registers or DMA engine
        // 4) Return an ioctl token back if needed (via user-space struct)

        // store request in internal queue
        enqueue_hw_request(&req);
        return 0;
    }
    case CAMX_IOCTL_WAIT: {
        int frame = 0;
        // blocking wait for a frame complete - simplistic
        wait_for_frame_complete(&frame);
        if (copy_to_user((int __user *)arg, &frame, sizeof(frame)))
            return -EFAULT;
        return 0;
    }
    default:
        return -ENOTTY;
    }
}

// File operations struct pointing to camx_ioctl ...
```

**Kernel-side notes:**
- Real code must convert user-space dma-buf FDs to `struct dma_buf` and attach them to device-specific DMAs.
- For security, validate metadata pointer, lengths, and fd rights.

---

## B) User-Space Submit Example (C)

This small example demonstrates opening the driver, preparing a submit struct, and calling `ioctl()` to submit a frame.

```c
// user_submit.c (pseudo)
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CAMX_IOCTL_SUBMIT _IOW('C', 1, struct camx_submit_req)

struct camx_buffer_desc { uint64_t dma_addr; uint32_t len; uint32_t handle; };
struct camx_submit_req { uint32_t frame_number; uint32_t num_buffers; struct camx_buffer_desc bufs[4]; uint64_t metadata_ptr; };

int main() {
    int fd = open("/dev/camx0", O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    struct camx_submit_req req;
    req.frame_number = 123;
    req.num_buffers = 1;
    req.bufs[0].dma_addr = 0x12345000; // normally obtained via mmap/dma-buf
    req.bufs[0].len = 4096;
    req.bufs[0].handle = 42; // placeholder
    req.metadata_ptr = 0; // could be pointer to metadata in shared mem

    if (ioctl(fd, CAMX_IOCTL_SUBMIT, &req) < 0) {
        perror("ioctl submit");
        close(fd);
        return 1;
    }

    // Optionally wait for completion
    int completed_frame = -1;
    if (ioctl(fd, CAMX_IOCTL_WAIT, &completed_frame) == 0) {
        printf("Frame %d completed\n", completed_frame);
    }

    close(fd);
    return 0;
}
```

**User-space notes:**
- In real systems, DMA addresses are obtained by importing `dma-buf` FDs using `sync_file` / `dma-buf` APIs and mapping via `mmap()` or using `ION` allocations.
- Use proper cache maintenance (ioctl or sync_file) when needed.

---

# Final Notes & Next Steps

If you want I can:
- Turn the bash script into a more advanced Python analyzer that parses the perfetto text export and highlights long nodes automatically.
- Produce a synthetic perfetto `.pb` file with the mock events (requires proto generation & binary tooling). I can provide the proto-compatible JSON if you want to import into perfetto UI.
- Generate a fully annotated C kernel module skeleton (with DMA-buf attach/detach) for study purposes.

Which one would you like next?

