# Day 4 – Google Camera HAL + DSA (Mix Mode — Recommended)

Today’s plan continues the mixed track: **Trees (DFS/BFS) + Camera 3A (AE/AF/AWB)** + more Qualcomm CAMX/CHI insights.
This is structured exactly like Day 3.

---
# 🟦 Part 1 — DSA: Trees (DFS/BFS)
Tree problems are extremely common at Google.
We cover:
- Traversals (DFS, BFS)
- Binary tree problems
- Recursion breakdowns

---
## ⭐ Problem 1: Binary Tree Level Order Traversal (BFS)

### **Idea:**
Use queue → process each level → push children.

### **C++ Code:**
```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> ans;
    if (!root) return ans;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int n = q.size();
        vector<int> level;

        for (int i = 0; i < n; i++) {
            TreeNode* cur = q.front();
            q.pop();
            level.push_back(cur->val);

            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
        ans.push_back(level);
    }
    return ans;
}
```

---
## ⭐ Problem 2: Max Depth of Binary Tree (DFS + Recursion)

### **Idea:**
Depth = 1 + max(leftDepth, rightDepth)

### **C++ Code:**
```cpp
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + max(maxDepth(root->left), maxDepth(root->right));
}
```

---
## ⭐ Problem 3: Validate Binary Search Tree

### **Idea:**
Use DFS with min/max bounds.

### **C++ Code:**
```cpp
bool valid(TreeNode* node, long low, long high) {
    if (!node) return true;
    if (node->val <= low || node->val >= high) return false;
    return valid(node->left, low, node->val) && valid(node->right, node->val, high);
}

bool isValidBST(TreeNode* root) {
    return valid(root, LONG_MIN, LONG_MAX);
}
```

---
# =========================
# 🟩 Part 2 — Camera 3A Deep Dive (AE / AF / AWB)
# =========================

3A = **Auto Exposure, Auto Focus, Auto White Balance**.
These are tightly integrated in Qualcomm CAMX + CHI pipelines.
Google expects high-level understanding.

---
# ⭐ 1. Auto Exposure (AE)
AE adjusts:
- exposure time (shutter)
- analog/digital gain (ISO)
- frame duration

### **AE Flow:**
```
HAL3 settings → CHI → CAMX 3A module → sensor driver/ISP → applied exposure → reported as result metadata
```

### Key concepts:
- AE converges over frames
- Uses scene histogram, luminance, face detection
- Controls exposure ramp-down to avoid flicker

---
# ⭐ 2. Auto Focus (AF)
AF controls lens movement.

### AF Modes:
- CAF: continuous auto-focus
- AF_TRIGGER: single scan

### AF Flow:
```
HAL3 AF trigger → CHI → CAMX PDAF / laser / contrast algorithm → lens driver → AF state → result metadata
```

Qualcomm uses PDAF (Phase Detect AF) + vendor tuning.

---
# ⭐ 3. Auto White Balance (AWB)
AWB adjusts color gains.

### AWB Flow:
```
HAL3 AWB mode → CHI → CAMX AWB algorithm → RGB gains → IFE/IPE → reported as metadata
```

---
# ⭐ How 3A interacts with HAL3 / CHI / CAMX

### End-to-end path:
```
HAL3 control metadata
   ↓
CHI translation
   ↓
CAMX 3A engine
   ↓ (gives applied values)
ISP tuning modules
   ↓
CAMX result metadata
   ↓
HAL3 CaptureResult
```

Interviewers love this.

---
# 🟥 Part 3 — CHI/CAMX: How 3A Metadata is Routed

Example metadata tags:
- `android.control.aeMode`
- `android.control.aeRegions`
- `android.control.afTrigger`
- `android.control.awbMode`

Vendor tags:
- `com.qti.chi.aeExpComp`
- `com.qti.chi.awbConvergence`

### Routing:
```
Framework metadata → HAL3 → CHI vendor translation → CAMX IQ modules → ISP → CAMX → HAL3 → Framework
```

---
# ⭐ Still Capture vs Preview – 3A Behavior

### Preview:
- fast AF
- AE tries stable exposure
- AWB runs continuously

### Still Capture:
- AE pre-capture sequence
- AF triggers final lens scan
- AWB locked before capture (for consistency)

---
# ⭐ CAMX Blocks where 3A interacts

| Block | Role |
|-------|------|
| **IFE** | stats collection (AF/AE/AWB) |
| **BPS** | RAW→demosaic + early tuning |
| **IPE** | final color + tone-mapping for AE/AWB |
| **Sensor Driver** | AE exposure + gains |

---
# 🟧 Trees + Camera Combined Interview

### Q1: How would you process a preview + raw stream together?
Expect:
- multi-output
- CAMX pipeline branching
- buffer flow insight

### Q2: How does 3A run during burst capture?
Expect:
- lock conditions
- converged states

### Q3: How is AF ensured stable during continuous tracking?
Expect:
- CAF algorithm basics
- PDAF usage

---
# 🟦 Day 4 Summary
Today you learned:
- Binary trees: BFS/DFS fundamentals for Google rounds
- 3A internals: AE/AF/AWB
- How 3A flows through HAL3 → CHI → CAMX → ISP
- Additional Qualcomm architecture insights

---
# Next: Choose Day 5

Select one:
- **A)** Binary Tree Advanced (LCA / Build Tree)
- **B)** RAW pipeline deep dive (IFE/BPS/IPE in detail)
- **C)** Camera stream config + formats (YUV/NV12/RAW/JPEG)
- **D)** Mix (recommended)

