# Codexion — 12-Day Beginner Plan

> Goal: Finish the mandatory Codexion project in 12 days while learning the concurrency concepts well enough to explain and modify the code during evaluation.

## Overall Strategy

Treat Codexion as a **learning project first, implementation project second**.

Do not immediately try to build the whole program. Learn and test the core concepts separately before integrating them:

```text
Thread
  ↓
Mutex
  ↓
Condition variable
  ↓
Shared state + race conditions
  ↓
Timing
  ↓
Priority queue / heap
  ↓
Dongle arbitration
  ↓
Coder simulation
  ↓
Monitor
```

Aim for roughly **5–7 focused hours per day**.

## Core Resource Design Decision

Each compile attempt is a request for a **pair of neighbouring dongles**, not two
unrelated one-dongle requests. Use one scheduler/arbitration layer to queue these
pair requests and grant both dongles together only when both are free and their
cooldowns have expired.

This prevents a coder from holding one dongle while waiting for the other, breaking
the hold-and-wait condition that can cause a circular deadlock. Each dongle must
still have mutex-protected state, as required by the subject, but FIFO/EDF priority
belongs to the pair-request scheduler.

---

# Day 1 — POSIX Threads

## Learn

- Process vs thread
- What memory threads share
- Thread stack
- Passing data through `void *arg`
- `pthread_create`
- `pthread_join`
- Why thread execution order is unpredictable

## Exercises

1. Create one worker thread.
2. Print from both `main()` and the worker.
3. Create five threads and pass each one an ID.
4. Let multiple threads increment one shared integer without protection.

The final exercise should expose your first **race condition**.

## Notes

```
pthread_create()
    ↓
system allocates thread resources
    ↓
worker finishes
    ↓
resources are still retained because the thread is joinable
    ↓
pthread_join()
    ↓
resources can be released
```

## End-of-Day Goal

You should be able to explain:

> Threads execute independently but share process memory, so multiple threads may access the same data at the same time.

---

# Day 2 — Mutexes, Race Conditions, and Deadlocks

## Learn

```c
pthread_mutex_init()
pthread_mutex_lock()
pthread_mutex_unlock()
pthread_mutex_destroy()
```

Use a mutex to fix yesterday's shared-counter race condition.

Then deliberately create a deadlock:

```text
Thread A:
    lock mutex 1
    lock mutex 2

Thread B:
    lock mutex 2
    lock mutex 1
```

## Learn Coffman's Four Deadlock Conditions

1. Mutual exclusion
2. Hold and wait
3. No preemption
4. Circular wait

## End-of-Day Goal

Understand why checking and modifying shared resource state without synchronization is unsafe.

---

# Day 3 — Condition Variables and Timing

## Learn Condition Variables

```c
pthread_cond_wait()
pthread_cond_timedwait()
pthread_cond_signal()
pthread_cond_broadcast()
```

Mental model:

> A condition variable allows a thread to sleep until shared state may have changed instead of continuously polling.

Avoid busy waiting such as:

```c
while (!available)
    ;
```

## Learn Timing

Study:

```c
gettimeofday()
```

Create a helper conceptually like:

```text
now_ms()
```

Test elapsed time:

```text
start = now_ms()
sleep
end = now_ms()

elapsed = end - start
```

## Codexion Concept

A coder's burnout deadline is based on:

```text
last_compile_start + time_to_burnout
```

The deadline resets when compiling **starts**, not when it finishes.

## End-of-Day Goal

You can measure milliseconds and understand how a monitor can detect missed deadlines.

---

# Day 4 — Priority Queue / Binary Heap

Do not use pthreads today.

Codexion requires a custom priority queue implemented as a heap.

## Learn Heap Layout

Example:

```text
          2
        /   \
       5     3
      / \
     9   7
```

Array form:

```text
[2, 5, 3, 9, 7]
```

For index `i`:

```text
left   = 2 * i + 1
right  = 2 * i + 2
parent = (i - 1) / 2
```

## Implement Independently

- `push`
- `peek`
- `pop`
- heap-up
- heap-down

## FIFO Priority

Conceptually:

```text
priority = request arrival order
```

Earlier request wins.

## EDF Priority

Conceptually:

```text
priority = last_compile_start + time_to_burnout
```

Earlier deadline wins.

Add a deterministic tie-breaker.

## End-of-Day Goal

You can draw heap insertion/removal on paper and explain every swap.

---

# Day 5 — Project Architecture and Parsing

Now begin the actual Codexion project.

## Suggested Structure

```text
include/
    codexion.h

src/
    main.c
    parse.c
    init.c
    time.c
    thread.c
    coder.c
    monitor.c
    dongle.c
    scheduler.c
    heap.c
    log.c
    cleanup.c

Makefile
README.md
```

## Suggested Data Responsibilities

### `t_config`

Store:

- number of coders
- burnout time
- compile time
- debug time
- refactor time
- compile requirement
- dongle cooldown
- scheduler type

### `t_coder`

Store:

- coder ID
- last compile start
- compile count
- left/right dongle references
- simulation reference

