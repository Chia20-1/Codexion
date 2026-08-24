# How Day 4 connects to Codexion

This guide explains why you are learning a binary min-heap and where it will
eventually fit inside Codexion. Read it after `day04_notes.md`, but before trying
to design the complete scheduler.

## 1. The subject requirement

Codexion accepts a `scheduler` argument whose value must be either `fifo` or
`edf`.

The subject defines the policies as follows:

- **FIFO**: the request that arrived first is served first.
- **EDF**: the coder with the earliest burnout deadline is served first.
- The EDF deadline is `last_compile_start + time_to_burnout`.
- Equal EDF deadlines require a deterministic tie-breaker.
- You must implement a priority queue using a heap. A standard-library priority
  queue is not allowed.

These requirements appear in the Global Rules and Mandatory Part of
`codexion.pdf`, especially subject pages 8, 11, and 12.

This means Day 4 is not an unrelated data-structure exercise. The heap will
become the ordering mechanism inside your scheduler.

## 2. The problem the heap solves

Imagine three coders are waiting:

```text
Coder 1 requested at order 20; deadline = 900 ms
Coder 2 requested at order 21; deadline = 700 ms
Coder 3 requested at order 22; deadline = 800 ms
```

Under FIFO, the desired order is:

```text
Coder 1 -> Coder 2 -> Coder 3
```

Under EDF, the desired order is:

```text
Coder 2 -> Coder 3 -> Coder 1
```

The requests are the same. Only the comparison policy changes.

A min-heap keeps the request that should win next at index `0`:

```text
push request
     |
     v
heap_up restores priority order
     |
     v
peek index 0 to find the next request
     |
     v
pop granted request
     |
     v
heap_down restores priority order
```

This is why Day 4 practises `push`, `peek`, `pop`, `heap_up`, and `heap_down`.

## 3. From practice integers to project requests

The first exercises use integers:

```c
int items[16];
```

That makes the heap movement easy to see. In the project, an item will need to
describe a coder's request. Conceptually, it may contain information like:

```c
typedef struct s_request
{
	int			coder_id;
	int			left_dongle;
	int			right_dongle;
	long long	arrival_order;
	long long	deadline_ms;
}t_request;
```

Do not copy this structure into the project yet. Its final fields depend on the
architecture you choose on Day 5. For now, notice the important change:

```text
integer heap: compare two integer values
request heap: compare two request structures
```

The tree calculations do not change:

```text
left   = 2 * i + 1
right  = 2 * i + 2
parent = (i - 1) / 2
```

Only the meaning of "comes before" changes.

## 4. FIFO comparison

FIFO means First In, First Out. A request needs an arrival-order number that
increases whenever a new request enters the scheduler:

```text
first request  -> arrival_order 0
second request -> arrival_order 1
third request  -> arrival_order 2
```

For FIFO:

```text
A comes before B when A.arrival_order < B.arrival_order
```

Do not use `coder_id` as the main FIFO priority. Coder 1 does not automatically
deserve a dongle before Coder 2. The actual request arrival order decides.

Later, the arrival counter will be shared state. It must be read and incremented
while holding the scheduler mutex so that two threads cannot receive the same
order number. That is where Day 2's mutex lesson reconnects with Day 4.

## 5. EDF comparison

EDF means Earliest Deadline First. Codexion defines a coder's deadline as:

```text
deadline = last_compile_start + time_to_burnout
```

The deadline resets when compiling **starts**, not when the coder requests
dongles and not when compiling finishes.

For EDF:

```text
A comes before B when A.deadline_ms < B.deadline_ms
```

This connects directly to Day 3:

- `now_ms()` gives you time in milliseconds.
- `last_compile_start` is protected shared state.
- the monitor also reasons about the same burnout deadline.

The scheduler and monitor use the deadline for different purposes:

```text
scheduler: who is more urgent?
monitor:   has anyone already missed the deadline?
```

## 6. Why EDF needs a tie-breaker

Two requests can have the same deadline. The subject requires deterministic
behaviour even if that case is rare.

A sensible comparison sequence to study is:

```text
1. Earlier deadline wins.
2. If equal, earlier arrival order wins.
3. If still equal, smaller coder ID wins.
```

The third comparison should almost never be needed if arrival orders are unique,
but it makes the policy fully defined.

Be precise about the difference:

```text
deadline is the EDF priority
arrival order and coder ID are tie-breakers
```

If you compare coder IDs first, you are no longer implementing EDF.

## 7. What the heap does not solve

This is the most important boundary to understand.

The heap answers:

> Which waiting request has the best scheduling priority?

It does not answer:

- Are both neighbouring dongles currently free?
- Has each dongle's cooldown finished?
- Which mutex protects a dongle's state?
- Should a waiting coder sleep or wake?
- Has a coder burned out?
- Has the simulation stopped?
- Can another non-conflicting request safely make progress?

Those belong to later parts of the project:

