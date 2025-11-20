# 📸 **Day 7 — Mixed (DSA + Camera HAL + CAMX + CHI)**
_Advanced Interview Prep — Google Camera / Imaging / Android Camera Framework_

---
# 🟦 **Part 1 — DSA (Advanced Graph + DP + Concurrency Safe Code)**

## ✅ **1. Topological Sort + Real Camera Use Case**
### **Why Topo Sort matters in CAMX?**
CAMX pipelines are DAGs:
- Nodes = IFE, BPS, IPE, LRME, FD, Stats modules
- Edges = buffer flow
- Must execute in dependency order → Topo Sort

### **Code (C++)**
```cpp
vector<int> topoSort(int n, vector<vector<int>>& adj) {
    vector<int> indeg(n, 0);
    for (auto& v : adj)
        for (int x : v) indeg[x]++;

    queue<int> q;
    for (int i = 0; i < n; i++) if (indeg[i] == 0) q.push(i);

    vector<int> order;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        order.push_back(u);
        for (int x : adj[u]) {
            if (--indeg[x] == 0) q.push(x);
        }
    }
    return order;
}
```

### **Where Google may connect this?**
- CAMX pipeline ensures **IFE → BPS → IPE → JPEG** ordering.
- CHI graphs follow DAG execution scheduling.

---

## ✅ **2. DP: Minimum Path Sum + Camera Power Optimization Analogy**
Used to model **best-performance vs battery tradeoffs**.

### **Code (C++)**
```cpp
int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<vector<int>> dp(m, vector<int>(n));

    dp[0][0] = grid[0][0];
    for (int i = 1; i < m; i++) dp[i][0] = dp[i-1][0] + grid[i][0];
    for (int j = 1; j < n; j++) dp[0][j] = dp[0][j-1] + grid[0][j];

    for (int i = 1; i < m; i++)
        for (int j = 1; j < n; j++)
            dp[i][j] = grid[i][j] + min(dp[i-1][j], dp[i][j-1]);

    return dp[m-1][n-1];
}
```

---

## ✅ **3. Concurrency Safe Queue (C++17) — Relevant for Camera Threading**
Camera HAL uses many worker threads:
- RequestThread
- ResultThread
- 3A Thread
- CAMX Scheduler Thread

### **Lock-free Queue (Simplified)**
```cpp
class MPSCQueue {
    atomic<Node*> head{nullptr};
    atomic<Node*> tail{nullptr};
public:
    void push(int v) {
        Node* n = new Node(v);
        Node* prev = tail.exchange(n);
        if (prev) prev->next = n;
        else head.store(n);
    }

    bool pop(int& out) {
        Node* h = head.load();
        if (!h) return false;
        out = h->val;
        head.store(h->next);
        delete h;
        return true;
    }
};
```
This models camera **buffer queues** with multiple producers (hardware) and a single consumer (HAL processing thread).

---

# 🟥 **Part 2 — Camera HAL + CAMX + CHI (Deep Theory)**

## ✅ **1. Deep Dive — CAMX IFE → BPS → IPE Routing Architecture**

```
 RAW Sensor
    │
    ▼
 ┌────────┐     ┌────────┐     ┌────────┐
 │  IFE   │ --> │  BPS   │ --> │  IPE   │ --> Display/Preview
 └────────┘     └────────┘     └────────┘
    │              │               │
    │              │               └──→ JPEG encode path
    │              └──────────────────→ Multi-frame pipelines
    └──────────────────────────────────→ 3A Stats
```

### **IFE** (Image Front End)
- Black level
- Lens shading (optional)
- Statistics collection ← most important

### **BPS** (Bayer Processing Subsystem)
- Demosaic
- Noise reduction
- Local tone mapping

### **IPE** (Image Processing Engine)
- Scaling
- Sharpening
- Denoise
- YUV processing

---

## ✅ **2. ZSL Queue + HAL Capture Flow (Google Style)**

```
App → CameraService → HAL3 → CAMX → ISP → HAL3 → CameraService → App
```

### **Preview path** (fast)
```
App Request → HAL → ZSL ring buffer → real-time ISP → result metadata
```

### **Capture path** (slow)
```
App trigger → choose ZSL frame → reprocess via BPS/IPE → JPEG
```

---

## ✅ **3. CHI: How Google/Qualcomm Extends Camera Controls**
CHI (Camera Hardware Interface) gives OEMs advanced hooks:
- Custom nodes
- Custom metadata
- Custom pipelines
- Per-frame effects

### **Example CHI Node Flow**
```
Request → CHI Node → CAMX Node → CHI Node (Post) → HAL3 → App
```

---

## ✅ **4. 3A (AE/AF/AWB) Deep-Dive — Frame-by-Frame Flow**
### **AE Flow**
```
Frame N stats → AE algo → exp/gain → ISP → Frame N+1 brighter/darker
```

### **AF Flow**
```
Lens position sweep → contrast curve → max contrast → lock
```

### **AWB Flow**
```
Image RGB stats → white point estimation → gain correction
```

### **Visual Overview**
```
   [Stats Frame N]
          │
          ▼
  [3A Algorithms]
          │
          ▼
 [Apply settings for Frame N+1]
```

---

# 🟩 **Part 3 — Interview Questions (Day 7)**

### **Q1: Explain multi-camera sync between wide + tele.**
### **Q2: How does CAMX schedule IFE and IPE for parallel streams?**
### **Q3: How is HDR+ implemented across multiple frames?**
### **Q4: Describe how fences protect buffer access in HAL3.**
### **Q5: How would you debug a frame drop in result thread?**

---

# 🟪 **Part 4 — Assignments**

### **Task 1:** Implement topological sort + detect cycles.
### **Task 2:** Draw full CAMX pipeline with your device's sensors.
### **Task 3:** Write AE simulation: input brightness → output exp.
### **Task 4:** Implement ZSL queue in C++.

---

# 🎉 End of Day 7 — Great Progress!

