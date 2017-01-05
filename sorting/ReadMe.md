## ReadMe for COMP 15 HW5
## Sorting Assignment


1. The purpose of this program is to sort arrays of integers

2.sortumns.cpp is the main file that reads in data and calls one of three
sorting algorithms

IntVector.h and IntVector.cpp are the header and implementation files of
intvector which is an implementation of a dynamic array. One can replace
replace the value at an index(or set if the index does not hold a value)
add to the end of the array or get the value at a specific index.

3. use included Makefile to compile

4.The data structure used is a dynamic array. Only some  public functions are
included as described above

5. The algorithms of the following sorts have been implemented:

Bubble sort:
Compares each element with its next one and swaps if needed. Loops through
the entire list the first, then the entire loop except for the last element 
the second time etc. The complexity is O(nsquared)

Insertion sort:
breaks the problem into n subproblems and sorts each one at
a time. each subset is equal to the last one with the
addition of one new element(nsquared complexity). Starts with just the 
first element. Complexity (n squared)

Quicksort:
-uses a pivot value in the middle
-loops through data from the left and ther right
	until a value larger than the pivot is found from
	the left and smallet than the pivot from the right
-swaps those two values
-uses recursion as the list is broken in subrpoblems to sort 
each one

-complexity O(nlogn)

6. No collaboration
