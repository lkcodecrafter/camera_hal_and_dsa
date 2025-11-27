# 📸 **Day 10 — HDR+, Multi‑Frame Fusion, Perf Debug, and CAMX Deep Internals**

This README contains:
- HDR+ end‑to‑end theory + diagrams
- Multi‑frame merge pipeline (IFE → BPS → IPE → GCH/GPU)
- Motion metering + exposure bracketing flow
- CAMX request flow for multi‑frame capture
- Perf debugging hooks
- C++ snippets for burst/ZSL/HDR+ request building
- Interview questions

---

# 🟩 **Part 1 — HDR+ Theory (Deep Dive)**

HDR+ is a **multi-frame computational photography pipeline** used in Pixel‑class devices.

### **Core steps:**
1. **ZSL queue collects 15–30 RAW frames**
2. **Motion metering** → detect stable vs moving region
3. **Frame selection** (typically 5–9 frames)
4. **Align frames** (optical‑flow, tile-based)
5. **Merge frames** (denoise + tone preserve)
6. **Local Tone Mapping (LTM)**
7. **Color correction + gamma + sharpening**
8. **JPEG encode**

### Diagram
```
[Sensor → IFE] --RAW--> [ZSL Queue] → [Frame Selector]
                                 → [Alignment Engine]
                                 → [Merge Engine]
                                 → [LTM]
                                 → [Color / Sharpen]
                                 → [JPEG]
```

---
# 🟦 **Part 2 — Multi‑Frame Capture CAMX Flow**

### **Flow for HDR+:**
```
apps → camera3 → HAL3 → CAMX → pipeline → IFE → BPS → IPE → CPU/GPU → result
```

### Detailed Steps

#### 1. **`process_capture_request()` (HAL3)**
```
- Receives capture request with HDR+ tag
- Chooses mode:
  - If ZSL available → use multi-frame burst
  - Else → create bracket (short, long, extra-long)
- Pushes N internal requests to CAMX
```

#### 2. **`ExecuteProcessRequest()` (CAMX)**
- Each frame generates a unique requestID
- Requests are pushed into the IFE/BPS/IPE pipeline
- Metadata per frame includes AE values, motion stats

#### 3. **Frame Alignment (software)**
- Done by GCH / CPU / GPU
- CAMX provides RAW buffers, timestamps, gyroscope metadata

#### 4. **Merge**
- Tiles are blended across multiple exposures
- Noise reduction by weighted averaging

#### 5. **Still Post‑processing**
- LTM (tone mapping)
- Gamma correction
- Sharpening

---
# 🟧 **Part 3 — Motion Metering (Pixel-style)**

**Goal:** choose a subset of frames with minimal motion.

### Inputs
- Gyro timestamps
- OIS samples
- Face motion vectors
- Optical‑flow between frames

### Algorithm
```
for each frame in ZSL-window:
    motionScore = gyroWeight*gyroDelta + OFWeight*opticalFlow + faceWeight*faceMovement
select frames with lowest motionScore
```

---
# 🟥 **Part 4 — Exposure Bracketing (Non-ZSL)**

### Typical bracket set
- EV0 (base frame)
- EV-2 (short exposure)
- EV+2 (long exposure)

### Flow
```
Request 1 → short exp → IFE → RAW
Request 2 → normal exp → IFE → RAW
Request 3 → long exp → IFE → RAW
```

---
# 🟩 **Part 5 — CAMX Architecture for Multi‑Frame HDR+**

### Pipeline Nodes
```
Sensor → IFE → (optional BPS) → IPE → FD / Stats → APP
```

### Which node handles what?
- **IFE** – RAW capture, black level, linearization
- **BPS** – demosaic, WB, basic NR
- **IPE** – tone-map, LTM, sharpening, crop, scale

---
# 🟦 **Part 6 — Performance Debug Points**

### Trace you should log for multi-frame
```
FrameIn:REQ <id>
IFE:SOF
IFE:EOF
BPS:IN/OUT
IPE:IN/OUT
3A:AE/AWB update received
MergeStart
MergeFinish
JPEGStart
JPEGFinish
FrameOut:RESULT <id>
```

### Slow HDR+ symptoms
| Symptom | Reason |
|--------|--------|
| Buffer queue backpressure | IPE slow / merge slow |
| Merge taking 100+ ms | Too many frames, motion blur |
| High ISO / noise | Selector picked wrong frames |

---
# 🟧 **Part 7 — Sample Code Snippets**

## **1. Build an HDR+ Burst from HAL3**
```cpp
for (int i = 0; i < burstCount; i++) {
    camera3_capture_request request = {};
    request.frame_number = base + i;
    request.settings = buildHdrSettings(i);
    request.input_buffer = nullptr;
    request.output_buffers = outputs;
    device->ops->process_capture_request(device, &request);
}
```

## **2. ZSL Frame Selection (Simplified)**
```cpp
Frame selectBestFrames(ZSLQueue& q) {
    vector<Frame> lowMotion;
    for (auto& f : q.frames) {
        if (f.motion < MOTION_THRESHOLD)
            lowMotion.push_back(f);
    }
    sort(lowMotion.begin(), lowMotion.end(),
        [](Frame& a, Frame& b){ return a.exposure < b.exposure; });
    return lowMotion;
}
```

## **3. Merge Tile (Pseudo)**
```cpp
for each tile T:
    alignedFrames = align(frames, T.region)
    T.out = weightedAverage(alignedFrames)
```

---
# 🟥 **Part 8 — Debugging Multi‑Frame Failures**

### 1. **Misaligned frames → ghosting**
Fix: Drop high-motion frames.

### 2. **HDR+ slow**
Fix: reduce bracket count or downscale merge resolution.

### 3. **Green/Pink tint**
Fix: mismatch in black level / WB between frames.

---
# 🟪 **Part 9 — Interview Questions (Day 10)**

### Q1. How does HDR+ merge differ from standard HDR bracketing?
### Q2. Explain motion metering and why gyro is critical.
### Q3. Why is aligning RAW frames difficult?
### Q4. How do you ensure temporal metadata consistency in HDR+ flow?
### Q5. Why is BPS useful before IPE in multi-frame pipelines?
### Q6. How do Pixel devices pick the *reference* frame?
### Q7. What causes rolling‑shutter distortion during merge?

---
# 🟫 **Part 10 — Assignments**

### **Task 1:** Draw full HDR+ pipeline for your device.
### **Task 2:** Implement motion scoring function.
### **Task 3:** Simulate a bracket capture with 3 exposures.
### **Task 4:** Log timestamps for fake pipeline and compute stall time.

---
👍 Day 10 Complete — Ready for Day 11 (Night Sight + Super Res Zoom)?