### `t_dongle`

Store:

- current availability/owner
- cooldown deadline
- mutex

### `t_scheduler`

Store:

- heap of pair-acquisition requests
- scheduler mutex and condition variable
- request-order counter

### `t_sim`

Store:

- configuration
- coders
- dongles
- stop state
- logging mutex
- monitor synchronization

## Implement Today

- Argument validation
- Memory allocation
- Struct initialization
- Cleanup
- Makefile
- minimal README skeleton (update it throughout the project)

Example valid command:

```bash
./codexion 5 800 200 200 200 3 50 fifo
```

Reject:

- missing arguments
- negative values
- non-integers
- invalid scheduler
- malformed values

Remember: **no global variables**.

## End-of-Day Goal

The program validates arguments and initializes/cleans its data structures correctly.

---

# Day 6 — Coder Threads and Logging

Create:

```text
N coder threads
1 monitor thread
```

Then join and clean them correctly.

## Implement the Coder Lifecycle

Conceptually:

```text
compile
debug
refactor
repeat
```

You can still fake resource acquisition at first.

## Implement Serialized Logging

Expected messages:

```text
timestamp id has taken a dongle
timestamp id is compiling
timestamp id is debugging
timestamp id is refactoring
timestamp id burned out
```

Use one mutex around output so two threads cannot mix their messages.

## End-of-Day Goal

Multiple coder threads run concurrently and produce clean, non-interleaved logs.

---

# Day 7 — Dongles and Cooldown

Now model the actual shared resources.

There are normally:

```text
N coders
N dongles
```

Each coder uses its neighboring left and right dongles.

Special case:

```text
1 coder = 1 dongle
```

## Resource Model

```text
t_dongle:
    availability / owner
    cooldown_until
    mutex protecting its state

t_scheduler:
    heap of pair-acquisition requests
    mutex and condition variable
    request-order counter
```

Do **not** let a coder acquire one dongle and wait while holding it for the second.
The scheduler should grant the coder's left and right dongles as one operation once
both are available and off cooldown. This is the deadlock-prevention strategy you
will use and explain in the README/defence.

## Cooldown Logic

When released:

```text
cooldown_until = release_time + dongle_cooldown
```

A dongle cannot be granted again before that time.

## Test Example

```text
release at: 500 ms
cooldown:   100 ms
```

Must reject acquisition at:

```text
520
550
599
```

Can become available around:

```text
600+
```

## End-of-Day Goal

Dongle ownership is thread-safe, cooldown works, and the design can grant a complete
pair without a coder holding only one dongle.

---

# Day 8 — FIFO Scheduling

Integrate your heap.

Example request order:

```text
A requests first
B requests second
C requests third
```

FIFO grant order:

```text
A → B → C
```

## Pair-Request Flow

Conceptually:

```text
request left + right dongles together
      ↓
enter the scheduler heap
      ↓
wait
      ↓
become highest-priority request
      ↓
both dongles become available
      ↓
both cooldowns completed
      ↓
acquire both dongles atomically
```

Then integrate the pair acquisition:

```text
acquire two dongles
compile
release both
debug
refactor
repeat
```

## End-of-Day Goal

FIFO contention behaves predictably under multiple coder threads.

---

# Day 9 — EDF and Deadlock Prevention

Add EDF by changing the scheduler's comparison logic.

## FIFO

```text
priority = request order
```

## EDF

```text
priority = burnout deadline
```

where:

```text
deadline = last_compile_start + time_to_burnout
```

## Study Deadlock Again

Dangerous situation:

```text
C1 owns D1, waits for D2
C2 owns D2, waits for D3
C3 owns D3, waits for D1
```

Nobody can move.

Use the pair-request scheduler from Day 7: a coder receives both dongles together
or neither. Make sure you can explain **why this breaks the hold-and-wait Coffman
condition**.

Also consider starvation: EDF must keep feasible coders alive rather than indefinitely denying resources.

## End-of-Day Goal

Both FIFO and EDF work, and your dongle acquisition strategy does not rely on luck.

---

# Day 10 — Monitor and Simulation Stopping

Implement the separate monitor thread.

## Burnout Check

Conceptually:

```text
for every coder:
    deadline = last_compile_start + time_to_burnout

    if now >= deadline:
        stop simulation
        print burnout
```

Burnout logging needs high timing precision. Use a timed wait (for example,
`pthread_cond_timedwait()`) until the nearest burnout deadline or until a state
change wakes the monitor. Do not use a coarse polling loop: the subject requires the
burnout message within 10 ms of the actual deadline.

## Simulation Stop Conditions

The program stops when:

```text
1. Any coder burns out
```

or:

```text
2. Every coder reaches number_of_compiles_required
```

## Important Special Case

Test:

```text
1 coder
```

There is only one dongle, while compiling requires two.

The program should behave correctly rather than freezing forever.

Also ensure that once the simulation stops, normal coder messages do not continue printing afterward.

## End-of-Day Goal

Burnout and successful-completion termination both work reliably.

---

# Day 11 — Testing and Breaking the Program

Do not add features today.

