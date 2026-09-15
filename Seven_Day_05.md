# Day 5 — Monitor, Burnout, and Global Shutdown

[Schedule](Seven_Day_Completion.md) · [Previous](Seven_Day_04.md) · [Next](Seven_Day_06.md)

## Goal and time budget

Replace the placeholder monitor with deadline-driven detection and make every
blocking path stop safely. Budget two hours for monitor state, two for stop/logging
integration, and two for special cases. This is a major integration checkpoint.

Work in `monitor.c`, `coder.c`, `time.c`, `scheduler.c`, `log.c`, and `thread.c`.
Keep the Day 3 lock order visible while reviewing every cross-module call.

## Step 1 — Define the terminal state

The existing `should_stop` says whether execution should end. Also retain enough
state to distinguish normal completion, burnout, and internal failure. A small
enum or explicit fields under `sim_state_mutex` can provide this; choose one clear
representation rather than unrelated flags that can contradict one another.

Only the first terminal transition wins. Runtime errors must not masquerade as
successful completion or print a fake burnout. Decide what main returns for each
outcome and document it.

## Step 2 — Replace per-coder retirement with global completion

The condition is:

```text
every coder.compile_count >= number_of_compiles_required
```

Reaching the quota individually does not mean the whole simulation is complete.
Remove Day 2's temporary finite-loop exit. A coder keeps following its lifecycle
until global stop; its count may exceed the minimum while others catch up.

Increment a count only after a full compile finishes. Protect counts and
`last_compile_start` with `sim_state_mutex` and notify the monitor when they change.
If your accepted input domain includes zero required compiles, complete immediately
without starting unnecessary activity. Verify zero-value rules against the subject
and parser rather than inventing undocumented input restrictions.

## Step 3 — Scan burnout deadlines under state protection

For every coder:

```text
deadline = last_compile_start + time_to_burnout
burnout is due when now >= deadline
```

The clock resets at compile **start**, not completion. A compile longer than the
burnout interval can itself end in burnout. Check arithmetic overflow for large
accepted arguments; an overflowed deadline must not become an immediate false death.

Scan for expired deadlines, all-counts-complete, and the nearest future deadline.
For deterministic handling, document precedence if burnout and completion are
observed together; for example, check already-expired deadlines first. Do not
silently exclude a coder from monitoring because it reached its individual quota.

## Step 4 — Wait until the next deadline or state change

After the common start gate:

```text
lock sim_state_mutex
while not stopped:
    inspect current state and clock
    if a terminal condition is found:
        leave the wait loop to perform the terminal transition
    otherwise:
        timed-wait on wakeup_cond until nearest burnout deadline
unlock sim_state_mutex
```

Use the absolute real-time millisecond-to-timespec conversion from Day 4. The
monitor's condition uses `sim_state_mutex`; the scheduler's condition uses its queue
mutex. Keep these pairings consistent. Re-evaluate after notification or timeout.
Unexpected wait/time errors request failure shutdown.

The subject requires burnout output within 10 ms of the actual deadline. Avoid
coarse periodic polling and long-held critical sections. Check timing without
Valgrind or race instrumentation, which changes scheduling substantially.

## Step 5 — Serialize terminal decisions with output

A logger that checks stop, unlocks, and prints later can print after burnout.
Use one ordering for ordinary logging and terminal logging:

```text
lock log_output_mutex
lock sim_state_mutex
check terminal state
print an ordinary line only if still running
unlock sim_state_mutex
unlock log_output_mutex
```

For a burnout transition, the monitor takes log then state, **rechecks the current
deadlines and terminal conditions**, records stop/reason, and prints exactly one
burnout line before releasing the locks. A previous scan may be stale because a
coder began compiling in between. Never acquire log while retaining state from
the scan; that would reverse the order.

Do not call the ordinary public logger from a context already holding its locks.
Use an internal already-locked printing helper or perform the terminal line there.
Print the current elapsed time, not a fabricated deadline timestamp.

## Step 6 — Make compile-start updates agree with the monitor

After a grant, a coder must not overwrite an already-expired deadline and appear
to recover before the monitor sees it. Under the same protected transition used
for start logging, check stop and the coder's previous deadline before setting
`last_compile_start` to now.

If its deadline has passed, leave the old timestamp intact, notify the monitor,
and follow the shutdown/wait path without starting another compile. The monitor
remains responsible for detecting and announcing burnout. If stop was recorded
after a grant, release that pair rather than performing more activity.

Keep critical sections short. Hold no state, log, queue, or dongle mutex through
the actual activity wait.

## Step 7 — Make sleeps and queues respond to stop

Extend the sleep interface to receive `t_data *` or add a stop-aware activity
helper. Between small sleeps, read stop under `sim_state_mutex`. Preserve the
difference between “duration finished”, “stopped”, and “clock/sleep error”.
An enum result is clearer than treating every early return as success.

On the first terminal transition:

1. Set stop/reason and broadcast `wakeup_cond` under state protection.
2. Release state and output locks.
3. Acquire the scheduler mutex, clear/cancel waiting requests according to the
   storage design, and broadcast `request_queue_cond`.
4. Release the scheduler mutex. Workers release any owned pairs and return.

A queue waiter checks stop while holding queue and briefly acquiring state before
waiting. The stop path's later acquisition of queue synchronizes its notification
with that check. Never hold state while trying to acquire queue.

## Step 8 — Handle one coder explicitly

With one coder, left and right refer to the same dongle. It cannot compile:

- Never lock that same mutex twice or claim two acquisitions.
- If implementing a single-dongle acquisition message, record actual ownership
  under its mutex and release it on stop; do not fabricate a pair grant.
- Keep the worker interruptibly waiting; the monitor reaches its initial deadline,
  prints burnout, and wakes the worker for shutdown.

Expected: no `is compiling` line, one burnout line, and prompt termination.

## Step 9 — Verify final thread ownership

The monitor now decides normal terminal conditions. Main can join workers while
the monitor continues running, then join the monitor once all have stopped. It no
longer decides successful completion by waiting for finite worker loops.

Reuse the partial-startup failure path. Cleanup occurs only when all threads are
known finished; a join failure does not grant permission to destroy live state.

## Checks

```bash
./codexion 1 100 10 10 10 1 0 fifo
./codexion 2 50 100 10 10 2 0 fifo
./codexion 2 5000 10 10 10 2 0 fifo
./codexion 5 100 10 10 10 5 500 fifo
```

Respectively check one-coder burnout, burnout during a long compile, generous-time
completion, and stop while requests face cooldown. For the last case, derive the
actual victim from the trace; do not assume a fixed coder ID.

Measure burnout against the victim's last compile-start deadline (initial start
if it never compiled). Check one terminal line, no subsequent ordinary output,
no partial compile counted, and joins before cleanup.

## Completion checklist

- [ ] Monitor uses nearest-deadline waits and state-change notifications.
- [ ] Counts reflect completed compiles; completion is global.
- [ ] Burnout cannot be erased by a late compile-start update.
- [ ] Terminal decisions and logs are serialized; all waiters can exit.
- [ ] Stop-aware sleep distinguishes interruption from successful duration.
- [ ] One coder and burnout-during-compile cases terminate correctly.
- [ ] Uninstrumented burnout timing meets the subject's tolerance.

Explain the complete shutdown path from a detected deadline to the last free.
If it hangs, trace which predicate each thread is waiting for before proceeding.

References: [project subject](codexion.pdf), global rules/mandatory part;
[condition waits](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html).
