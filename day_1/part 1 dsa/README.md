============================
🟦 PART 1 — DSA: Two Pointers Pattern (Google Favorite)
============================
⭐ Simple Explanation

Two pointers =
You take two indexes, usually:

left (start)

right (end)

You move them depending on condition.

Best for:

Pair sum problems

Removing duplicates

Reversing

Merging

Searching sorted data

🔶 Example Problem 1

Reverse an array in-place.

Visualization
[1, 2, 3, 4]
 L        R


Swap & move:

[4, 2, 3, 1]
   L    R


Next:

[4, 3, 2, 1]

Dry Run

Input: arr = [10, 20, 30, 40, 50]

Step	L	R	Swap	Array
1	0	4	10 ↔ 50	[50,20,30,40,10]
2	1	3	20 ↔ 40	[50,40,30,20,10]
3	2	2	Stop	
Questions to Practice

Reverse array

Check if array is palindrome

Two-sum in sorted array

Merge two sorted arrays

If you want, I’ll solve them with you.