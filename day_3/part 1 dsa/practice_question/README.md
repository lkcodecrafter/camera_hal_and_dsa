# Sliding Window String Problems --- With Code Understanding & Dry Runs

This README contains **detailed explanations**, **C++ code**, and
**step‑by‑step dry runs** for:

1.  **Longest Substring Without Repeating Characters**\
2.  **Smallest Distinct Window**

------------------------------------------------------------------------

# 1. Longest Substring Without Repeating Characters

## 🔍 Problem

Given a string `s`, find the **length** of the **longest substring**
that contains **no repeating characters**.

Time Complexity: **O(n)** using Sliding Window.

------------------------------------------------------------------------

# 🧠 Understanding the Code

We maintain:

-   `left` → start of window\
-   `right` → end of window\
-   `lastIndex[c]` → last seen index of char `c`\
-   If we see a repeating character inside the window, move `left`.

This ensures window **always has unique characters**.

------------------------------------------------------------------------

# ✅ C++ Code

``` cpp
#include <bits/stdc++.h>
using namespace std;

int lengthOfLongestSubstring(const string &s) {
    int n = s.size();
    vector<int> lastIndex(256, -1);   // last index of each char
    int maxLen = 0;
    int left = 0;

    for (int right = 0; right < n; ++right) {
        char c = s[right];

        // If char already inside window, move left
        if (lastIndex[c] >= left) {
            left = lastIndex[c] + 1;
        }

        lastIndex[c] = right;                        // update last index
        maxLen = max(maxLen, right - left + 1);      // update answer
    }
    return maxLen;
}
```

------------------------------------------------------------------------

# 🧪 Dry Run Example: `"pwwkew"`

  Step   Right   Char   Action                                         Window   maxLen
  ------ ------- ------ ---------------------------------------------- -------- --------
  0      0       p      add                                            p        1
  1      1       w      add                                            pw       2
  2      2       w      repeated → shift left to index after old 'w'   w        2
  3      3       k      add                                            wk       2
  4      4       e      add                                            wke      **3**
  5      5       w      repeated → move left                           kew      3

Final answer = **3** → `"wke"`.

------------------------------------------------------------------------

# 2. Smallest Distinct Window

## 🔍 Problem

Find the **length of the smallest substring** that contains **ALL
distinct characters** of the full string.

Example: `"aabcbcdbca"` → Output = **4** (`"dbca"`)

------------------------------------------------------------------------

# 🧠 Understanding the Code

Steps:

1.  Count **distinct characters** in entire string → `distinctCount`
2.  Use sliding window to include characters until window has all
    distinct ones.
3.  Once valid, shrink window from the left to get minimum size.

We track:

-   `freq[c]` → frequency inside window\
-   `countInWindow` → number of distinct chars currently covered\
-   `minLen` → result we keep updating

------------------------------------------------------------------------

# ✅ C++ Code

``` cpp
#include <bits/stdc++.h>
using namespace std;

int smallestDistinctWindow(const string &s) {
    int n = s.size();
    if (n == 0) return 0;

    // Find total distinct characters
    vector<bool> present(256, false);
    int distinctCount = 0;
    for (char c : s) {
        if (!present[c]) {
            present[c] = true;
            distinctCount++;
        }
    }

    vector<int> freq(256, 0);
    int countInWindow = 0;
    int minLen = INT_MAX;
    int start = 0;

    for (int end = 0; end < n; ++end) {
        char c = s[end];
        freq[c]++;

        if (freq[c] == 1)
            countInWindow++;  // new distinct char

        // When window contains all distinct characters
        while (countInWindow == distinctCount) {
            minLen = min(minLen, end - start + 1);

            // Shrink from left
            char sc = s[start];
            freq[sc]--;
            if (freq[sc] == 0)
                countInWindow--;
            start++;
        }
    }
    return (minLen == INT_MAX) ? 0 : minLen;
}
```

------------------------------------------------------------------------

# 🧪 Dry Run Example: `"aabcbcdbca"`

Distinct characters = `{a, b, c, d}` → `distinctCount = 4`

  ----------------------------------------------------------------------------
  end      char       Window                   countInWindow       Action
  -------- ---------- ------------------------ ------------------- -----------
  0        a          a                        1                   expand

  1        a          aa                       1                   expand

  2        b          aab                      2                   expand

  3        c          aabc                     3                   expand

  4        b          aabcb                    3                   expand

  5        c          aabcbc                   3                   expand

  6        d          aabcbcd                  **4**               valid →
                                                                   shrink

  6→       window     reduces to               **4**               update
  shrink   becomes                                                 minLen = 4
           `bcdbca`                                                
           then                                                    
           `cdbca`                                                 
           then                                                    
           `dbca`                                                  
  ----------------------------------------------------------------------------

Answer = **4** (`"dbca"`)

------------------------------------------------------------------------

# 🎯 Summary

  -------------------------------------------------------------------------
  Problem            Task         Technique                Time
  ------------------ ------------ ------------------------ ----------------
  Longest substring  Find longest Sliding Window + last    O(n)
  without repeating  unique       index                    
                     substring                             

  Smallest distinct  Find         Sliding Window + freq    O(n)
  window             smallest     array                    
                     substring                             
                     containing                            
                     all distinct                          
                     chars                                 
  -------------------------------------------------------------------------

------------------------------------------------------------------------

If you need diagrams or Java versions too, I can add them!
