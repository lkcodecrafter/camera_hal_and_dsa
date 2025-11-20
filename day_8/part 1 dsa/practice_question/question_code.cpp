#include<iostream>
#include<vector>
#include<unordered_map>
using namespace std;

/*
✅ Problem 1: Longest Substring Without Repeating Characters

### **Idea:**
Use window + HashMap to track last index of each character.
Shrink window when you see a duplicate.
*/
 
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

int main() {
 

    return 0;
}