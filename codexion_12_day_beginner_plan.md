# Codexion — Revised Beginner Plan

> Revised 6 September 2026. Target: finish in seven more working days from the
> current timing-helper stage, while understanding the code well enough to explain
> and modify it during evaluation. Seven days is a stretch target, not a deadline.

## How to Read This Revised Plan

The **seven-day schedule below replaces the original calendar**. The original
“Day 1” through “Day 12” headings remain as lesson references, so “Day 6, Step 3”
still means the same thing. In those lessons, “today” and “end-of-day” describe a
module's scope and completion checkpoint, not a one-day time limit.

Do not restart the earlier lessons. Continue from the code you have, revisiting
concepts when the next task needs them.

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

Use the revised schedule's workload assumption above. Count time spent asking
questions, tracing code, and debugging as project work, not as lost time.

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
- shared data reference (`t_data *`)

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

### `t_data`

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

## Goal and Scope

Today, connect the data created on Day 5 to real threads. By the end of the day,
the program should create `N` coder threads and one placeholder monitor thread,
start them safely, produce serialized logs, join every created thread, and only
then clean the shared data.

Day 6 is about **thread lifecycle and logging**, not correct dongle arbitration.
It is acceptable to fake the two dongle-acquisition messages today. Leave these
features for their later days:

- Real dongle ownership and cooldown: Day 7
- Heap-backed FIFO arbitration: Day 8
- EDF scheduling and final deadlock strategy: Day 9
- Burnout detection and final stop conditions: Day 10

## What You Should Understand Before Coding

Be able to explain this lifecycle:

```text
initialize all shared data
        ↓
create threads
        ↓
release a common start gate
        ↓
threads run concurrently
        ↓
request them to stop when necessary
        ↓
join every successfully created thread
        ↓
destroy mutexes/conditions and free memory
```

`pthread_create()` may let the new thread run immediately. Therefore, never
create a thread before all data that it may access has been initialized.

`pthread_t` itself does not need `pthread_*_init()`. `pthread_create()` writes
the thread handle into it. A successfully created joinable thread must later be
passed to `pthread_join()`.

## Day 6 Shared-State Contract

Decide which mutex protects each value before writing thread code:

| State | Protection on Day 6 |
|---|---|
| `monitor.should_stop` | `monitor.sim_state_mutex` |
| `monitor.simulation_started` | `monitor.sim_state_mutex` |
| `data.start_time` | Set under `sim_state_mutex` before start broadcast; read afterward |
| `coder.last_compile_start` | `monitor.sim_state_mutex` |
| `coder.compile_count` | `monitor.sim_state_mutex` |
| Terminal output | `monitor.log_output_mutex` |
| Thread-created flags | Main thread only, before workers are joined |

Do not sometimes read a shared value with its mutex and sometimes without it.
That would still be a data race.

## Step 1 — Add Thread Lifecycle Fields

Choose where the thread handles and creation flags live. A simple beginner-friendly
design is one handle per coder and one handle in the monitor:

```c
struct s_coder
{
	/* existing fields */
	pthread_t	thread;
	bool		thread_created;
};

struct s_monitor
{
	/* existing fields */
	pthread_t	thread;
	bool		thread_created;
	bool		simulation_started;
};
```

The exact field order is your choice. Because `t_data` is zero-initialized and
the coder array is allocated with `ft_calloc()`, both creation flags initially
start as `false`.

Why keep a creation flag?

```text
create coder 1: success
create coder 2: success
create coder 3: failure
```

Only coder 1 and coder 2 have valid thread handles. The failure path must wake
and join those two threads, but must not try to join coder 3.

Do not destroy or free a `pthread_t`. Joining is the matching lifecycle operation.

## Step 2 — Add the Day 6 Function Prototypes

Plan the public interface before filling the source files. Names may differ, but
you will likely need functions with these responsibilities:

```c
long long	get_time_ms(void);
long long	get_elapsed_ms(t_data *data);
void		log_status(t_coder *coder, const char *status);
void		*coder_routine(void *argument);
void		*monitor_routine(void *argument);
bool		run_simulation(t_data *data);
```

Keep helpers used by only one `.c` file `static`. Only shared functions belong in
`codexion.h`.

## Step 3 — Implement Millisecond Time Helpers in `time.c`

Use `gettimeofday()` to obtain an absolute millisecond value:

```text
milliseconds = seconds * 1000 + microseconds / 1000
```

