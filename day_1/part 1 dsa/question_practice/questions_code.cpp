#include <iostream>
#include <vector>
using namespace std;

// Q1. Reverse array
void reverseArray(int nums[5])
{
    int left = 0;
    int right = 5 - 1;
    while (left < right)
    {
        int temp = nums[left];
        nums[left] = nums[right];
        nums[right] = temp;
        left++;
        right--;
    }
}

// Q2. Palindrome checking
bool isPalindrome(vector<int> &arr)
{
    int left = 0;
    int right = arr.size() - 1;
    while (left < right)
    {
        if (arr[left] != arr[right])
        {
            return false;
        }
        left++;
        right--;
    }
    return true;
}
// one more optimize code
void palindrome(int arr[], int n)
{
    // Initialise flag to zero.
    int flag = 0;

    // Loop till array size n/2.
    for (int i = 0; i <= n / 2 && n != 0; i++)
    {

        // Check if first and last element are different
        // Then set flag to 1.
        if (arr[i] != arr[n - i - 1])
        {
            flag = 1;
            break;
        }
    }

    // If flag is set then print Not Palindrome
    // else print Palindrome.
    if (flag == 1)
        cout << "Not Palindrome";
    else
        cout << "Palindrome";
}

// ✅ Problem 3: Two Sum (Sorted Array)
// Goal: find two numbers that sum to a target.

vector<int> twoSumSorted(vector<int> &arr, int target)
{
    int left = 0;
    int right = arr.size() - 1;
    while (left < right)
    {
        int sum = arr[left] + arr[right];
        if (sum == target)
        {
            return {left, right};
        }
        else if (sum < target)
        {
            left++;
        }
        else
        {
            right--;
        }
    }
    return {-1, -1};
}

// ✅ Problem 4: Remove Duplicates from Sorted Array
/*Explanation:
Two pointers:

i → points to place where next unique element goes
j → scans array
*/
int removeDuplicates(vector<int> &nums)
{
    int i = 0;
    for (int j = 1; j < nums.size(); j++)
    {
        if (nums[i] != nums[j])
        {
            i++;
            nums[i] = nums[j];
        }
    }
    return i + 1; // unique elements at nums[0..4]: [0, 1, 2, 3, 4]
}

/*
✅ Problem 5: Merge Two Sorted Arrays
Explanation:
Use two pointers to merge while comparing elements.
*/
vector<int> mergeSorted(vector<int> &a, vector<int> &b)
{

    int i = 0;
    int j = 0;
    vector<int> result;
    while (i < a.size() && j < b.size())
    {
        if (a[i] < b[j])
        {
            result.push_back(a[i++]);
        }
        else
        {
            result.push_back(b[j++]);
        }
    }

    while (i < a.size())
    {
        result.push_back(a[i++]);
    }
    while (j < b.size())
    {
        result.push_back(b[j++]);
    }

    return result;
}
int main(int argc, char const *argv[])
{
    int numbers[5] = {10, 20, 30, 40, 50}; // Declares and initializes 'numbers'
    // reverseArray(numbers);

    vector<int> arr = {5, 5, 0, 3, 3};
    cout << isPalindrome(arr) << endl;

    // below is sorted array
    vector<int> arr1 = {1, 2, 3, 4, 5};
    vector<int> ans = twoSumSorted(arr1, 3);
    for (auto n : ans)
    {
        cout << n << " ";
    };
    cout << endl;

    vector<int> nums = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    int k = removeDuplicates(nums);
    cout << "Number of unique elements: " << k << endl;

    cout << "Array after removing duplicates: ";
    for (int i = 0; i < k; i++)
    {
        cout << nums[i] << " ";
    }
    cout << endl;

    vector<int> a = {1, 3, 5, 7};
    vector<int> b = {2, 4, 6, 8, 10};

    vector<int> merged = mergeSorted(a, b);

    cout << "Merged Array: ";
    for (int x : merged) {
        cout << x << " ";
    }
    cout << endl;

    return 0;
}
