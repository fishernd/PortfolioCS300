---
title: CS 300 Project
subtitle: Data Structures Runtime Analysis
author: Nathan Fisher
email: nathan.fisher2@snhu.edu
university: Southern New Hampshire University
date: August 15, 2025
---

+--------------------+------------------------------------------+------------------+
| Data Structure     | Runtime Complexity                       | Space Complexity |
|                    | (insert – worst case)                    |                  |
+:===================+:========================================:+:================:+
| Vector             | O(1)                                     | O(n)             |
+--------------------+------------------------------------------+------------------+
| Hash Table         | O(n)[^1]                                 | O(n)             |
+--------------------+------------------------------------------+------------------+
| Binary Search Tree | O(n)                                     | O(n)             |
+====================+==========================================+==================+
table: Figure 1: Runtime and space efficiency

**Figure 1** shows that the runtime and space complexity are equivalent for the hash table
and tree data structures, with the vector being significantly faster for insertion, *using
the criteria suggested*. This would suggest that the vector is the best choice, with the
other two structures being equally bad in comparison. However, this is quite misleading as
not only is this figure showing the worst-case performance, but it is ignoring all of the
other operations that the program is expected to perform. Before making a recommendation I
will briefly go into a little more detail on each of the three, taking into account a
search operation and what is required to print a sorted course listing. 

+--------------------+-----------------------+--------------------+------------------+
| Data Structure     | Runtime Complexity    | Runtime Complexity | Space Complexity |
|                    | (insert – worst case) | (insert – average) |                  |
+:===================+:=====================:+:==================:+:================:+
| Vector             | O(1)                  | O(1)               | O(n)             |
+--------------------+-----------------------+--------------------+------------------+
| Hash Table         | O(n)                  | O(1)               | O(n)             |
+--------------------+-----------------------+--------------------+------------------+
| Binary Search Tree | O(n)                  | O(h)[^2]           | O(n)             |
+====================+=======================+====================+==================+
table: Figure 2: Runtime and space efficiency (expanded)

By adding in the average time for the insertion algorithm we already begin to see a
different picture than if we only look at the worst case. Our insert operation has
the same relative complexity for both the Vector and the Hash Table, and the Binary
Search Tree shows a significant improvement, as our complexity is equal to the height
of the tree. However, we still are not evaluating all of the operations that will be
performed because we were told to ignore searching and sorting in our runtime analysis. 

If a vector is used, the initial loading of values will be predictably fast, as we
only need to keep the index of the next empty pointer to find the next memory address
for insertion. However, finding a specific course in such an unsorted list requires a
potentially expensive linear search. A larger problem is that the list must be sorted
before printing in alphanumeric order (the assumption being that the data being input
will not already be sorted). While there are various sorting algorithms to choose
from, even looking at a relatively good general-purpose algorithm like QuickSort gives
us a runtime complexity of `O(n log n)` average, and `O(n2)` in the worst case. 

Once sorted, finding a specific course would of course be quicker, as one could use
the much quicker binary search algorithm, where one compares the desired course number
with the middle element in the vector. If it is smaller, the process is repeated with
the middle element of the first half, if larger, with the middle element of the second
half. This is done recursively until the matching course is found. 

A hash table, while having a significantly worse worst-case insertion time, has an
average insertion time that is essentially the same as than that of the vector. This
average insertion time is equivalent to the complexity of the hashing algorithm in use.
The actual time will only be longer in the case of hash collisions. While we might
expect a few collisions with any particular hash function, we would have to have
literally every course hash to the same value in order to get the worst case. That is
a statistical near-impossibility. The real strength of the hash table over the vector
is that a search is just about the same average runtime as an insertion, that is to
say, only as expensive as the hash function assuming no hash collision has occurred.
However, the hash table falls down when it comes to printing a sorted list. Just about
the only way to do this is to use a temporary vector or tree which can then be sorted,
not only doubling the space requirement but also requiring the same sorting operation
as the vector. 

Finally, we come to the Binary Search Tree. While the worst-case scenario for insertion
is O(N) where N is the number of elements, in most cases the tree will be relatively
balanced. A binary tree with three levels only requires at most three comparisons to
find the correct place to insert a new node, or to find a specific node, but can have
seven elements. A four-level tree would require at most four comparisons for those
operations but contain up to fifteen elements. Searching for a specific course would
be on average much faster than the Vector, and only slightly slower than the hash table.
Where the Binary Search Tree really shines, however, is that it requires no separate
sorting step because the elements are already sorted. In practice, because one of the
requirements is the ability to print the course list in alphanumeric order, this is a
huge advantage, particularly over the hash table, which has the same drawback in
relation to sorting as the vector plus the additional problem of requiring so much
extra space for the sorting step. 

Therefore, my recommendation is that a Binary Search Tree be used as the basis of the
program. While it might at first not be the obvious choice, overall it has the best mix
of properties for this application and will likely be the most performant overall.

[^1]: Where ‘n’ is the number of elements
[^2]: Where ‘h’ is the height of the tree 
