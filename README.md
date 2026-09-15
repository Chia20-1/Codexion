*This project has been created as part of the 42 curriculum by \<chilim>*

# Description

# Instructions

# Mutexes

### Mutex Responsibilities

| Mutex | Protects |
|---|---|
| `monitor.state_mutex` | Shared simulation/monitor state |
| `dongle->mutex` | That dongle’s ownership and cooldown |
| `scheduler.request_queue_mutex` | Scheduler queue and request coordination |
| `monitor.log_output_mutex` | Log output, preventing overlapping messages |

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