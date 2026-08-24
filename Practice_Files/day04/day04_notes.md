# Day 4 — Priority queue and binary min-heap

Today has **no pthreads**. The goal is to understand the data structure that
will later choose which coder request should be served first.

## Study order

1. Run `01_heap_indices.c` and draw the printed tree on paper.
2. Run `02_push_attempt.c`. It intentionally fails its heap-order checks.
3. Implement `heap_up()` in `02_push_attempt.c` before reading the walkthrough.
4. Compare your reasoning with `03_push_walkthrough.c`.
5. Implement `heap_down()` in `04_pop_attempt.c` before reading its walkthrough.
6. Compare your reasoning with `05_pop_walkthrough.c`.
7. Run `06_request_priority.c` and predict every comparison before reading the
   output.

Build an exercise with:

```sh
cc -g3 -O0 -Wall -Wextra -Werror 01_heap_indices.c -o 01_heap_indices
```

There is no `-pthread` today because the heap is being learned independently.

## What is a priority queue?

A normal queue serves the oldest item. A priority queue serves the item that
currently has the highest priority. In Codexion, "highest priority" means the
request with the **smallest comparison key**:

```text
FIFO: smaller arrival order wins
EDF:  smaller burnout deadline wins
```

We will implement that priority queue using a **binary min-heap**.

## Heap shape and array layout

A binary heap is a complete binary tree. Every level is filled from left to
right, so it can be stored compactly in an array.

```text
          2                 index:       0
        /   \                           / \
       5     3                         1   2
      / \                             / \
     9   7                           3   4

array = [2, 5, 3, 9, 7]
```

For a node at index `i`:

```c
left = 2 * i + 1;
right = 2 * i + 2;
parent = (i - 1) / 2;
```

Only calculate the parent when `i > 0`.

## The min-heap rule

Every parent must be less than or equal to both of its children:

```text
parent <= left child
parent <= right child
```

This does **not** mean the whole array is sorted. It only guarantees that the
smallest item is at index `0`, so `peek` is fast.

For example, this is a valid min-heap:

```text
[2, 5, 3, 9, 7]
```

`5` appears before `3`, but both are greater than their parent `2`.

## Push and heap-up

To insert a value:

1. Put it at the next free array position.
2. Compare it with its parent.
3. If it is smaller, swap them.
4. Continue until it is not smaller than its parent or it reaches index `0`.

```text
before:       [2, 5, 3, 9, 7]
append 1:     [2, 5, 3, 9, 7, 1]
swap with 3:  [2, 5, 1, 9, 7, 3]
swap with 2:  [1, 5, 2, 9, 7, 3]
```

## Peek

`peek` reads index `0` without removing it. An empty heap has no valid peek,
so your function must report failure rather than reading outside the array.

## Pop and heap-down

Removing index `0` would leave a hole. Fill that hole with the last item, make
the heap one item smaller, and move the replacement downward:

1. Compare it with its existing children.
2. Choose the **smaller child**.
3. Swap only if the parent is greater than that child.
4. Continue until the heap rule is restored.

The smaller-child step is important. Choosing the wrong child can leave the
other child smaller than its parent.

## Heap state

The practice heap has a fixed capacity to keep memory management out of today's
lesson:

```c
typedef struct s_heap
{
    int items[16];
    int size;
}   t_heap;
```

- `size` is the number of valid items.
- Valid indices are `0` through `size - 1`.
- Push must reject a full heap.
- Peek and pop must reject an empty heap.

## FIFO, EDF, and deterministic ties

A project request will contain more than one value:

```c
typedef struct s_request
{
    int         coder_id;
    long long   arrival_order;
    long long   deadline_ms;
}   t_request;
```

The heap needs a comparison function rather than directly comparing integers.

For FIFO:

```text
smaller arrival_order wins
```

For EDF:

```text
smaller deadline_ms wins
if deadlines are equal, smaller arrival_order wins
if both are equal, smaller coder_id wins
```

The extra comparisons make the result deterministic: the same inputs always
produce the same winner.

## Common mistakes

1. Treating the heap array as a fully sorted array.
2. Calculating the parent of index `0`.
3. Forgetting to check whether a child index is below `size`.
4. Comparing with the left child without checking whether the right child is
   smaller.
5. Swapping equal-priority items forever.
6. Reading index `0` when the heap is empty.
7. Mixing up heap capacity with current heap size.

## Paper exercise before coding

Start with an empty min-heap and insert:

```text
8, 3, 6, 1, 5
```

After every insertion, write:

- the array,
- the inserted item's index,
- its parent's index,
- every swap.

Then pop twice and write every smaller-child decision.

## End-of-day check

Explain these without reading the code:

1. Why is the smallest item always at index `0`?
2. Why is a heap array not necessarily sorted?
3. Why does push begin at the end of the array?
4. Why must heap-down select the smaller child?
5. What happens when `peek` is called on an empty heap?
6. What is the FIFO comparison key?
7. What is the EDF comparison key?
8. Why does EDF need a tie-breaker?

You are ready for Day 5 when you can draw push and pop on paper and explain the
reason for every swap.
