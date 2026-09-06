# Day 6 — EDF Integration and Contention Checks

[Schedule](Seven_Day_Completion.md) · [Previous](Seven_Day_05.md) · [Next](Seven_Day_07.md)

## Goal and time budget

Use the tested EDF comparator in the live scheduler and investigate liveness under
contention. Budget two hours for integration, two for policy checks, and two for
regressions/repairs. Begin with working FIFO ownership and global shutdown.

Work mainly in `scheduler.c` and `heap.c`; change coder/monitor code only when a
specific test reveals an integration issue. Keep a known-good FIFO test case.

## Step 1 — Follow one deadline from creation to grant

For a request:

```text
burnout_deadline = coder.last_compile_start + config.time_to_burnout
```

Read the timestamp under state protection when constructing the request. It is
normally stable while the coder waits because that coder cannot start another
compile until the request is granted. The next attempt receives a new deadline
and new arrival order.

Do not mutate a deadline already in the heap without repairing its position. Do
not use the elapsed log timestamp as an absolute deadline. Keep monitor and
scheduler arithmetic in the same clock domain.

## Step 2 — Select policy once and use one comparator everywhere

Your parsed scheduler string is exactly `fifo` or `edf`. Connect that selection to
the comparator used by push, pop, and any priority-sensitive eligibility checks.
If you introduce an enum during initialization, document its mapping and keep the
original command-line interface unchanged.

| Requests | FIFO winner | EDF winner |
|---|---|---|
| A: arrival 1, deadline 900; B: arrival 2, deadline 700 | A | B |
| A: arrival 1, deadline 700; B: arrival 2, deadline 700 | A | A |

The subject requires deterministic EDF ties. Use arrival order, then coder ID as
a final fallback. Compare relationally instead of subtracting large integer keys.

## Step 3 — Apply priority to shared dongles

Retain Day 4's all-or-neither pair grant and conflict checks. Under EDF, an earlier
deadline request waiting for either of a candidate's dongles takes precedence over
the candidate. A request with independent ready dongles need not wait behind an
unrelated blocked heap root.

Do not replace dongle eligibility with priority: the highest-priority request
still cannot use an occupied or cooling dongle. EDF orders access; it does not
shorten compilation, cancel cooldown, or give a coder a third resource.

## Step 4 — Test policy with controlled requests first

Thread launch order is nondeterministic. Use a temporary scheduler driver with
known request keys and resource state before interpreting full simulation logs.

1. Queue A and B for the same pair with the first row's keys above.
2. Make the pair ready and run arbitration under FIFO; check A wins.
3. Reset the fixture fully and run EDF; check B wins.
4. Use equal deadlines; confirm the chosen tie rule.
5. Make the first request's other dongle unavailable; check a lower-priority
   request sharing either dongle cannot bypass it.
6. Add a ready independent pair; check it can be granted.

For each check, record expected owners, heap size, grant flags, and waiting keys.
Temporary debug traces must not become extra lines in the final action output.

## Step 5 — Distinguish deadlock, starvation, and infeasible timing

| Observation | What to inspect |
|---|---|
| No thread progresses or can exit | Lock cycle, missed notification, stale predicate |
| Others compile while one contender keeps being denied | Request ordering, ties, unnecessary conflicts, wakeups |
| Required activity already exceeds the burnout budget | Timing may be infeasible even with correct arbitration |
| Free independent resources remain idle | Global-root blocking or overbroad eligibility rule |

Your pair scheme removes the resource hold-and-wait cycle: a coder receives both
dongles together or neither. Internal mutex deadlock is still possible if code
violates the lock order. Explain these separately.

The [subject](codexion.pdf), mandatory part, requires EDF liveness for feasible
parameters. A priority queue alone does not prove that requirement. Inspect
resource utilization and whether older/urgent conflicting requests actually get
service. Do not label every failure “infeasible” without deriving a schedule or
an unavoidable timing bound.

An isolated coder's repeat start interval includes compile, debug, and refactor
time, plus resource/cooldown waiting. Satisfying that isolated lower bound does
not by itself prove feasibility for the whole ring.

## Step 6 — Run integration cases under both policies

```bash
./codexion 2 5000 10 10 10 3 0 fifo
./codexion 2 5000 10 10 10 3 0 edf
./codexion 5 5000 10 10 10 5 20 fifo
./codexion 5 5000 10 10 10 5 20 edf
./codexion 1 100 10 10 10 1 0 edf
./codexion 2 50 100 10 10 2 0 edf
```

The first four give generous timing for completion checks. The last two force
burnout. Repeat with even and odd coder counts. Then tighten one timing parameter
at a time and explain each change in outcome.

Check that both modes still preserve cooldown, unique ownership, log serialization,
deadline detection, and clean shutdown. Different valid thread interleavings need
not produce identical entire logs.

## Step 7 — Review shutdown at every queue state

Trigger or reproduce stop while a coder is:

- About to enqueue.
- Queued without a grant.
- Granted resources but not yet compiling.
- Compiling.
- Debugging or refactoring.

For every state, identify who removes the request, who releases owned dongles,
what wakes the thread, and which function joins it. Check no completed count is
invented for an interrupted compile. Perform the same review for clock/runtime
failure, using controlled test hooks if necessary.

## Step 8 — Record the actual design for evaluation

Add short README notes explaining:

1. FIFO/EDF keys and the tie rule.
2. How conflicting requests are identified and independent pairs make progress.
3. Why pair grants prevent partial-resource deadlock.
4. How cooldown and monitor deadlines wake waiting threads.
5. What your liveness tests establish and which timing cases cannot succeed.

## Completion checklist

- [ ] Controlled examples show FIFO and EDF choose different expected winners.
- [ ] EDF ties are deterministic; queued keys remain valid.
- [ ] Shared-dongle priority is honored while independent pairs can progress.
- [ ] Both policies pass completion, cooldown, and burnout regressions.
- [ ] Every request state has a verified shutdown path.
- [ ] No known starvation or unnecessary-idling issue is left unexplained.

Explain: Why does EDF use last compile start? Can EDF rescue impossible timing?
Why doesn't “the heap is correct” prove the entire scheduler is correct?

If EDF implementation remains incomplete, extend the schedule before the final
verification day. Do not count documentation as a substitute for missing behavior.
