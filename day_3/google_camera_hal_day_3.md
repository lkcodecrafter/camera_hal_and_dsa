# Day 3 – Google Camera HAL + DSA (Parallel Track)
# Mode: **D) Mix (Recommended)**

Today we combine **DSA (Sliding Window Advanced)** + **Camera HAL Deep Dive** with **Qualcomm CAMX + CHI architecture details**, which are VERY important for Qualcomm → Google transitions.

---

# 🟦 Part 1 — DSA: Sliding Window (Advanced)

Sliding Window is a core Google pattern.
Two types:
- **Fixed window** → size = k
- **Variable window** → expand + shrink based on condition

Today we learn **variable window**, frequently asked at FAANG.

---

# ✅ Problem 1: Longest Substring Without Repeating Characters

### **Idea:**
Use window + HashMap to track last index of each character.
Shrink window when you see a duplicate.

### **Solution (C++):**
```cpp
int lengthOfLongestSubstring(string s) {
    vector<int> last(256, -1);
    int l = 0, maxLen = 0;

    for (int r = 0; r < s.size(); r++) {
        if (last[s[r]] >= l) {
            l = last[s[r]] + 1;
        }
        last[s[r]] = r;
        maxLen = max(maxLen, r - l + 1);
    }
    return maxLen;
}
```

---

# ✅ Problem 2: Minimum Window Substring (Hard, but simplified)

### **Idea:**
- Expand `r` until all required characters included.
- Shrink `l` to minimize window.

### **Solution Template (C++):**
(We keep it conceptual for now, as this is Google-hard)
```cpp
string minWindow(string s, string t) {
    vector<int> need(128, 0);
    for (char c : t) need[c]++;

    int required = t.size();
    int l = 0, r = 0, minLen = INT_MAX, start = 0;

    while (r < s.size()) {
        if (need[s[r]]-- > 0) required--;

        while (required == 0) {
            if (r - l + 1 < minLen) {
                minLen = r - l + 1;
                start = l;
            }
            if (++need[s[l]] > 0) required++;
            l++;
        }
        r++;
    }
    return minLen == INT_MAX ? "" : s.substr(start, minLen);
}
```

---

# ============================
# 🟩 Part 2 — Camera HAL Deep Dive: Request Threading, Buffer Flow, CAMX & CHI (Qualcomm)
# ============================

This is the REAL low-level knowledge that impresses Google.
You already work on Qualcomm → we connect HAL3 ↔ CAMX ↔ CHI.

---

# ⭐ 1. HAL3 Request-Handling Thread Model

HAL3 uses **asynchronous threading**:

```
Framework → Request Thread → ISP Thread(s) → Callback Thread
```

### **Threads inside HAL3:**
- **Request Thread** → handles all CaptureRequests
- **Processing Thread(s)** → interacts with ISP hardware
- **Callback Thread** → sends results back to framework

This decoupling allows:
- Parallel pipeline
- Reduced latency
- Multiple in-flight requests

---

# ⭐ 2. Buffer Flow Detailed (Google-level answer)

### **Complete Data Path:**
```
App → CameraDevice → CameraService → HAL3
HAL3 → CAMX / CHI → ISP → CAMX → HAL3 → Framework → App
```

### Step-by-step:
1. **App sends request with output streams (SurfaceTextures)**
2. **Framework maps Surfaces → BufferQueue → GraphicBuffers**
3. **CameraService sends capture to HAL**
4. **HAL3 validates buffers + settings**
5. **HAL3 passes request to CAMX pipeline via CHI**
6. **CAMX configures hardware nodes + IQ modules**
7. **ISP processes RAW → YUV/JPEG**
8. **CAMX returns processed buffer**
9. **HAL3 fills result metadata**
10. **Framework notifies app**

---

# 🟥 Part 3 — Qualcomm CAMX + CHI (DEEP THEORY)

This is the MOST important section for your Qualcomm → Google Camera Engineer interview.

---

# ⭐ CAMX Architecture (Qualcomm)

### **Definition:**
CAMX is Qualcomm’s camera engine responsible for:
- Hardware pipeline configuration
- Real-time image processing
- Interfacing with ISP, sensor drivers
- Managing multiple modules (PDLib, AF, AWB, AEC, etc.)

### **CAMX Pipeline Example:**
```
Sensor → ISP → CAMIF → BPS → IFE → IPE → JPEG → HAL3
```

### Key Components:
- **IFE (Image Front End)** → RAW processing, demosaic
- **BPS (Bayer Processing System)** → noise reduction, sharpening
- **IPE (Image Processing Engine)** → scaling, filtering
- **JPEG** → encoding

---

# ⭐ CHI Layer (Camera Hardware Interface)

CHI is Qualcomm’s vendor extension layer ON TOP OF HAL3.

### **Purpose:**
- Extends HAL3
- Provides a plugin architecture
- Used for OEM tuning, custom features
- Routes metadata & buffers to CAMX

### **Flow:**
```
HAL3 → CHI → CAMX → HW Blocks → CAMX → CHI → HAL3
```

### **CHI Features:**
- Custom metadata tags
- Custom algorithms (face detection, HDR)
- OEM feature integration

---

# ⭐ How Google Evaluates Qualcomm Engineers

Google expects you to:
- Explain CAMX pipeline blocks
- Explain how CHI fits between HAL3 and CAMX
- Explain metadata routing
- Show understanding of 3A loops in Qualcomm

They do NOT expect specific secret internals.
Only your architecture-level understanding.

---

# ⭐ Interview-Perfect Explanation (Memorize this)

### **"In Qualcomm’s architecture, HAL3 is just a thin layer. Most processing happens in CAMX, driven by CHI plugins. HAL3 forwards requests to CHI, CHI configures CAMX nodes, CAMX interacts with ISP hardware, processes buffers, and returns results back through CHI and HAL3."**

This impresses Google a LOT.

---

# ⭐ Comparison Table (Google LOVES this)

| Layer | Android HAL3 | Qualcomm CHI | Qualcomm CAMX |
|------|---------------|--------------|----------------|
| Role | Standard Google-defined HAL | Vendor extension layer | Full camera engine + ISP control |
| Purpose | Interface between framework & vendor | Extend HAL3 | Configure HW pipelines |
| Who controls it? | Google | OEM/Qualcomm | Qualcomm |
| Main Work | Metadata + buffers | OEM features | Sensor, ISP, IQ modules |

---

# 🟦 Day 3 Interview Questions

### **Q1:** Explain complete request-to-result flow including CAMX.
### **Q2:** Difference between HAL3 and CHI.
### **Q3:** How does CAMX talk to ISP hardware?
### **Q4:** What is IFE/BPS/IPE? Which processes RAW?
### **Q5:** How does buffer flow differ between preview & still capture?

---

# 🎯 Day 3 Complete
Choose for Day 4:
- **A)** Trees + Camera 3A (AE/AF/AWB)
- **B)** CameraStream configuration + formats
- **C)** More CAMX deep dive (IFE/BPS/IPE breakdown)
- **D)** Mix (recommended)

