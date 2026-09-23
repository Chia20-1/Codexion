*This project has been created as part of the 42 curriculum by \<chilim>*

# Codexion

## Description

A multithreaded simulation where coders share dongles to compile, debug, and
refactor. Requests use FIFO or earliest-deadline-first (EDF) scheduling. A monitor
checks for burnout and whether all coders have completed the required compiles.

## Instructions

Build with `make`, then run:

```sh
./codexion <coders> <burnout_ms> <compile_ms> <debug_ms> <refactor_ms> <required_compiles> <cooldown_ms> <fifo|edf>
```

All eight arguments are required; times are in milliseconds. Use `make clean`
to remove build files or `make fclean` to also remove the executable.

## How It Works

```text
Wait for startup → request dongles → validate and start compile
                 → finish compile → release dongles → debug → refactor → repeat
```

### Scheduling

Coder threads call the scheduler directly; there is no separate scheduler thread.
Each request is enqueued once and waits for both dongles. A condition wait
releases the queue mutex while sleeping and reacquires it before returning.
After waking, the coder checks for shutdown and retries arbitration: a wakeup
alone does not guarantee a grant.

### Why Check the Deadline Again?

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

Activity sleeps currently finish their full duration; they are not interrupted
by shutdown.

## Synchronization

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
- Do not call `log_status()` or `is_stop_requested()` while holding the mutexes they acquire. Compile-start logging prints directly because log and state are already locked.

## Shutdown

| State | Meaning |
|---|---|
| `SIM_RUNNING` | No terminal outcome recorded |
| `SIM_COMPLETED` | All coders reached the required compile count |
| `SIM_BURNOUT` | A coder reached its deadline |
| `SIM_ERROR` | An internal operation failed |

The first terminal state wins. State checks and updates use `sim_state_mutex`;
later errors must not overwrite the original outcome.

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

Used to generate learning exercises for external functions, summarize mutex
responsibilities, and refine README explanations and structure.
