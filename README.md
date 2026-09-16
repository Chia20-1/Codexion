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
| `monitor.sim_state_mutex` | `monitor.should_stop` | `bool` | Whether the simulation must stop |
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

# Resources

1. [Build directory conventions](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html)  
   Explains the use of a separate build directory for generated files, such as object files (`.o`), static libraries (`.a`), and executables. Inspired my choice of `build/` as the folder name.

2. [Pthreads fundamentals](https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM)  
   Covers thread creation, joining threads, passing arguments to thread routines, concurrency versus parallelism, debugging, and checking for memory leaks in multithreaded programs.

### AI Usage
1. Used AI to generate learning modules and self-practice exercises to help me master each external function listed in the subject before starting the project.