Important details:

- Use `long long` in the multiplication.
- A displayed timestamp is `current_time - data->start_time`.
- Do not set `start_time` during `init_data()`. Thread creation takes time, so set
  it immediately before releasing all threads through the start gate.
- Check the return value of `gettimeofday()` if you design your helper to report
  failure.

For Day 6, implement a millisecond sleep helper using `usleep()` in small chunks.
Avoid converting an arbitrarily large millisecond value into one large
microsecond value, because that conversion can overflow the type accepted by
`usleep()`.

A useful conceptual loop is:

```text
end = current time + requested duration
while current time is before end:
    sleep for a small remaining chunk
```

Later, this helper should also check `should_stop` between chunks so coder threads
can exit promptly after burnout. Do not chase Day 10 precision yet.

### Time Helper Check

Write a temporary test or use a small simulation duration:

```text
before = get_time_ms()
sleep approximately 50 ms
after = get_time_ms()
```

Verify that `after - before` is approximately 50 ms, allowing normal scheduler
delay.

## Step 4 — Implement Serialized Logging in `log.c`

The final project accepts only these state messages:

```text
timestamp id has taken a dongle
timestamp id is compiling
timestamp id is debugging
timestamp id is refactoring
timestamp id burned out
```

For a normal Day 6 action logger:

```text
lock log_output_mutex
calculate elapsed timestamp
print one complete line
unlock log_output_mutex
```

Keep the mutex locked for the entire line, not separately around each value.
Otherwise two threads could produce output such as:

```text
12 13 1 2 is compiling
```

Use exact status strings so you do not spread slightly different spellings across
the program. For example, pass `"is debugging"` to one logger instead of creating
a different printing function for every status.

Do not keep the logging mutex locked while sleeping or doing coder work. Logging
should hold it only long enough to print one line.

The Day 10 monitor will add the stronger rule that no ordinary messages may print
after burnout. For today, focus on non-interleaved lines and use one consistent
locking order whenever a function needs both `sim_state_mutex` and
`log_output_mutex`.

## Step 5 — Build a Start Gate

Without a start gate, coder 1 may begin compiling while main is still creating
coder 20. That gives later-created coders less time before their first burnout
deadline.

Initialize:

```text
simulation_started = false
should_stop = false
```

At the beginning of every thread routine:

```text
lock sim_state_mutex
while simulation has not started AND stop was not requested:
    wait on wakeup_cond using sim_state_mutex
remember whether stop was requested
unlock sim_state_mutex

if stop was requested:
    return from the thread
```

Always use `while`, not `if`, around `pthread_cond_wait()`. Condition variables may
wake spuriously, and a broadcast only means that the predicate **may** have
changed.

After all thread creations succeed, main releases the start gate:

```text
lock sim_state_mutex
start_time = get_time_ms()
for every coder:
    last_compile_start = start_time
simulation_started = true
broadcast wakeup_cond
unlock sim_state_mutex
```

The predicate and broadcast are performed while holding the same mutex. This
prevents a thread from missing the transition between checking the predicate and
going to sleep.

## Step 6 — Write a Temporary Coder Routine in `coder.c`

Pass a pointer to the actual coder, not the address of the loop variable:

```c
pthread_create(&coder->thread, NULL, coder_routine, coder);
```

Do **not** do this:

```c
pthread_create(&coder->thread, NULL, coder_routine, &i);
```

All threads would share the same changing `i`.

Inside the routine:

1. Cast `argument` back to `t_coder *`.
2. Wait at the common start gate.
3. If startup was cancelled, return `NULL`.
4. Repeat a finite temporary lifecycle.
5. Return `NULL`.

For Day 6 only, a finite fake lifecycle may be:

```text
while this coder has not completed the requested count:
    log "has taken a dongle"          ← fake for now
    log "has taken a dongle"          ← fake for now

    under sim_state_mutex:
        last_compile_start = current absolute time
    log "is compiling"
    sleep time_to_compile

    under sim_state_mutex:
        increment compile_count
        broadcast wakeup_cond

    log "is debugging"
    sleep time_to_debug

    log "is refactoring"
    sleep time_to_refactor
```

This is scaffolding, not the final simulation. In particular:

- It does not really acquire dongles.
- It does not enforce cooldown.
- It does not arbitrate requests.
- It does not correctly handle the final one-coder behavior.
- Final completion stopping will be refined on Day 10.

