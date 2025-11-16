#include<iostream>
#include<vector>
#include<unordered_map>
using namespace std;

/*
✅ Problem 1: Two Sum (Unsorted Array)
Approach: HashMap
Store value → index
For each element, check if target - value exists
*/
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int,int> mp;
    for(int i =0; i<nums.size();i++){
        int need = target - nums[i];
        if(mp.count(need)){
            return {mp[need], i};
        }
        mp[nums[i]]=i;
    }
    return {-1,-1};
}

/*
✅ Problem 2: Maximum Sum of Subarray of Size K (Fixed Window)
Approach:
Create a window of size k
Slide by removing old element and adding new one
*/
int maxSumSubarrayK(vector<int>& arr, int k) {
    int windowSum = 0;
    int maxSum = 0;

    for (int  i = 0; i < k; i++)
    {
        windowSum+= arr[i];
    }

    maxSum = windowSum;
    for (int i = k; i < arr.size(); i++)
    {
        /*
        Sliding window logic:
        NEW window sum = OLD window sum + (value coming in) - (value going out)
        That's it.
        */
        windowSum+= arr[i] - arr[i-k];
        maxSum= max(windowSum,maxSum);
    }

    return maxSum;
    
/*
✔ Visual Explanation

Let’s say:

i = 3
k = 3


Window size = 3

Index:   0  1  2  3  4  5
Array:   2  3  5  2  9  7
Window:     [3  5  2]


arr[i] = arr[3] = 2 (coming in)

arr[i-k] = arr[0] = 2 (going out)

So:

windowSum = windowSum + 2 - 2


*/


}    

int main() {
    vector<int> nums = {2, 7, 11, 15};
    int target = 9;

    vector<int> result = twoSum(nums, target);

    cout << "Indices: " << result[0] << ", " << result[1] << endl;


    vector<int> arr = {2, 3, 5, 2, 9, 7, 1};
    int k = 3;

    cout << "Max sum of subarray of size " << k << " = "
         << maxSumSubarrayK(arr, k) << endl;

    return 0;
}