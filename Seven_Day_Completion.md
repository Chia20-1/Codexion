# Codexion — Seven-Day Completion Plan

> Revised 6 September 2026. Target: finish in seven more working days from the
> current timing-helper stage, while understanding the code well enough to explain
> and modify it during evaluation. Seven days is a stretch target, not a deadline.

## How to Use This Plan

The **seven-day schedule below replaces the original calendar**. Follow the daily
guides linked below for implementation steps, explanations, checks, and completion
criteria. The [beginner plan](codexion_12_day_beginner_plan.md) remains an additional
reference; its original lesson numbers are separate from these seven working days.

Do not restart the earlier lessons. Continue from the code you have, revisiting
concepts when the next task needs them.

## Daily Step-by-Step Guides

| Day | Guide |
|---|---|
| 1 | [Build, Time, Logging, and One Thread](Seven_Day_01.md) |
| 2 | [Multiple Threads and a Safe Lifecycle](Seven_Day_02.md) |
| 3 | [Heap and Dongle State](Seven_Day_03.md) |
| 4 | [FIFO Scheduling and Real Compilation](Seven_Day_04.md) |
| 5 | [Monitor, Burnout, and Global Shutdown](Seven_Day_05.md) |
| 6 | [EDF Integration and Contention Checks](Seven_Day_06.md) |
| 7 | [Verification, README, and Defence](Seven_Day_07.md) |

Each guide includes a time budget, ordered steps, pseudocode or examples where
useful, checks with expected outcomes, and a completion checklist. Read the next
step only after you can explain and check the current one. The guides refine the
older scheduler sketch to account for priority on shared dongles and progress on
independent pairs, based on the [project subject](codexion.pdf).

## Current Starting Point

Observed in the repository on 6 September; implemented does not mean fully tested:

- Parsing, initialization, cleanup, and the main program skeleton are present.
- `get_time_ms()`, `get_elapsed_ms()`, and `sleep_ms()` are written. Their checks
  and caller error handling still need verification.
- Thread handles, creation flags, and synchronization fields are declared.
- `log.c`, `thread.c`, `coder.c`, and `monitor.c` are empty.
- `heap.c`, `dongle.c`, and `scheduler.c` are empty. If you have a separate heap
  exercise, reuse your understanding and verified code rather than starting over.
- The Makefile still needs source/object lists and objects in the link command.
- The README has only a skeleton.

## Seven More Days: Target Schedule

Your available hours are not yet specified. This schedule assumes **about six
focused hours per day, including learning and debugging**, with breaks outside
those hours. That is approximately 42 hours of work, not seven short evenings.
It is an estimate to revise after the first two days, not a promise of completion.

If you can spend three focused hours per day, budget about **14 working days for
the same work**, before extra debugging. Since you have no fixed submission
deadline, move unfinished work forward rather than cutting correctness checks.

| Remaining day | Main work | Lesson references | Checkpoint before moving on |
|---|---|---|---|
| 1 | Make the existing project build; verify time helpers; serialize logging; create and join one coder thread | Original Day 5; Day 6 Steps 3–4 and Pass A | A normal build works; a roughly 50 ms wait is measured; one worker logs and is joined before cleanup |
| 2 | Extend to N coders; start gate; placeholder monitor; finite fake lifecycle; startup failure and joins | Original Day 6 Steps 5–11 | Repeated short runs start together and exit; failed startup wakes and joins created threads; no mixed log lines |
| 3 | Implement or bring in the heap; test FIFO and EDF comparisons without threads; model dongle pair ownership and release/cooldown | Original Days 4 and 7 | Heap order and ties are predictable; pair availability and cooldown can be checked independently; write down lock ownership/order |
| 4 | Integrate FIFO pair requests into the coder lifecycle; wait/wake logic; cooldown expiry wakeups | Original Day 8 | Real dongles replace fake messages; a coder gets both or neither; two/many-coder runs progress without double ownership or stuck cooldown waits |
| 5 | Implement burnout and completion detection; stop-aware sleeps and scheduler waits; terminal logging; one-coder shutdown | Original Day 10 | Burnout and completion both wake all waiters and join threads; one coder terminates; ordinary logs stop after terminal output |
| 6 | Integrate EDF using the tested comparator; check ties and contention; repeat edge cases under both policies | Original Day 9 and part of Day 11 | FIFO and EDF select requests as intended; long cooldown, short burnout, and stop-during-wait cases terminate correctly |
| 7 | Fix remaining failures; memory/race checks; build and Norm checks; finish README and practise explaining the implementation | Original Days 11–12 | Required checks pass, no known synchronization/shutdown bugs remain, and you can explain the actual code |

Days 3–5 carry the most uncertainty. The heap, scheduler, and monitor each combine
new concepts with implementation. Treat a missed checkpoint as evidence that the
estimate needs more time, not as a reason to skip the checkpoint.

### Work Blocks Within Each Day

Use three blocks of about two focused hours, with breaks between them:

| Day | Block 1 | Block 2 | Block 3 |
|---|---|---|---|
| 1 | Build existing sources and resolve compiler errors | Verify time helpers and write the logger | One worker create/log/join check; note ownership |
| 2 | N workers and common start gate | Placeholder monitor and finite activities | Failure-path review/checks, repeated runs, integration repairs |
| 3 | Heap operations and small deterministic examples | FIFO/EDF comparator and tie checks | Dongle state, pair-grant/release helpers, cooldown checks |
| 4 | Queue a pair request and wait for eligibility | Grant/release real pairs; wake on release and cooldown expiry | Contention checks and repairs; confirm request lifetime |
| 5 | Monitor deadlines and completion state | Propagate stop through sleeps, queues, and logging | One coder, burnout during compile/wait, completion, joins |
| 6 | Connect EDF to the live queue | Compare scheduling behavior under contention | Regression checks and repairs for both policies |
| 7 | Remaining failures plus memory/race investigation | Build/Norm checks and final regression runs | README, defence, and a small recode exercise |

These are time budgets, not instructions to stop mid-bug and begin another
subsystem. Use each block for learning, a small implementation, and its check.
Keep a short README/notes entry at the end of every day so documentation does not
start from zero on Day 7.

### Carryover Rules

- At the end of Day 2, assess progress against the thread-lifecycle checkpoint.
  If it is incomplete, use the next session to finish it and shift later work.
- Do the same after FIFO integration and monitor integration. Do not stack new
  scheduling features on unresolved ownership or shutdown bugs.
- Day 7 is for verification and explanation. If mandatory implementation remains,
  extend the schedule and keep a separate final verification day.
- If extra time is needed, add focused sessions for the actual blocker: heap,
  scheduler/cooldown, monitor/shutdown, or testing. Re-estimate after each completed
  checkpoint; do not assume every delay can be recovered the next day.
- If an entire block produces no progress, isolate one failing case and ask a
  specific question with the code and observed behavior before expanding scope.
- Preserve all mandatory functionality, including FIFO, EDF, cooldown, safe
  shutdown, and cleanup. Reduce optional refactoring and presentation polish first.

### Your Next Session

1. Complete the Makefile wiring and compile the existing program with the required
   flags. Fix build blockers before adding thread behavior.
2. Verify your three time helpers. Explain which start time each one uses and how
   a failure reaches its caller; measure a short sleep.
3. Implement `log_status()` and verify its timestamp and complete-line locking.
4. If time remains, create and join one worker. Otherwise, carry that task into
   the next session. Real dongles and burnout detection come after this foundation.
