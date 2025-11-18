# 📸 **Day 5 — CAMX Deep Dive + Multi-Camera + DSA Mix**
### *Google Camera Engineer Interview Prep (Advanced)*

Day 5 focuses on:
- Deep CAMX architecture internals
- Multi-camera fusion (wide+ultra-wide+tele)
- Sensor synchronization
- Rolling shutter theory
- ISP request batching & multi-stream routing
- DSA mix (Graphs + Sliding Window Advanced)

---

# 🧩 **Part A — CAMX Deeper Architecture**

### 📌 **1. CAMX High-Level Architecture**

```
Framework → HAL3 → CHI → CAMX Core → Pipelines → Nodes → Device Drivers → ISP/Sensor
```

CAMX layers:
1. **CamxCore**  
   - Request manager  
   - Metadata pool  
   - Pipeline builder

2. **CamxNodes**  
   - IFE, BPS, IPE, LRME, StatsNodes

3. **CamxDevice**  
   - ISP bridges  
   - Kernel IOCTL

4. **CamxHAL**  
   - HAL3 adaptation

---

## 📌 2. CAMX Request Manager (VERY IMPORTANT FOR INTERVIEW)

Handles:
- Request queue
- Buffer routing
- Metadata handoff
- Sync fences (input + output)
- Batch processing (video)

### Request State Machine
```
QUEUED → VALIDATE → PROGRAM_HW → WAIT_IRQ → READY → RETURN_TO_HAL
```

### Key Components
- `RequestQueue`  
- `CamxScheduler`  
- `PerFrameSettings`  
- `CmdBufferManager`

---

# 📷 **Part B — Multi-Camera (Wide + Ultra-Wide + Tele)**

Google LOVES asking questions about:
- sensor sync
- metadata fusion
- switching pipelines
- zoom mapping

### 1. Multi-Camera Sync Pipeline Diagram

```
Sensor_W → IFE_W ─┐
                  ├──→ [Sync Node] → [FOV Matching] → [Depth / Parallax] → IPE → HAL
Sensor_UW → IFE_UW ┘
```

Important Concepts:
- Master/slave sensor clocking
- Rolling-shutter compensation
- Timestamp alignment
- Lens pose calibration

### 2. Why multi-camera is difficult
- Different FoVs = different pixel mapping
- Different rolling shutter skew
- Per-sensor noise profiles
- Independent 3A per-sensor
- Fusion must preserve texture + color

### 3. Interview Trick Question: *What if sensors return frames at slightly different times?*

Answer:
- Apply timestamp alignment window (ex: ±1ms)
- Resample warp grid based on IMU + gyro
- Use the last converged 3A metadata from master camera

---

# 🧠 **Part C — Rolling Shutter Explained (with Diagram)**

### Rolling Shutter Frame Readout

```
Time →
Row1  --------------------
Row2      --------------------
Row3          --------------------
Row4              --------------------
```

Due to row-by-row exposure & readout:
- Fast motion = skew
- Multi-camera sync = harder
- HDR fusion = alignment required

### Fixes
- Rolling shutter calibration
- Gyro-based de-skew
- Global shutter correction in IPE

---

# 🔄 **Part D — Buffer Routing: Multi-Stream Case**

Example: App requests
- Preview (YUV)
- Video (NV12)
- JPEG

CAMX builds **three branches** from one sensor.

```
RAW → IFE → BPS → IPE → [Preview]
                        [Video]
                        [JPEG → HW encoder]
```

IFE produces RAW + Stats → BPS → IPE branches.

### Buffer Mapping Table
| Stream | Format | Node Output |
|--------|---------|-------------|
| Preview | YUV_420 | IPE Out 0 |
| Video | NV12 | IPE Out 1 |
| JPEG | BLOB | JPEG Encoder |

---

# 🔥 **Part E — DSA Mix (Day 5)**

Two problems related to camera pipelines.

## 1. **DSA Graph Problem: Detect Cycle in Directed Graph**
Why relevant?  
CAMX pipeline is a **directed graph** of nodes.

### C++ Solution
```cpp
bool dfs(int u, vector<vector<int>>& g, vector<int>& vis, vector<int>& rec) {
    vis[u] = rec[u] = 1;
    for (int v : g[u]) {
        if (!vis[v] && dfs(v, g, vis, rec)) return true;
        else if (rec[v]) return true;
    }
    rec[u] = 0;
    return false;
}

bool hasCycle(vector<vector<int>>& g) {
    int n = g.size();
    vector<int> vis(n, 0), rec(n, 0);
    for (int i = 0; i < n; i++)
        if (!vis[i] && dfs(i, g, vis, rec)) return true;
    return false;
}
```

Use this in interview to connect CAMX → node graphs.

---

## 2. **Sliding Window Hard Problem: Longest Substring with K Distinct**
Matches concepts of **buffer windows**, **request queues**, etc.

### C++ Solution
```cpp
int longestK(string s, int k) {
    unordered_map<char,int> mp;
    int l = 0, ans = 0;

    for (int r = 0; r < s.size(); r++) {
        mp[s[r]]++;

        while (mp.size() > k) {
            mp[s[l]]--;
            if (mp[s[l]] == 0) mp.erase(s[l]);
            l++;
        }

        ans = max(ans, r - l + 1);
    }
    return ans;
}
```

---

# 📝 **Part F — Day 5 CAMX High-Level Question Set**

### Q1. Explain how CAMX schedules multiple pipelines.
- Each stream has a pipeline instance
- Request Manager distributes frame numbers
- Sync fences used for resource arbitration

### Q2. What is frame bundling?
- Submitting N requests together (video)
- Reduces IOCTL calls
- Better pipeline saturation

### Q3. Why do we need multi-camera timestamp alignment?
- Global shutter mismatch
- Per-frame pose estimation must match
- Fusion quality depends on <1ms sync

### Q4. How do you reduce IPE latency by 15%?
- Reduce block size
- Disable heavy NR
- Skip tone curves
- Use fast-path kernels

---

# 🧪 **Part G — Day 5 Exercises**

### 1. Read CAMX source and list 10 nodes
Examples:
- IFENode
- BPSNode
- IPENode
- StatsNode
- LRME
- ANR

### 2. Draw multi-camera fusion block diagram (practice)

### 3. Solve: Kth largest element (heaps) — common Google question

---

# ✅ **End of Day 5**
Want Day 6 with:
- **Request-to-Result detailed timing diagrams**
- **JPEG + RAW + ZSL pipeline internals**
- **Concurrency (Threading) in CAMX**?

