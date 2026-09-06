# Day 7 — Verification, README, and Defence

[Schedule](Seven_Day_Completion.md) · [Previous](Seven_Day_06.md)

## Goal and time budget

Verify the finished mandatory implementation and prepare to explain it. Budget
two hours for failures/memory/races, two for build/Norm/regressions, and two for
README/defence. If a required subsystem is unfinished, complete it and keep a
separate final verification day. Seven days is a target, not permission to skip
remaining checks.

## Step 1 — Record a reproducible baseline

Save a concise table of command, expected behavior, observed outcome, and any
failure. Keep representative passing cases and the smallest known failing case.
Change one thing at a time when debugging. Re-run affected checks after a repair.

Check implementation against [codexion.pdf](codexion.pdf), especially global rules,
mandatory part, and README requirements. The subject is authoritative when a
learning-plan sketch is incomplete.

## Step 2 — Check parsing and supported boundaries

Reject missing/extra arguments, malformed numbers, negatives, overflow, and
scheduler strings other than exactly `fifo` or `edf`.

```bash
./codexion
./codexion -1 800 200 200 200 3 10 fifo
./codexion abc 800 200 200 200 3 10 fifo
./codexion 5 800 200 200 200 3 10 banana
./codexion 5 800 200 200 200 3 10 fifo extra
```

Also try an integer too large for its destination type. Review addition and unit
conversion boundaries in deadlines, cooldowns, and sleeps; accepting a numeric
value must not lead to signed overflow later.

Review zero cases individually: zero coders cannot form the required simulation;
zero cooldown should be usable. If zero required compiles is accepted, it should
complete immediately. Do not apply one blanket rule to every numeric argument.

## Step 3 — Exercise the behavioral matrix

Run each applicable case with both scheduler policies:

| Case | What must be checked |
|---|---|
| One coder | Cannot compile with one dongle; burns out and exits |
| Two coders | Shared pair ownership stays exclusive |
| Odd/even groups | Correct ring links and pair conflicts |
| Generous timing and finite quota | All reach the required completed count |
| Compile longer than burnout | Burnout can occur during compilation |
| Long cooldown | Resources remain unavailable until expiry |
| Zero cooldown | No artificial extra cooldown delay |
| Simultaneous requests and EDF ties | Controlled priority examples stay deterministic |
| Stop while waiting/compiling | Waiters wake, owners release, threads join |
| Repeated execution | No intermittent hangs or stale state |

Useful representative commands:

```bash
./codexion 1 100 10 10 10 1 0 fifo
./codexion 2 5000 10 10 10 3 0 fifo
./codexion 5 5000 10 10 10 5 20 edf
./codexion 2 50 100 10 10 2 0 edf
```

Use a generous external timeout to catch hangs in test tooling. A forced timeout
is a failure to investigate, not successful program shutdown.

## Step 4 — Check logs and timing against actual events

For every ordinary line, check elapsed timestamp, valid coder ID, and an exact
permitted status. Lines must be whole; diagnostics do not belong in action output.
Each compile needs two actual dongle acquisitions, and phases must not overlap for
one coder. No ordinary message follows terminal output.

For burnout timing:

```text
expected deadline = victim's actual last_compile_start + time_to_burnout
lateness = actual burnout log time - expected deadline
```

Use the initial simulation start if the victim never compiled. A log timestamp
near compile start is useful evidence but may not equal the stored timestamp;
use a controlled diagnostic trace when resolving a boundary discrepancy. The
subject requires burnout output within 10 ms; evaluate with the normal build and
reasonable machine load, not under instrumentation.

## Step 5 — Investigate memory and race reports

If the tools are installed:

```bash
valgrind --leak-check=full --show-leak-kinds=all ./codexion 5 100000 10 10 10 2 0 fifo
valgrind --tool=helgrind ./codexion 5 100000 10 10 10 2 0 edf
```

Generous deadlines help isolate synchronization checks from instrumentation delay.
Also inspect a burnout path and a controlled partial-startup failure path.
Investigate stack traces for invalid accesses, leaks, inconsistent lock order,
and unprotected shared state. These checks do not prove the absence of every race.

Audit lifecycle ownership manually:

- Every successful allocation has an owner and matching free.
- Every successfully initialized synchronization object is destroyed once.
- Partial initialization cleanup only touches initialized objects.
- Every created thread is joined; no possibly live shared state is freed.
- Queued pointers never outlive their request storage.
- Runtime failures propagate to a coordinated shutdown.

## Step 6 — Check build, Norm, and allowed functions

Run separately and inspect each result:

```bash
make
make
make clean
make
make fclean
make re
norminette
```

Ensure the second unchanged `make` does not relink, header changes rebuild affected
objects, and cleaning removes the intended generated outputs. Keep required
compiler flags. Check functions against the subject's allowed list: Libft is not
authorized and global variables are forbidden. Remove test hooks and temporary
action-output diagnostics from the final executable.

If a required check cannot run, record it as outstanding rather than marking it
passed. Complete it in the intended evaluation environment before submission.

## Step 7 — Finish the English README

The first line must follow the subject's italicized attribution format, with your
actual login. Write documentation for the current implementation, not the earlier
fake lifecycle.

| Required section | Include |
|---|---|
| Description | What the simulation models and when it stops |
| Instructions | Build command, all eight arguments, units, FIFO/EDF examples |
| Resources | References used and specific tasks/parts where AI assisted |
| Blocking cases handled | Deadlock/Coffman conditions, starvation, cooldown, burnout precision, serialized output |
| Thread synchronization mechanisms | Mutex ownership, condition predicates, lock order, coder-monitor communication |

Use one concrete example to explain a waiting pair and one to explain shutdown.
Do not claim a proof or test result you have not established. Ask a peer to review
the code and explanations, as the subject encourages; include their feedback in
your own review process.

## Step 8 — Practise defence and a small modification

Without opening the code, explain:

1. What memory threads share and what stays on an individual thread's stack.
2. Why the start gate exists and why its predicate uses a loop.
3. Which mutex protects each shared value and the nested lock order.
4. Why a coder cannot hold one dongle while waiting for the other.
5. How heap insertion/removal and EDF ties work.
6. Who wakes a worker after release, cooldown expiry, and stop.
7. Why the burnout deadline resets at compile start.
8. How one terminal line is guaranteed and later action logs are suppressed.
9. Why one coder cannot compile and how it still exits.
10. How failure during thread creation reaches safe cleanup.

Then point to the corresponding functions. In a temporary exercise, modify one
validation rule or tie-breaker and explain all affected checks. Restore intended
subject behavior afterward. Avoid unrelated refactoring during final validation.

## Completion checklist

- [ ] Mandatory behavior works under FIFO and EDF, including edge cases.
- [ ] Normal runs meet the burnout logging tolerance.
- [ ] No known memory, race, deadlock, ownership, or shutdown defect remains.
- [ ] Required build and Norm checks pass; no forbidden functions/globals remain.
- [ ] README meets required structure and explains the actual implementation.
- [ ] Temporary hooks are removed and final changes receive relevant checks.
- [ ] You can explain and make a small change to the code yourself.

If a box remains open, write the exact next task and continue in another session.
Completion is the checked result, not the day number.
