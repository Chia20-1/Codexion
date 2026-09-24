*This project has been created as part of the 42 curriculum by \<chilim>*

# Codexion

## Description

A multithreaded simulation where coders share dongles to compile, while debugging and refactoring do not require dongles. Requests use FIFO or earliest-deadline-first (EDF) scheduling. A monitor checks for burnout and whether all coders have completed the required compiles.

## Instructions

Build with `make` or `make all`, then run:

```sh
./codexion <coders> <burnout_ms> <compile_ms> <debug_ms> <refactor_ms> <required_compiles> <cooldown_ms> <fifo|edf>
```

All eight arguments are required; times are in milliseconds. Use `make clean`
to remove build files or `make fclean` to also remove the executable.

## Blocking Case Handled

### Deadlock Prevention

- Reserve both dongles or neither, preventing a coder from owning one while waiting for the other.
- Lock dongle mutexes in lower-index-first order to prevent circular waiting; unlock them after checking or updating ownership.

### Coffman's four conditions
Deadlock requires all four conditions to hold.
1. **Mutual exclusion**: Each dongle only has at most one owner.
2. **Hold and wait**: A coder reserves both dongles together or neither, so it never owns just one while waiting for the other.
3. **No preemption**: Dongles are released by their owning coder; another coder cannot forcibly take them.
4. **Circular wait**: Nested mutex acquisition follow a consisten order, preventing a cycle of threads waiting on locks.

### Starvation Prevention

- Process requests by arrival order (FIFO) or burnout deadline (EDF), preserving their priority while pending.
- Block requests sharing dongles with an earlier waiting request in that order, preventing unfair bypassing. Independent requests may proceed.
- This policy does not guarantee compilation before burnout.

### Cooldown Handling

- On release, clear ownership and set `cooldown_deadline = release_time + dongle_cooldown`, preventing immediate reuse.
- Use timed condition waits for pending unowned pairs to finish cooling down, avoiding busy polling. Recheck availability after every wakeup.

### Burnout Detection

- Detect burnout when `now >= last_compile_start + time_to_burnout`.
- Hold sim_state_mutex during the check so the coder cannot update last_compile_start while the monitor reads it.
- Wait until the nearest deadline or a notification, then recheck; OS scheduling may delay detection.
- Recheck before starting a granted compile so a late timestamp update cannot hide burnout.

### Log Serialization

- Hold log_output_mutex across both dongle messages and the compile-start message so another thread cannot print between them.
- Acquire `sim_state_mutex` after the log mutex and hold both through the state check and output, preventing normal messages after burnout.

## Thread Synchronization Mechanisms

### Implementation
**`pthread_mutext_t`**: Protects shared data by allowing only one thread at a time to hold a given mutex. Used for dongle ownership/cooldowns, the request queue, simulation staate/coder progress, and logging.

**`pthread_cond_t`**: Threads sleep until notified or a timeout expires. Waiting releases the associated mutex and reacquires it before returning; threads then recheck the sim_state.

- `wakeup_cond`: During simulation, coders notify the monitor when compilation starts or finishes. At shutdown, a broadcast wakes threads waiting on it so they can exit. These waits use `sim_state_mutex` to safely check the shared state.

- `request_cond_cond`: Coders wait here for dongles. Grants, releases, shutdown, or cooldown timeouts wake them to recheck their request under `request_queue_mutex`.

### Shared Resources

| Shared resource | How access is coordinated |
|---|---|
| **Dongles** | Hold the queue mutex and both dongle mutexes when reserving or releasing a pair. Reserve both or neither. |
| **Logging** | Hold the log mutex across related messages; also hold the state mutex during the state check and printing. |
| **Monitor state** | Use `sim_state_mutex` for coder timestamps, compile counts, and simulation state so reads and updates cannot race. |

### Race Condition Prevention
- Coders and the monitor hold sim_state_mutex when accessing timestamps and compile counts, preventing reads during updates.
- Logging holds both the log and state mutexes through the state check and printing, preventing a stop transition between them.

### Thread Communication
- Coders update timestamps or compile counts under sim_state_mutex, then broadcast wakeup_cond to notify the monitor.
- The monitor’s condition wait releases the mutex while sleeping and reacquires it before returning.
- The monitor rechecks shared data under that mutex.
At shutdown, broadcasts wake monitor and scheduler waiters so they can observe the stop state and exit.

## Mutex Responsibilities

Every thread must follow the same locking agreement:

| Mutex | Protects |
|---|---|
| `monitor.sim_state_mutex` | Simulation state, startup flag, wait errors, coder timestamps and compile counts; publishes `start_time` at startup |
| `scheduler.request_queue_mutex` | Request heap, heap size, arrival counter, request deadline/order and grant flag |
| `dongle->mutex` | Dongle owner and cooldown deadline |
| `monitor.log_output_mutex` | Status output |

Acquire nested locks in this order:

```text
request_queue → dongle[low] → dongle[high] → log → state
```

