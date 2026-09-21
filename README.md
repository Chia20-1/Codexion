*This project has been created as part of the 42 curriculum by \<chilim>*

# Description

# Instructions

# Mutexes

### Mutex Responsibilities

The table defines which mutex to hold when reading or writing shared mutable
state during simulation. Scheduler coordination and compile progress updates are
still being implemented; these are the locking rules those additions must follow.

| Mutex | Variable / resource | Type | Purpose |
|---|---|---|---|
| `monitor.sim_state_mutex` | `monitor.state` | `t_sim_state` | Whether the simulation is running, or why it stopped |
| `monitor.sim_state_mutex` | `monitor.simulation_started` | `bool` | Whether coders may begin their work |
| `monitor.sim_state_mutex` | `monitor.wait_error` | `int` | Shared condition-wait error code |
| `monitor.sim_state_mutex` | `coder->last_compile_start` | `long long` | Most recent compile start, read by the scheduler and monitor |
| `monitor.sim_state_mutex` | `coder->compile_count` | `int` | Completed compiles, read by the monitor |
| `monitor.sim_state_mutex` | `data->start_time` | `long long` | Set at startup before publishing `simulation_started`; remains unchanged afterward |
| `dongle->mutex` | `dongle->current_owner` | `t_coder *` | Coder currently reserving this dongle |
| `dongle->mutex` | `dongle->cooldown_deadline` | `long long` | Earliest timestamp when this dongle can be acquired again |
| `scheduler.request_queue_mutex` | `scheduler.request_heap[i]` | `t_request *` | Heap entries and their ordering |
| `scheduler.request_queue_mutex` | `scheduler.heap_size` | `int` | Number of queued requests |
| `scheduler.request_queue_mutex` | `scheduler.arrival_counter` | `unsigned long` | Assigns fresh request arrival numbers |
| `scheduler.request_queue_mutex` | `request->burnout_deadline` | `long long` | Request's deadline key; keep unchanged while queued |
| `scheduler.request_queue_mutex` | `request->arrival_order` | `unsigned long` | Request's arrival key; keep unchanged while queued |
| `scheduler.request_queue_mutex` | `request->dongles_granted` | `bool` | Whether the request has received both dongles |
| `monitor.log_output_mutex` | Status output (`printf`) | Output stream | Serializes status messages; does not protect a struct field |

A mutex does not automatically attach to these variables: every thread must
follow the same locking agreement. Local copies, such as a local
`last_compile_start`, do not need a mutex after the protected read.

Values initialized before threads use them and left unchanged, such as `config`,
`scheduler.policy`, `scheduler.heap_capacity`, and coder/dongle/request links,
do not need a lock for each read. `data->start_time` is published through the
startup state lock and can be read without locking after that startup barrier.
Cleanup reads after all relevant threads have joined also need no lock.

Pair acquisition and release require the queue mutex **and** both distinct
dongle mutexes. The queue mutex coordinates scheduling; each dongle mutex
protects that dongle's fields.

### Mutex Flow

```
ACQUIRE
──────────────────────────────►

request_queue → dongle[low] → dongle[high] → log → state


RELEASE
◄──────────────────────────────

request_queue ← dongle[low] ← dongle[high] ← log ← state
```

# Scheduler Flow

```text
Lock queue → enqueue once → arbitrate → granted?
                              ↑           │
                              │           ├─ yes → unlock queue → return
                              │           │
                              └── wake ── wait
                                         releases queue while sleeping
```

The wait reacquires `request_queue_mutex` before returning. After waking, the
coder checks stop and runs arbitration again; it does not enqueue its request
again. A wakeup or timeout does not guarantee a grant.

# Coder Compile Cycle

After the start gate:

```text
coder_routine()
    │
    ├─ scheduler_process_request(coder)
    │      Wait until granted or stopped/error
    │
    ├─ Record compile start and log actions
    │
    ├─ Wait for compile duration
    │
    ├─ Count the compile if fully completed
    │
    ├─ scheduler_release_dongles(coder)
    │
    ├─ Debug
    │
    └─ Refactor
```

Repeat the cycle while running. The coder thread calls the scheduler functions;
they do not run in a separate scheduler thread. If stop or an error interrupts
the cycle, release any owned pair before exiting, even if compilation did not
finish. Only fully completed compiles increase `compile_count`.

# Simulation State

- `SIM_RUNNING`: Simulation has no recorded terminal outcome; initialized before threads start.
- `SIM_COMPLETED`: All coders reached the required compile count.
- `SIM_BURNOUT`: A coder reached its burnout deadline.
- `SIM_ERROR`: An internal operation failed.

Only the first transition from `SIM_RUNNING` to a terminal state takes effect.
Protect both the state check and update with `monitor.sim_state_mutex`, including
direct assignments in error paths. Later stop requests must not overwrite the
original outcome. `monitor.wait_error` retains condition-wait error details
separately from the simulation outcome.

`is_stop_requested(data)` locks the state mutex and returns whether
`monitor.state != SIM_RUNNING`. When already holding that mutex, compare the
state directly instead of calling the getter and attempting to lock it again.

### Requesting Stop

```c
request_stop(data, SIM_ERROR);
```

`request_stop(t_data *data, t_sim_state reason)` accepts `SIM_COMPLETED`,
`SIM_BURNOUT`, or `SIM_ERROR`; other values are ignored. Call it without holding
the state, log, or queue mutex. For a valid reason, it:

1. Locks `sim_state_mutex` and records the reason only if still `SIM_RUNNING`.
2. Broadcasts `wakeup_cond`, then unlocks the state mutex.
3. Calls `scheduler_clear_queue()`, which locks the queue mutex, clears queued
   references, broadcasts `request_queue_cond`, and unlocks the queue mutex.

Repeated valid requests still notify waiters and clear the queue while preserving
the first terminal outcome. Releasing state before acquiring queue respects the
lock order used by queue waiters. A granted coder releases its own dongles on
exit; request storage is freed during cleanup after threads have joined.

The current finite-loop scaffold requests completion after successful coder
joins. The monitor still waits for a stop request; automatic burnout detection
and monitor-driven global completion are later Day 5 work. Recording
`SIM_BURNOUT` alone does not detect or print a burnout.

### Exit Status

`main()` returns `0` for completion or burnout when startup and joins succeed and
no condition-wait error is recorded. It returns `1` for input, initialization,
startup, join, or recorded runtime failure. An operational failure can therefore
produce a nonzero exit status without replacing an earlier terminal outcome.

# Resources

1. [Build directory conventions](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html)  
   Explains the use of a separate build directory for generated files, such as object files (`.o`), static libraries (`.a`), and executables. Inspired my choice of `build/` as the folder name.

2. [Pthreads fundamentals](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM)  
   Covers thread creation, joining threads, passing arguments to thread routines, concurrency versus parallelism, debugging, and checking for memory leaks in multithreaded programs.

### AI Usage
1. Generate learning modules and self-practice exercises to help me master each external function listed in the subject before starting the project.
2. Create a table summarizing each mutex’s purpose and the shared data it protects.