Your job is to behave like an evaluator trying to destroy your implementation.

## Test Categories

- invalid input
- one coder
- two coders
- many coders
- very short burnout
- long cooldown
- compile time longer than burnout
- FIFO contention
- EDF contention
- simultaneous pair requests
- zero cooldown
- `number_of_compiles_required = 0` (if your parser accepts it, it should complete immediately)
- successful completion
- forced burnout
- repeated execution

## Invalid Input Examples

```bash
./codexion
./codexion -1 800 200 200 200 3 10 fifo
./codexion abc 800 200 200 200 3 10 fifo
./codexion 5 800 200 200 200 3 10 banana
```

## Memory Testing

Example:

```bash
valgrind --leak-check=full ./codexion ...
```

## Norm and Build Testing

```bash
norminette
make
make
make clean
make
make fclean
make re
```

Compilation must use:

```text
-Wall -Wextra -Werror -pthread
```

## End-of-Day Goal

No known leaks, Norm errors, obvious races, deadlocks, or shutdown bugs.

---

# Day 12 — README, Defence, and Recode Practice

Finish documentation and prepare to explain the project without relying on the code.

## README Sections

At minimum, cover:

- Description
- Instructions
- Resources
- How AI was used
- Blocking cases handled
- Thread synchronization mechanisms

In the concurrency sections, explain:

- race-condition prevention
- deadlock prevention
- Coffman's conditions
- starvation handling
- dongle cooldown
- burnout monitoring
- logging serialization
- mutex usage
- condition-variable usage
- communication with the monitor

## Defence Questions

Practice answering these without opening your code:

1. What is a thread?
2. What memory do threads share?
3. Why do dongles require mutexes?
4. What is a race condition?
5. What is a deadlock?
6. What are Coffman's conditions?
7. How does your program prevent deadlock?
8. What is starvation?
9. How does FIFO work?
10. How does EDF work?
11. Why do you need a heap?
12. How does heap insertion work?
13. How does heap removal work?
14. Why use condition variables?
15. How is dongle cooldown implemented?
16. Who detects burnout?
17. What resets the burnout deadline?
18. Why does logging need a mutex?
19. How does the simulation stop?
20. What happens with one coder?

Then open your code and point to the exact implementation for every answer.

## Recode Practice

Practice making small changes quickly:

- add a field to a struct
- change a log behavior
- modify a scheduler comparison
- adjust validation
- change a timing rule

## End-of-Day Goal

You understand your code well enough to explain and modify it during evaluation.

---

# 12-Day Milestone Summary

| Day | Main Target | Finished When |
|---|---|---|
| 1 | Threads | You can create and join N threads yourself |
| 2 | Mutexes | You understand race conditions and deadlocks |
| 3 | Condition variables + timing | You can wait/wake threads and measure milliseconds |
| 4 | Heap | Push/pop works without pthreads |
| 5 | Architecture + parser | Arguments and initialization work |
| 6 | Coder threads + logging | Threads run with serialized output |
| 7 | Dongles + cooldown | Resource ownership and cooldown work |
| 8 | FIFO | Arrival-order arbitration works |
| 9 | EDF | Deadline arbitration and deadlock strategy work |
| 10 | Monitor | Burnout and completion stop correctly |
| 11 | Testing | No known leaks, Norm issues, or major synchronization bugs |
| 12 | Defence | README finished and you can explain/recode the project |

---

# Recommended Daily Routine

For Days 1–4:

```text
~2 hours  Learn concepts
~2 hours  Write tiny experiments from scratch
~1 hour   Debug and experiment
~1 hour   Explain what you learned without notes
```

For Days 5–10:

```text
~1 hour   Review yesterday's concepts
~3–4 hours Implement one focused subsystem
~1 hour   Testing/debugging
~1 hour   Read and explain your own code
```

For Days 11–12:

```text
Testing
Debugging
Documentation
Evaluation preparation
```

---

# Rule for Using AI During the Project

Avoid asking AI to write complete Codexion functions that you copy into the project.

Prefer questions such as:

> Teach me why `pthread_cond_wait()` needs a mutex.

> Give me a small isolated exercise for a binary min-heap.

> Walk through my dongle acquisition logic with two threads and help me identify possible races.

> Do not give me the solution. Ask questions that help me discover why this code deadlocks.

> Give me test cases that could expose starvation.

Your target is not merely:

```text
"It works."
```

Your target is:

```text
"I know why it works,
I know where it can fail,
and I can change it myself."
```

---

# Priority if You Fall Behind

If the schedule slips, prioritize in this order:

1. Correct thread lifecycle
2. Correct mutex/resource protection
3. Correct burnout detection
4. Deadlock-free dongle acquisition
5. FIFO scheduling
6. EDF scheduling
7. Cooldown correctness
8. Clean shutdown
9. Memory cleanup
10. README polish

Do not sacrifice core synchronization correctness to make the project look polished.

The difficult section of this schedule is **Days 7–10**. Protect those days. Avoid spending too much time polishing parsing, folder structure, formatting, or README before the core concurrency system works.
