# 📘 **Day 7 – Interview Questions + Assignments (Detailed README)**
This README contains complete explanations for Day 7 interview questions and all assignments with diagrams, flows, and examples.

---
# 🟩 **Part 3 — Interview Questions (Day 7)**

## 🔵 **Q1: Explain multi-camera sync between wide + tele.**
Multi‑camera sync ensures wide + tele (or ultra‑wide) capture the **same moment**.

### 📌 How sync works:
1. **Master–Slave model**
   - Wide = master sensor
   - Tele = slave sensor

2. **Shared clock + shared exposure timing**
   - Sensors must start exposure at same time.

3. **Sync signals (VSYNC / SOF)**
```
Wide SOF ----┐
             ├──> CAMX Sync Manager → Frame Align
Tele SOF ----┘
```

4. **Frame timestamp correction**
   - CAMX aligns timestamps so HDR+/fusion algorithms can merge frames.

5. **Fusion requirement**
   - Use cases: Super Res Zoom, Portrait, Stabilization.

---

## 🔵 **Q2: How does CAMX schedule IFE and IPE for parallel streams?**

### 📌 Example parallel streams:
- Preview 1080p
- Video 4K
- Still Capture YUV

CAMX uses a **DAG-based runtime scheduler**:
1. Build graph: IFE → BPS → IPE → Outputs
2. Determine dependencies
3. Compute execution order via **topological sort**
4. Assign work to hardware blocks based on availability

### Flow:
```
           ┌────→ IPE (Preview) ───→ Surface
IFE ─→ BPS ┤
           └────→ IPE (Video) ─────→ Encoder
```

### Scheduling ensures:
- No deadlock
- No overlap of incompatible pipelines
- Max hardware parallelism

---

## 🔵 **Q3: How is HDR+ implemented across multiple frames?**
HDR+ is a **multi-frame burst merge algorithm** used heavily by Google Pixel.

### Steps:
1. Capture 8–15 **under‑exposed** frames via ZSL.
2. Choose best 4–7 frames using **AI-based scoring**.
3. Align frames using ML-based motion estimation.
4. Merge frames → noise reduction + detail enhancement.
5. Apply tone-map + color grading.
6. Output a single HDR JPEG.

### Flow:
```
[Frames] → [Alignment] → [Merge] → [Tone-map] → [JPEG]
```

---

## 🔵 **Q4: Describe how fences protect buffer access in HAL3.**
Fences ensure camera pipeline does not read/write a buffer at the wrong time.

### Types:
- **Acquire Fence** → wait before reading
- **Release Fence** → signal after writing

### Example Flow:
```
IFE writes buffer --release_fence-->
HAL waits --acquire_fence-->
IPE reads buffer safely
```

### Purpose:
- Prevent race conditions
- Ensure metadata–buffer consistency
- Synchronize multithreaded nodes

---

## 🔵 **Q5: How would you debug a frame drop in result thread?**

### Steps interviewers expect:
1. **Check logs**
   - CAMX, CHI, Kernel dmesg
2. **Check IPE deadlines**
   - If > 33ms (30fps), frame drops are guaranteed
3. **Verify buffer queue capacity**
4. **Check if 3A/ML algorithms overloaded CPU**
5. **Analyze timestamps**
   - Missing or out-of-order Result Metadata
6. **Simulate offline replay**
7. **Reduce pipeline load** (test mode)

### Flow:
```
Sensor → IFE → BPS → IPE (delay) → Frame Drop
```

---

# 🟪 **Part 4 — Assignments**

## 🧩 **Task 1: Implement topological sort + detect cycles.**
### Requirements:
- Use adjacency list
- Detect back edges
- Return error if cycle found

### Output:
- Valid topo order or "cycle detected"

---

## 🧩 **Task 2: Draw full CAMX pipeline with your device's sensors.**

### Include:
- IFE
- BPS
- IPE
- LRME
- JPEG
- Stats Nodes
- CHI custom nodes (if any)

### Example pattern:
```
Sensor → IFE → BPS → IPE → JPEG
               └→ Stats → 3A
```

---

## 🧩 **Task 3: Write AE simulation: input brightness → output exposure.**

### Simple model:
```
if (brightness < target)
   increase exposure
else
   decrease exposure
```

### Output:
- New exposure/gain values

---

## 🧩 **Task 4: Implement ZSL queue in C++.**

### Requirements:
- Ring buffer of max N frames
- push(frame)
- getBestFrame()
- dropOldest()

### Expected Flow:
```
push → push → push → capture → choose best → reprocess
```

---

# 🎉 End of Day 7 Interview + Assignments README
This completes all Day 7 explanations and tasks!

