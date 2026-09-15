# Day 3 — Heap and Dongle State

[Schedule](Seven_Day_Completion.md) · [Previous](Seven_Day_02.md) · [Next](Seven_Day_04.md)

## Goal and time budget

Implement a tested priority queue and resource helpers before connecting them to
worker threads. Budget two hours for heap operations, two for comparisons/tests,
and two for dongle ownership/cooldown. Reuse a working earlier heap exercise.
This day is dense; extend it if heap removal is not yet understood.

Work in `srcs/heap.c`, `srcs/dongle.c`, and the header. Add only needed fields and
declarations; do not rename all the existing structures.

## Step 1 — Define one request's lifetime

Your `t_request` contains a coder pointer, burnout deadline, arrival order, and
grant flag. Each compile attempt creates one request for that coder's pair.

Choose stable storage: one persistent request per coder is a simple option.
Allocate it with coder data or another explicitly owned array. The heap stores
pointers, so moving a pointer does not move or free the request itself.

Write these rules before coding:

- A coder has at most one outstanding request.
- A queued request remains alive until it is removed or the queue is cleared.
- A new attempt gets a new arrival number; a spurious wakeup does not.
- A queued key remains unchanged. Changing it requires heap repair.
- Grant/enqueue state is protected by the scheduler mutex once threads use it.

Persistent storage also simplifies shutdown: clear queued references under the
scheduler mutex and free storage only after joins. Do not return from a function
while its local request address is still in the heap.

## Step 2 — Define one comparison function

The comparison answers: “Should request A precede request B?”

| Policy | Primary key | Tie-breaker |
|---|---|---|
| FIFO | Smaller arrival order | Coder ID as a final deterministic fallback |
| EDF | Smaller burnout deadline | Smaller arrival order, then coder ID |

Compare with `<` and `>` instead of subtracting potentially large keys.
Use this same comparison in every heap operation and scheduler priority check.
The EDF deadline is `last_compile_start + time_to_burnout`, not enqueue time plus
burnout time. Guard arithmetic at supported input boundaries.

## Step 3 — Implement the array heap without threads

For zero-based index `i`:

```text
left child  = 2*i + 1
right child = 2*i + 2
parent      = (i - 1)/2, only when i > 0
```

Implement in small passes:

1. `peek`: return the root, or a documented empty result.
2. `push`: reject full capacity, append the pointer, bubble it upward while it
   precedes its parent, and update size correctly.
3. `pop`: save the root, move the last element to the root, reduce size, then
   repeatedly swap with the higher-priority child until order is restored.
4. Handle empty and one-element heaps explicitly.

The invariant is that no child precedes its parent. The array is **not globally
sorted**. Iterating array indices does not produce FIFO or EDF order.

Capacity N is enough only if the one-outstanding-request-per-coder rule holds.

## Step 4 — Check ordering with known examples

Use a separate driver; no pthreads are needed:

| Case | Expected result |
|---|---|
| Pop empty heap | Defined empty result, no invalid memory access |
| Push one, pop one | Same pointer; size becomes zero |
| Arrival keys inserted 3, 1, 4, 2 | Pop keys 1, 2, 3, 4 |
| EDF pairs `(deadline, arrival)` = (900,1), (700,3), (700,2) | Pop (700,2), (700,3), (900,1) |
| Push beyond capacity | Report failure without corrupting existing elements |
| Alternate pushes/pops | Invariant holds after each operation |

Explain every swap in one insertion and one removal. If a result fails, debug
here before adding shared-state locks.

## Step 5 — Define dongle availability

For your fields, a dongle is eligible when:

```text
current_owner == NULL AND now >= cooldown_deadline
```

Use the ring links already initialized in `init_dongles()`. Ownership is logical
state protected by a mutex; it does not require holding the mutex throughout a
compile. Every ownership reader and writer must obey the same protocol.

When the pair is released at time R:

```text
both owners become NULL
both cooldown deadlines become R + dongle_cooldown
```

Use one checked release timestamp. An ownerless dongle may still be cooling down.
Only its current owner may release it. Never grant the same dongle twice.

## Step 6 — Establish a lock order for integration

**Objective: prevent deadlocks when threads need multiple mutexes.**

**Mutex flow**

```text
scheduler.request_queue_mutex
    → dongle mutexes by increasing array index
        → monitor.log_output_mutex
            → monitor.sim_state_mutex
```

- Acquire only the mutexes needed, following this order; unlock in reverse.
- Order dongles by array index, regardless of their left/right position.
- If left and right point to the same dongle, reject the pair before locking.
- Release state/log mutexes before acquiring the scheduler mutex.

## Step 7 — Implement pair checking and release helpers

Under the scheduler lock, acquire both distinct dongle mutexes in index order.
Check both owners and cooldowns while protected. Commit both ownership changes
together or change neither; release the mutexes before waiting or sleeping.

These helpers enforce availability. **Day 4's scheduler decides whose request is
allowed to call the grant operation.** Calling it directly from every worker
would bypass priority rules.

Check without concurrent workers first:

- Both free and ready: grant both to A.
- One occupied: grant neither to B.
- Released at 500 with cooldown 100: reject at 599, accept at 600.
- Left equals right: cannot supply two distinct dongles.

Pass controlled timestamps to internal eligibility helpers for deterministic
boundary checks; keep the public timing behavior based on your clock helper.

## Completion checklist

- [ ] Heap operations preserve order and capacity boundaries.
- [ ] FIFO and EDF comparator examples pass, including ties.
- [ ] Request storage ownership and lifetime are explicit.
- [ ] Ownerless-but-cooling dongles are unavailable.
- [ ] Pair updates are all-or-neither and each dongle state is mutex-protected.
- [ ] Lock order and the equal-pointer case are documented.

Explain: Why isn't a heap array sorted? Why does acquiring a pair avoid a coder
holding one dongle while waiting for another? Why is cooldown different from
ownership?

Subject reference: [codexion.pdf](codexion.pdf), mandatory heap, per-dongle mutex,
cooldown, and deterministic EDF arbitration requirements.
