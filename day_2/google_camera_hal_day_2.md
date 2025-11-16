# Day 2 – Google Camera HAL Prep (Parallel Track)

This document contains **Part 1 (DSA)** and **Part 2 (Camera HAL)** for Day 2, following the **D) Mix (recommended)** pattern.

---

# 🟦 Part 1 — DSA: Two Sum Variations + Sliding Window Intro

Today we strengthen your two‑pointer foundation with **Two Sum problems** + introduce **Sliding Window**, a pattern Google asks frequently.

---

# ✅ Problem 1: Two Sum (Sorted Array) — Refresher

### **Approach:** Two pointers.
- If sum < target → move left pointer
- If sum > target → move right pointer
- If sum == target → found

### **Solution (C++):**
```cpp
vector<int> twoSumSorted(vector<int>& arr, int target) {
    int l = 0, r = arr.size() - 1;
    while (l < r) {
        int sum = arr[l] + arr[r];
        if (sum == target) return {l, r};
        else if (sum < target) l++;
        else r--;
    }
    return {-1, -1};
}
```

---

# ✅ Problem 2: Two Sum (Unsorted Array)

### **Approach:** HashMap
- Store `value → index`
- For each element, check if `target - value` exists

### **Solution (C++):**
```cpp
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> mp;
    for (int i = 0; i < nums.size(); i++) {
        int need = target - nums[i];
        if (mp.count(need)) return {mp[need], i};
        mp[nums[i]] = i;
    }
    return {-1, -1};
}
```

---

# 🟨 Introduction to Sliding Window

Sliding window helps in problems involving:
- Subarrays
- Consecutive sequences
- Longest/shortest window

Two types:
1. **Fixed window** (size = k)
2. **Variable window** (expand + shrink)

---

# ✅ Problem 3: Maximum Sum of Subarray of Size K (Fixed Window)

### **Approach:**
- Create a window of size `k`
- Slide by removing old element and adding new one

### **Solution (C++):**
```cpp
int maxSumSubarrayK(vector<int>& arr, int k) {
    int windowSum = 0;
    int maxSum = 0;

    for (int i = 0; i < k; i++) windowSum += arr[i];
    maxSum = windowSum;

    for (int i = k; i < arr.size(); i++) {
        windowSum += arr[i] - arr[i - k];
        maxSum = max(maxSum, windowSum);
    }

    return maxSum;
}
```

---

# ============================
# 🟩 Part 2 — Camera HAL: Request/Result Model (HAL3 Core)
# ============================

This is the **backbone** of Camera HAL3. Google ALWAYS asks this.

---

# ⭐ Overview: Request / Result Flow
```
APP → Framework → CameraService → HAL3 → ISP → HAL3 → Framework → APP
```

### 📌 Request contains:
- Capture settings (AE, AF, AWB)
- Stream buffers (preview, video, jpeg)
- Control metadata

### 📌 Result contains:
- Output buffers (processed frames)
- Result metadata
- 3A convergence info

---

# ⭐ Detailed Flow (Interview‑Perfect Explanation)

## Step 1: App Sends CaptureRequest
Contains:
- Settings (ISO, exposure, focus mode)
- Output Surfaces

## Step 2: Framework Converts It
Framework packs it into internal structures.

## Step 3: CameraService Routes Request
- Handles permissions
- Session management
- Routes to correct HAL camera

## Step 4: HAL3 Receives in `process_capture_request()`
HAL:
- Validates streams
- Extracts metadata
- Queues work to internal threads
- Notifies ISP

## Step 5: ISP Processes Frame
ISP performs:
- AE
- AF
- AWB
- Denoise
- Sharpen
- RAW → YUV

## Step 6: HAL3 Gets Output Buffers
HAL:
- Fills result metadata
- Sends buffers & metadata back

## Step 7: Framework Notifies App
App receives:
- `CaptureResult`
- Image buffers

---

# ⭐ Code‑Like Pseudocode for `process_capture_request()`
(This is very commonly asked)
```cpp
int CameraHAL::process_capture_request(request) {
    // 1. Validate buffers
    if (!validate(request)) return ERROR;

    // 2. Extract metadata
    applySettings(request.settings);

    // 3. Queue request for processing
    threadPool.enqueue([this, request](){
        // 4. Talk to ISP
        ISP.process(request);

        // 5. Fill result metadata
        metadata = createMetadata();

        // 6. Send back result
        sendResult(request.outputBuffers, metadata);
    });

    return OK;
}
```

---

# 🟦 Interview Questions (Day 2)

### **Q1: What is inside a capture request?**
**Answer:** metadata (AE/AF/AWB), output stream buffers, settings.

### **Q2: What triggers AE/AF/AWB?**
**Answer:** HAL uses metadata → passes parameters to ISP → ISP runs 3A loops.

### **Q3: What is the difference between request & result metadata?**
**Answer:**
- Request metadata = desired settings
- Result metadata = actual settings applied by ISP

---

# 🎯 Day 2 Complete
Choose what you want for Day 3:
- **A)** Sliding window advanced
- **B)** Trees (DFS/BFS) + Camera 3A
- **C)** More HAL internals
- **D)** Mix (recommended)

