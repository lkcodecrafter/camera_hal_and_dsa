#include<iostream>
using namespace std;


//Q1. Reverse array
void reverseArray(int nums [5]){
    int left = 0;
    int right = 5-1;
    while(left<right){
        int temp = nums[left];
        nums[left]=nums[right];
        nums[right]=temp;
        left++;
        right--;
    }
}

//Q2. Palindrome checking



int main(int argc, char const *argv[])
{
    int numbers[5] = {10, 20, 30, 40, 50}; // Declares and initializes 'numbers'
    reverseArray(numbers);

    for(auto n : numbers){
        cout<<n << " ";
    }

    return 0;
}