Protect `compile_count` and `last_compile_start` now because the real monitor will
read them concurrently later. After changing data relevant to the monitor,
broadcast `wakeup_cond` so this communication pattern is already in place.

Do not hold `sim_state_mutex` during compile, debug, or refactor sleeps.

## Step 7 — Create a Placeholder Monitor Routine in `monitor.c`

Day 10 implements deadline selection and burnout. Today, the monitor thread only
needs a safe lifecycle without busy waiting:

```text
lock sim_state_mutex
while simulation has not started AND stop was not requested:
    wait on wakeup_cond
while stop was not requested:
    wait on wakeup_cond
unlock sim_state_mutex
return NULL
```

The loop must re-check `should_stop` after every wakeup. Coder state broadcasts may
wake the monitor even though the simulation should continue.

This placeholder proves that you can create, wake, stop, and join the separate
monitor thread. Do not add a polling loop such as this:

```c
while (!should_stop)
	usleep(1000);
```

## Step 8 — Own Thread Creation and Joining in `thread.c`

`run_simulation()` should own the complete thread lifecycle. A recommended order
is:

```text
create monitor thread
mark monitor.thread_created = true

for every coder:
    create coder thread
    if successful:
        mark coder.thread_created = true
    otherwise:
        go to the startup-failure path

release the start gate
join every coder thread
request monitor stop and broadcast
join monitor thread
return success
```

### Normal Stop for the Day 6 Placeholder

After all coder threads finish their finite temporary loops:

```text
lock sim_state_mutex
should_stop = true
broadcast wakeup_cond
unlock sim_state_mutex
```

Then join the monitor. Do not set `should_stop` before joining the coders on the
normal Day 6 path, or they may exit before exercising their lifecycle.

### Partial `pthread_create()` Failure

If any creation fails:

```text
lock sim_state_mutex
should_stop = true
broadcast wakeup_cond
unlock sim_state_mutex

join only coder threads whose thread_created flag is true
join the monitor only if monitor.thread_created is true
return failure
```

The broadcast is essential: already-created threads may be asleep at the start
gate and otherwise never reach a point where they can be joined.

Try to join all successfully created threads even if one `pthread_join()` reports
an error. Record that an error happened, continue joining the others, and return
failure afterward.

After a successful join, you may reset that thread's creation flag to `false`.

## Step 9 — Connect the Lifecycle in `main.c`

The high-level structure should become:

```text
zero-initialize data
parse input
initialize shared resources
run simulation
cleanup shared resources
return the appropriate status
```

Conceptually:

```c
if (!init_data(&data))
	return (1);
if (!run_simulation(&data))
{
	cleanup_data(&data);
	return (1);
}
cleanup_data(&data);
```

This contract assumes that `run_simulation()` joins every thread it successfully
created, including on failure. `cleanup_data()` must never destroy a mutex or free
coder data while a thread could still use it.

## Step 10 — Implement in Small, Testable Passes

Do not write all Day 6 code before compiling. Use these passes:

### Pass A — One Thread

- Add thread fields and prototypes.
- Create one coder thread.
- Pass `&data->coders[0]`.
- Have it return immediately.
- Join it successfully.

### Pass B — N Threads

- Create all coder threads in a loop.
- Confirm each thread receives a distinct coder ID.
- Add the creation flags and partial-failure cleanup path.

### Pass C — Start Gate

- Make every coder wait.
- Create all coders and the monitor.
- Set the common start time and broadcast.
- Verify no thread remains stuck during normal or failed startup.

### Pass D — Logging

- Add elapsed timestamps.
- Add the output mutex.
- Log complete single lines.
- Confirm that high thread counts do not produce mixed lines.

### Pass E — Temporary Lifecycle

- Add compile, debug, and refactor phases.
- Use small timing arguments while testing.
- Keep fake dongle messages visibly documented as temporary scaffolding.

## Step 11 — Day 6 Test Matrix

Use short durations so mistakes do not leave you waiting:

```bash
./codexion 1 1000 10 10 10 1 0 fifo
./codexion 2 1000 10 15 20 2 0 fifo
./codexion 5 1000 5 5 5 3 0 edf
./codexion 20 1000 1 1 1 2 0 fifo
```

At this stage, test thread infrastructure rather than real scheduling semantics.

Check each run:

- The program exits rather than hanging.
- Exactly `N` coder threads were created and joined.
- The monitor thread was stopped and joined.
- Each output line contains one timestamp, one valid coder ID, and one complete
  permitted message.
