# Day 4 — FIFO Scheduling and Real Compilation

[Schedule](Seven_Day_Completion.md) · [Previous](Seven_Day_03.md) · [Next](Seven_Day_05.md)

## Goal and time budget

Replace fake dongle messages with real pair acquisition. Budget two hours for
requests/waits, two for grant/release/cooldown, and two for contention checks.
Start only with a tested heap and a written lock order.

Work primarily in `scheduler.c`, `dongle.c`, and `coder.c`. A scheduler layer is
shared arbitration code; it does not require another thread. Coders call it with
their own requests. Keep the separate monitor thread from Day 2.

## Step 1 — Draw a request's state transitions

```text
idle → enqueued → waiting → granted → compiling → released → idle
                    ↓
                 cancelled on stop
```

Document whether each helper expects the scheduler lock already held. Distinguish
“not ready yet” from “stopped” and “internal failure”; a failed availability check
is not an error and should normally lead to waiting.

On enqueue, while holding `request_queue_mutex`:

1. Check stop with `sim_state_mutex`, then release state while retaining queue lock.
2. Read this coder's last compile start under state protection.
3. Set the request's deadline and fresh arrival number.
4. Reset its grant flag and insert exactly once into the heap.
5. Run arbitration, then notify waiters if eligibility changed.

Never overwrite a queued request or enqueue it again after a wakeup.

## Step 2 — Respect priority where dongles are shared

The subject specifies arbitration when requests compete for the **same dongle**.
A single global heap root is a useful first exercise, but refusing all other
grants while it is blocked can leave unrelated pairs idle. The original beginner
plan's simple “become the highest-priority request” sketch needs this refinement
before considering EDF liveness complete.

For a request to receive a pair:

- Both dongles must be unowned and off cooldown.
- No earlier waiting FIFO request may need either of those dongles.
- Both ownership changes must be committed together under their mutexes.

One implementable approach using the required heap:

```text
hold scheduler mutex
pop requests in priority order into a preallocated temporary pointer array
for each request in that order:
    if it shares a dongle with an earlier request still waiting:
        keep it waiting
    else if its pair is ready:
        grant both; set its grant flag; remove it from the waiting set
    else:
        keep it waiting
restore waiting requests to the heap with their original keys
notify granted waiters
```

Keep earlier blocked requests available for overlap checks. Arrays sized to N
avoid allocation while arbitrating. Each comparison must use Day 3's comparator.
Do not assume heap array order is priority order. A simpler ordered scan with
explicit heap removal is also possible, but test heap repair after removal.

This is a proposed design, not a proof that all feasible EDF cases will succeed.
Day 6 must examine liveness and unnecessary idle resources under actual contention.

## Step 3 — Make waiting release the scheduler mutex

The waiting coder checks its grant flag and stop condition under the established
locking protocol, then waits on `request_queue_cond` with `request_queue_mutex`.
It holds no dongle, log, or state mutex across that wait.

After every wakeup, arbitrate/recheck: being awakened does not imply a grant.
The condition wait temporarily releases the queue mutex, so other coders can
enqueue and release resources. Consult the
[wait reference](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html).

Use broadcast when several distinct predicates may have become true. Waking only
one arbitrary waiter can leave the eligible coder asleep while an ineligible one
goes back to waiting.

## Step 4 — Arrange a wakeup when cooldown expires

Release broadcasts alone are insufficient:

```text
500 ms: owner releases; waiter wakes
500 ms: dongle still cooling until 600; waiter sleeps again
600 ms: no owner exists to send another release notification
```

Use `pthread_cond_timedwait()` for the next relevant **future** cooldown deadline
when time alone can make a waiting pair eligible. If the obstacle is ownership,
release will notify waiters; do not repeatedly wait on an already-past deadline.

Default condition variables use real time. Keep absolute timestamps from your
`gettimeofday()` helper in that clock domain. Convert an absolute millisecond
deadline D to `struct timespec`:

```text
tv_sec  = D / 1000
tv_nsec = (D % 1000) * 1000000
```

Do not pass a relative delay such as 50 as an absolute deadline. Recompute
eligibility after timeout. `ETIMEDOUT` is an expected scheduling event; other
unexpected wait errors enter the failure path. Include the needed time/error
headers. The [timed-wait reference](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)
describes absolute deadlines and return codes.

## Step 5 — Integrate a real compile cycle

In `coder_routine()`, replace the fake acquisition section:

```text
request pair; wait for grant or stop
if granted and still running:
    record actual compile start under sim_state_mutex
    notify monitor
    log two dongle acquisitions and compiling in the required order
    sleep for compile duration
    count only a fully completed compile
release owned pair even if the activity failed or stop was requested
debug, then refactor, checking outcomes
```

Log acquisitions only after ownership is granted. Release is followed by
cooldown bookkeeping, arbitration, and notification under the lock protocol.
Do not hold dongle mutexes during the compile sleep; ownership fields reserve
the resources. Day 5 makes the compile-start transition and terminal logging agree.

## Step 6 — Prepare cancellation without leaving dangling requests

On stop, waiting coders must leave the queue safely. Choose either removal of each
request with heap repair or clearing the entire queue under its mutex during
global shutdown. With persistent request storage, clear references before freeing
storage after joins. A granted coder releases its pair on the way out.

After recording stop, release state/log locks before taking the queue lock and
broadcasting. All queue waiters check stop while holding the queue lock and briefly
taking state. This coordination prevents a stop notification being lost between
their predicate check and their wait. Day 5 completes this path for normal burnout.

## Step 7 — Test FIFO with controlled order

Thread creation order does not establish enqueue order. Use a temporary controlled
driver or debug trace outside normal action output to capture actual arrival keys.

| Case | Expected result |
|---|---|
| Two coders, same pair | Earlier queued contender gets the next eligible grant |
| Earlier blocked A, later B sharing either dongle | B cannot jump ahead of A |
| Earlier blocked A, later B using independent ready dongles | B can progress |
| Positive cooldown | Grant occurs no earlier than the release deadline |
| Stop with a queued request | Waiter exits and no stale request remains |

For short integration runs, keep the finite scaffold and generous burnout values:

```bash
./codexion 2 5000 10 10 10 2 0 fifo
./codexion 5 5000 10 10 10 3 20 fifo
```

Use an external timeout when investigating hangs. Final one-coder behavior needs
Day 5's real monitor; an ungrantable single-coder request can otherwise wait forever.

## Completion checklist

- [ ] Fake acquisitions are removed; each compile owns two distinct dongles.
- [ ] Queue ordering matches actual FIFO arrival order for shared resources.
- [ ] Independent pairs can progress without bypassing shared-dongle priority.
- [ ] Release and cooldown expiry both cause eligibility to be reconsidered.
- [ ] No queue/dongle mutex stays locked across activity sleeps.
- [ ] Stop/failure releases ownership and preserves request lifetime.

Explain: Who wakes a coder after cooldown? Why isn't enqueue order the same as
thread ID order? What prevents a broadcast from being lost before a queue wait?

If contention runs hang, resolve the smallest failing case before adding burnout.
