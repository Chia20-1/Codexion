# Day 2 — Multiple Threads and a Safe Lifecycle

[Schedule](Seven_Day_Completion.md) · [Previous](Seven_Day_01.md) · [Next](Seven_Day_03.md)

## Goal and time budget

Create N coder threads plus a placeholder monitor, release a common start gate,
run finite temporary activities, and join all created threads. Budget two hours
for creation/gating, two for routines, and two for failure checks and integration.
Continue only once Day 1's build and single-worker check work.

## Step 1 — Write down shared-state ownership

Use your existing struct fields:

| State | Protection |
|---|---|
| `should_stop`, `simulation_started` | `monitor.state_mutex` |
| `last_compile_start`, `compile_count` | `monitor.state_mutex` |
| `start_time` | Written under state mutex before gate release; immutable afterward |
| Output | `monitor.log_output_mutex` |
| Creation flags | Main thread manages them |

Every read and write of mutable shared state needs the same protection. Introduce
small helpers for repeated operations such as reading the stop flag; do not hide
whether a helper locks a mutex in its interface documentation.

When both output and state locks are required, use **log mutex before state
mutex** throughout. Do not call a logger while already holding the state mutex.
Day 3 extends this ordering to scheduler and dongle locks.

## Step 2 — Expand creation from one worker to N

In `thread.c`, create each worker with a pointer to its own array element. Keep
`thread_created` flags false initially and set them only on successful creation.
Add the separate monitor handle and flag. All data must already be initialized.

Build the creation/join loops with immediate-return workers first. Check distinct
IDs in a temporary diagnostic; do not require execution in ascending ID order.

## Step 3 — Build the common start gate

Both routines begin with this predicate loop:

```text
lock state_mutex
while simulation_started is false AND should_stop is false:
    wait on wakeup_cond using state_mutex
remember should_stop
unlock state_mutex
if stopping: return
```

Waiting releases the mutex and reacquires it before returning. Wakeups require
rechecking the predicate, hence `while`. A broadcast is a notification, not stored
permission to proceed. [Condition-wait semantics](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html)

After all creations succeed, main does this under `state_mutex`:

```text
obtain start_time; on failure use the startup-cancellation path
set every coder.last_compile_start to start_time
set simulation_started = true
broadcast wakeup_cond
```

Then unlock. Later-created threads no longer lose part of their initial time
budget. Remove Day 1's temporary pre-creation start timestamp.

## Step 4 — Implement the placeholder monitor

In `monitor.c`, pass `t_data *` as the argument. After the gate, wait on
`wakeup_cond` in a loop while `should_stop` is false. Check wait return values.
It does not detect burnout yet. It proves that a separate waiting thread can be
started, notified, stopped, and joined without polling.

## Step 5 — Add a finite coder scaffold

In `coder.c`, use this temporary sequence:

```text
wait at start gate
repeat until this coder has completed the temporary target or stop is requested:
    log two fake dongle-acquisition messages
    update last_compile_start under state_mutex; notify monitor
    log compiling; wait time_to_compile
    if full compile completed: increment compile_count under state_mutex; notify
    log debugging; wait time_to_debug
    log refactoring; wait time_to_refactor
return
```

Document fake dongle messages in the source. They disappear in Day 4. Check stop
between phases. Never hold the state or log mutex through an activity sleep.
For now, use short durations because stop-aware sleep arrives in Day 5.

If a time/sleep operation fails, request stop, notify the monitor, and return
through normal cleanup. Do not count an interrupted compilation as completed.
Track runtime failure separately from ordinary stopping, using a state-protected
error field if needed, so `run_simulation()` can report it to main.

The per-coder finite target is temporary. Day 5 changes termination to the global
rule: all coders have reached the target, or a coder burns out.

## Step 6 — Own normal shutdown in run_simulation

For this finite scaffold:

```text
create monitor and workers
release start gate
join each worker
request stop and broadcast wakeup_cond
join monitor
return outcome to main
```

Do not request normal stop before the workers finish their finite loops. The
monitor would otherwise exit without exercising the planned lifecycle.

## Step 7 — Handle partial startup failure

Draw the case where monitor and coders 1–2 exist but creating coder 3 fails:

1. Set `should_stop` under `state_mutex`.
2. Broadcast `wakeup_cond` so threads waiting at the unopened gate can exit.
3. Join only workers marked as created, then the created monitor.
4. Report failure; clean shared data once no thread can access it.

Check pthread return codes directly. Do not use `errno` as their result. If a join
unexpectedly fails, record it and attempt the other joins; the failed join does
not establish that its target is finished. Do not blindly free possibly live
thread data. Diagnose invalid handles or lifecycle misuse before proceeding.

Use a temporary test hook that simulates creation failure before calling
`pthread_create()` for a selected index. Do not actually create a thread and then
pretend its handle was never created. Remove test hooks from the final build.

## Step 8 — Run the infrastructure checks

```bash
./codexion 1 1000 10 10 10 1 0 fifo
./codexion 2 1000 10 15 20 2 0 fifo
./codexion 5 1000 5 5 5 3 0 edf
./codexion 20 1000 1 1 1 2 0 fifo
```

Both scheduler strings still exercise the same scaffold. These runs do not prove
FIFO, EDF, cooldown, or correct final one-coder behavior.

Repeat short runs and check complete lines, valid IDs, nonnegative elapsed times,
and prompt exit. Test cancellation before any worker and after several workers.
If installed, investigate memory/race reports with a small run; do not judge
timing under instrumentation.

## Completion checklist

- [ ] N coder threads and one monitor are created with stable arguments.
- [ ] All threads use the common gate; the clock starts at release.
- [ ] Shared state has consistent protection and logs do not interleave.
- [ ] Workers execute finite activities and notify state changes.
- [ ] Partial creation failure wakes and joins created threads.
- [ ] Repeated runs finish before cleanup; runtime failure reaches main.

Explain: Why can a join hang if the start gate is never broadcast? Why is `while`
required around a wait? Which mutex does a wait release?

This is the first schedule checkpoint. If incomplete, finish it before Day 3.
