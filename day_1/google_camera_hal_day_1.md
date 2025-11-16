# Day 1 – Google Camera HAL Prep (Parallel Track)

This document contains **Part 1 (DSA)** and **Part 2 (Camera HAL)** for Day 1, with **explanations + solutions**.

---

# 🟦 Part 1 — DSA: Two Pointers Pattern

## 📌 Overview
Two pointers is a pattern where we maintain two indices (`left`, `right`) and move them based on conditions. Useful for:
- Reversing arrays
- Palindrome checking
- Two-sum (sorted)
- Removing duplicates
- Merging

---

## ✅ Problem 1: Reverse an Array
### **Explanation:**
We swap first and last, then move pointers inward.

### **Dry Run:**
Array: `[10, 20, 30, 40, 50]`

```
Step 1: L=0, R=4 → swap 10 and 50
[50, 20, 30, 40, 10]

Step 2: L=1, R=3 → swap 20 and 40
[50, 40, 30, 20, 10]

Step 3: L=2, R=2 → stop
```

### **Solution (C++):**
```cpp
void reverseArray(vector<int>& arr) {
    int l = 0, r = arr.size() - 1;
    while (l < r) {
        swap(arr[l], arr[r]);
        l++;
        r--;
    }
}
```

---

## ✅ Problem 2: Check Palindrome Array
### **Explanation:**
Compare `left` and `right` elements.

### **Solution:**
```cpp
bool isPalindrome(vector<int>& arr) {
    int l = 0, r = arr.size() - 1;
    while (l < r) {
        if (arr[l] != arr[r]) return false;
        l++;
        r--;
    }
    return true;
}
```

---

## ✅ Problem 3: Two Sum (Sorted Array)
### **Goal:** find two numbers that sum to a target.

### **Solution:**
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

## ✅ Problem 4: Remove Duplicates from Sorted Array
### **Explanation:**
Two pointers:
- `i` → points to place where next unique element goes
- `j` → scans array

### **Solution (C++):**
```cpp
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int i = 0;
    for (int j = 1; j < nums.size(); j++) {
        if (nums[j] != nums[i]) {
            i++;
            nums[i] = nums[j];
        }
    }
    return i + 1; // new length
}
```

---

## ✅ Problem 5: Merge Two Sorted Arrays
### **Explanation:**
Use two pointers to merge while comparing elements.

### **Solution (C++):**
```cpp
vector<int> mergeSorted(vector<int>& a, vector<int>& b) {
    int i = 0, j = 0;
    vector<int> res;

    while (i < a.size() && j < b.size()) {
        if (a[i] < b[j]) res.push_back(a[i++]);
        else res.push_back(b[j++]);
    }

    while (i < a.size()) res.push_back(a[i++]);
    while (j < b.size()) res.push_back(b[j++]);

    return res;
}

```
---

# 🟩 Part 2 — Camera HAL: HAL3 Architecture Overview

## ⭐ HAL3 Pipeline Overview
```
App → Framework → CameraService → CameraProvider → HAL3 → ISP → Sensor
```

### 🔹 **App Layer (Camera2 / CameraX)**
Creates capture requests.

### 🔹 **Framework Layer**
Converts high-level API calls to internal framework calls.

### 🔹 **CameraService**
Handles:
- Permissions
- Multi‑client support
- Routing requests to HAL

### 🔹 **CameraProvider**
Vendor module registration.

### 🔹 **Camera HAL3**
Your main area. Implements:
- `open()`
- `configure_streams()`
- `process_capture_request()` (MOST IMPORTANT)
- `flush()`

### 🔹 **ISP (Image Signal Processor)**
Handles:
- AE / AF / AWB
- Denoise
- Sharpen
- RAW → YUV

### 🔹 **Sensor**
Physical camera.

---

# ⭐ HAL3 Key Functions with Explanations

## 🔸 `open()`
Opens camera, allocates internal objects, threads, buffers.

## 🔸 `configure_streams()`
- Sets preview, video, still capture streams.
- HAL decides supported sizes/formats.

## 🔸 `process_capture_request()`
**Most critical interview topic.**

### What happens inside:
1. Read request metadata (exposure, focus, AWB, etc.)
2. Validate streams + buffers
3. Queue work to processing threads
4. Talk to ISP
5. Produce result metadata
6. Return buffers to framework

## 🔸 `flush()`
Cancel in‑flight requests.

---

# 🟨 Part 2 — Interview Questions + Answers

### **Q1: Explain HAL3 capture pipeline from request to result.**
**Answer:**
1. App creates a `CaptureRequest`.
2. Framework sends it to **CameraService**.
3. CameraService forwards to **CameraProvider**.
4. Provider sends to **HAL3**.
5. HAL3 receives request in `process_capture_request()`.
6. HAL configures metadata → passes to ISP.
7. ISP processes (3A, noise reduction, sharpening).
8. ISP returns image + metadata.
9. HAL wraps buffers → returns to framework.
10. Framework sends results back to app.

---

### **Q2: What happens inside `process_capture_request()`?**
**Answer:**
- Extract exposure/focus/awb metadata.
- Validate stream buffers.
- Queue request to HAL threads.
- Communicate with ISP.
- Prepare output buffers.
- Fill result metadata.
- Return results.

---

### **Q3: How does HAL3 interact with ISP?**
**Answer:**
- HAL sends sensor settings (exposure time, gain).
- ISP performs 3A + image processing.
- ISP produces processed buffers.
- HAL retrieves ISP output and fills metadata.

---

# 🎯 Next Steps
Choose one for Day 2:
- **A)** Two Sum + HAL3 Request/Result Model
- **B)** Camera internals first
- **C)** More DSA
- **D)** Mix (recommended)

Which one do you want for Day 2?