- Timestamps are relative to the common start time and are never negative.
- No mutex or condition variable is destroyed before joins finish.
- Running repeatedly does not intermittently hang.

When available, use:

```bash
valgrind --leak-check=full ./codexion 5 1000 5 5 5 2 0 fifo
valgrind --tool=helgrind ./codexion 5 1000 5 5 5 2 0 fifo
```

Treat Helgrind reports as leads to investigate, not as automatic proof of a bug.

## Common Day 6 Mistakes

| Mistake | Consequence |
|---|---|
| Passing `&i` from the creation loop | Threads observe the wrong or same ID |
| Creating threads inside `init_data()` | Workers may access partially initialized state |
| Setting `start_time` before creating all threads | Later threads lose part of their initial deadline |
| Using `if` around `pthread_cond_wait()` | Spurious wakeups can pass the gate incorrectly |
| Forgetting the startup-failure broadcast | Created threads wait forever and cannot be joined |
| Joining a handle after failed creation | Undefined or invalid thread-handle use |
| Detaching threads | Main cannot reliably wait before cleanup |
| Freeing coder data before joins | Use-after-free by running threads |
| Holding the log mutex during sleeps | Other threads cannot log for long periods |
| Reading `should_stop` without its mutex | Data race |
| Busy-waiting on shared state | Wasted CPU and still potentially racy |
| Implementing real scheduling today | Too many new failure sources at once |

## Revised Timing for This Module

Spread this module across **Remaining Days 1–2**, using the work blocks at the top
of this document. The old single-session estimate was too compressed for learning
time helpers, logging, a start gate, and thread failure handling together.

Your time helpers are already written, so begin with verification rather than
rewriting them. If the start gate or failure path needs another session, extend
this module before integrating real dongles. The completion checklist below is
the checkpoint; the calendar alone does not establish completion.

## Day 6 Completion Checklist

- [ ] All shared state is initialized before the first `pthread_create()`.
- [ ] The program creates exactly `N` coder threads.
- [ ] The program creates one monitor thread.
- [ ] Every created thread waits behind a common start gate.
- [ ] `start_time` and initial coder deadlines are set immediately before broadcast.
- [ ] Every successful `pthread_create()` has a matching `pthread_join()`.
- [ ] Partial thread-creation failure wakes and joins earlier threads.
- [ ] Each coder receives its own stable `t_coder *` argument.
- [ ] Logging uses one mutex around each complete output line.
- [ ] Timestamps are elapsed milliseconds from the common start time.
- [ ] Coder threads execute a finite compile/debug/refactor scaffold.
- [ ] The placeholder monitor sleeps on a condition variable instead of polling.
- [ ] No thread can access data after `cleanup_data()` begins.
- [ ] Repeated short test runs exit without hangs or mixed log lines.

## End-of-Day Goal

You can explain and demonstrate this statement:

> Main initializes all shared state, creates `N` coder threads and one monitor,
> releases them from a synchronized start gate, joins every successfully created
> thread, and only then cleans the data. Coder logs remain complete and
> non-interleaved even though the threads run concurrently.

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

# Original Lesson Reference Map

These numbers identify lessons, not the revised working days. Use the seven-day
schedule at the top to decide what to work on next.

| Original lesson | Main Target | Finished When |
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

# Recommended Work-Block Routine

For each two-hour block in the revised schedule:

```text
15 min  Recall the relevant concept and define one concrete result
60 min  Implement a small piece; compile as you go
30 min  Check behavior and investigate failures
15 min  Explain the code and record the next task
```

Adjust the split when a concept needs more explanation. If a function cannot yet
be explained, trace it with a small example before integrating it. Take breaks
between blocks and stop adding scope when concentration drops.

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

Finish the current checkpoint first. Thread lifecycle, shared-state protection,
safe shutdown, and cleanup are requirements throughout development, not tasks to
defer until the end. Keep FIFO, EDF, cooldown, and burnout detection in scope.

Defer optional folder changes, abstraction rewrites, extra features, and visual
README polish. Keep brief documentation and focused checks alongside each change.

The difficult portion is **Remaining Days 3–5**: heap and resource ownership,
scheduler integration, then monitoring and stopping. If these take longer, shift
the remaining schedule and retain final testing/defence time. With no fixed
deadline, a verified project completed later is a valid outcome of this plan.