```text
Day 2: mutex protection
Day 3: condition-variable waiting and time
Day 4: request priority
Day 7: dongle state and cooldown
Days 8-9: FIFO/EDF scheduler integration
Day 10: monitor and stopping
```

A correct heap can exist inside an incorrect scheduler. You will eventually need
all these pieces to agree.

## 8. Pair requests in your project plan

Your 12-day plan proposes that a coder requests its two neighbouring dongles as
one pair:

```text
request both dongles
        |
        v
enter scheduler heap
        |
        v
wait until the request can be granted
        |
        v
receive both dongles together, or receive neither
```

This design prevents a coder from holding one dongle while waiting for the
second. It breaks the **hold-and-wait** condition involved in circular deadlock.

The heap therefore stores a request for a pair, not two unrelated requests made
by the same coder. The scheduler must still protect each dongle's state with a
mutex as required by the subject.

Do not implement this threaded integration on Day 4. First prove that your heap
can order simple values and simple request records correctly without concurrency.

## 9. Priority versus eligibility

A request may have the best priority but still be temporarily impossible to
grant because one of its dongles is owned or cooling down.

```text
priority:    how urgent is this request?
eligibility: can its complete dongle pair be granted now?
```

Do not mix these ideas into one number. For example, changing a request's EDF
deadline because its dongle is cooling down would change the scheduling policy.

How to select among requests when the highest-priority request is temporarily
ineligible is an integration decision for Days 7-9. Test it carefully against:

- FIFO/EDF ordering,
- fair arbitration for contended dongles,
- cooldown correctness,
- deadlock prevention,
- the EDF liveness requirement for feasible parameters.

Day 4 gives you the ordering tool; it does not remove the need to reason about
those rules.

## 10. How the earlier days combine

By the end of Day 4, your mental model should look like this:

```text
Coder thread creates a request
        |
        v
Scheduler mutex protects queue state       <- Day 2
        |
        v
Heap orders the waiting requests            <- Day 4
        |
        v
Condition variable lets coder sleep/wake    <- Day 3
        |
        v
Scheduler checks both dongles and cooldown  <- Day 7
        |
        v
Both dongles granted; compiling starts
        |
        v
last_compile_start and deadline reset       <- Day 3
```

Day 1 supplies the coder threads that will execute this flow.

## 11. How each Day 4 exercise helps

### `01_heap_indices.c`

Teaches where a request's parent and children live in the array. You need this
for every heap movement.

### `02_push_attempt.c`

Teaches how a newly submitted request moves toward index `0` when it has better
priority than its parent.

### `03_push_walkthrough.c`

Shows every swap. In the project, imagine the printed integer is a FIFO order or
an EDF deadline.

### `04_pop_attempt.c`

Teaches how to remove the selected request and restore the heap after moving the
last request to index `0`.

### `05_pop_walkthrough.c`

Shows why heap-down must choose the smaller, higher-priority child.

### `06_request_priority.c`

Makes the direct project transition: requests are compared using FIFO or EDF
fields instead of plain integer values.

## 12. Tests to carry into the project

Before using the heap with threads, test it independently.

### Basic behaviour

- Peek and pop an empty heap.
- Push one request and pop it.
- Fill the heap to capacity.
- Attempt one extra push.
- Pop everything and verify the expected order.

### FIFO behaviour

Insert arrival orders in a scrambled order:

```text
8, 3, 6, 1, 5
```

Expected pop order:

```text
1, 3, 5, 6, 8
```

### EDF behaviour

Insert deadlines:

```text
900, 700, 800, 700
```

Expected deadline order:

```text
700, 700, 800, 900
```

For the two `700` requests, verify the arrival-order tie-breaker.

### Invariant check

After every push and pop, verify:

```text
for every child index i > 0:
    parent at (i - 1) / 2 must not come after child i
```

This checks the heap property without requiring the entire array to be sorted.

## 13. Stop point for Day 4

Do not build the complete Codexion scheduler today. Your Day 4 goal is narrower:

1. You can draw the heap as a tree and an array.
2. You can implement integer `push`, `peek`, and `pop`.
3. You can explain every heap-up and heap-down swap.
4. You can compare two request records under FIFO.
5. You can compare two request records under EDF with deterministic ties.
6. You understand that heap priority and dongle eligibility are different.

Once those ideas are clear, Day 5 can turn the exercise structures into planned
project structures without mixing data-structure bugs with thread bugs.

## Check your understanding

Answer these before moving on:

1. Coder A arrived first, but Coder B has an earlier deadline. Who wins under
   FIFO? Who wins under EDF?
2. Why should `last_compile_start` be recorded when compiling starts instead of
   when a dongle request is created?
3. If the heap root's dongles are cooling down, does that mean the heap is
   broken? Why not?
4. Which earlier lesson protects the arrival-order counter from a race?
5. Why is a working heap not enough to guarantee EDF liveness?