- Acquiring or releasing a dongle pair requires the queue mutex and both distinct dongle mutexes.
- Condition waits release their mutex while sleeping and reacquire it before rechecking state.
- Immutable configuration and local copies need no lock; `start_time` is immutable after the startup barrier.
- Do not call `log_status()` or `is_stop_requested()` when the caller already has the mutexes to prevent deadlock. 

## Function Flow

### Main Thread: Startup and Cleanup

```text
main()
├─ parse_input() → init_data()
├─ run_simulation()
│  ├─ create_monitor() → pthread_create(monitor_routine)
│  ├─ create_coders()  → pthread_create(coder_routine) for each coder
│  ├─ start_simulation()
│  │  └─ set timestamps and simulation_started → broadcast wakeup_cond
│  ├─ join_coders()  → wait for workers to exit
│  └─ join_monitor() → wait for monitor to exit
└─ cleanup_data()
```

This is the normal startup path with a nonzero compile requirement. The coder
and monitor routines run in separate threads. `start_simulation()` opens their
startup gate; main then waits while they work.

### Coder Thread: Request and Compile

```text
coder_routine()
├─ coder_wait_for_start()
└─ repeat while running:
   ├─ coder_compile()
   │  ├─ coder_start_compile()
   │  │  ├─ scheduler_process_request()
   │  │  │  ├─ queue_request() → push_heap()
   │  │  │  └─ wait_for_grant()
   │  │  │     ├─ scheduler_grants_request()
   │  │  │     │  ├─ build_waiting_list() → dongle_pair_try_acquire()
   │  │  │     │  └─ push waiting requests back into the heap
   │  │  │     └─ if not granted: scheduler_wait() → recheck stop/retry
   │  │  └─ if granted: coder_run_compile() → validate_compile_status()
   │  └─ if started: coder_wait_compile_duration()
   │     └─ sleep_ms() → count if completed and running → release dongles
   ├─ coder_debug()
   └─ coder_refactor()
```

Scheduler functions run in the calling coder thread. Requests are enqueued once;
condition waits release the queue mutex and reacquire it before returning.
A wakeup alone does not guarantee a grant.

### Monitor Thread: Burnout and Shutdown

```text
monitor_routine()
├─ wait for startup gate
├─ monitor_loop()
│  ├─ monitor_check() → scan_coders()
│  │  ├─ expired coder: set scan->victim → record SIM_BURNOUT → print burnout
│  │  └─ all completed: record SIM_COMPLETED
│  ├─ still running: monitor_wait_next_dl() → wake/timeout → check again
│  └─ stopped: broadcast wakeup_cond → leave loop
└─ scheduler_clear_queue() → clear requests → broadcast request_queue_cond
```

Workers observe stop, release any reserved dongles, and return so main can finish
joining them. With **one coder**, left and right are the same dongle, so pair
acquisition fails before locking. Its request stays pending until the monitor
detects burnout and wakes the queue waiter to exit.

### Why Coder Has to Check the Deadline Again Before Compiling?

A grant gives permission to compile, but does not guarantee immediate execution.
The OS scheduler may delay the coder, and it must acquire the log and state
mutexes before starting. The deadline could pass during that delay.

Before updating `last_compile_start`, the coder checks:

```text
deadline = last_compile_start + time_to_burnout
expired  = now >= deadline
```

If expired, it returns `COMPILE_EXPIRED`, notifies the monitor, releases its
dongles, and waits for shutdown. It preserves the old timestamp and prints no
compile-start messages, allowing the monitor to detect and announce burnout.

Otherwise, it updates the timestamp, logs the start, and sleeps without holding
mutexes. Afterward, it counts the completed compile only if the simulation is
still running, notifies the monitor, and releases the dongles.

Activity sleeps check stop between short waits, allowing shutdown before the
full duration finishes. Interrupted compiles are not counted.


### Shutdown

| State | Meaning |
|---|---|
| `SIM_RUNNING` | No terminal outcome recorded |
| `SIM_COMPLETED` | All coders reached the required compile count |
| `SIM_BURNOUT` | A coder reached its deadline |
| `SIM_ERROR` | An internal operation failed |

Once the simulation records a stop reason,the program do not change it. Always hold `sim_state_mutex` while checking or updating the simulation state.

Call `request_stop(data, reason)` without holding the state, log, or queue mutex.
It records the first valid stop reason, wakes monitor waiters, then clears the
request queue and wakes scheduler waiters. Each coder releases its granted
dongles; cleanup frees request storage after threads join.

Exit status is `0` for completion or burnout when startup and joins succeed
without a recorded wait error, or `1` for input, initialization, thread, or
recorded runtime failures.

## Resources

- [Build directory conventions](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html) — inspired the separate `build/` directory.
- [Pthreads fundamentals](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM) — threads, synchronization, debugging, and memory leaks.

### AI Usage

Used to generate learning exercises for phtread system functions, summarize mutex
responsibilities, and refine README explanations and structure.
