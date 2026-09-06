# Day 1 — Build, Time, Logging, and One Thread

[Schedule](Seven_Day_Completion.md) · [Next: Day 2](Seven_Day_02.md)

## Goal and time budget

Make the existing project build, verify your timing functions, and run one worker
that prints a complete status line and is joined before cleanup. Budget three
two-hour blocks including learning and debugging. Carry unfinished steps forward.
These are the revised schedule's days, not the original lesson numbers.

Your time helpers are already written. Review and test them rather than rewriting
them. Today does not implement real dongle acquisition or burnout detection.

## Files and responsibilities

| File | Work |
|---|---|
| `Makefile` | Compile sources to objects and link the executable |
| `include/codexion.h` | Includes and shared function declarations |
| `srcs/time.c` | Current time, elapsed time, and millisecond waits |
| `srcs/log.c` | Timestamped, serialized status messages |
| `srcs/coder.c` | One temporary worker routine |
| `srcs/thread.c` | Create and join that worker |
| `srcs/main.c` | Initialize, run, then clean up |

## Step 1 — Establish a working build

1. List the implemented `.c` files in a `SRCS` variable. Include `main.c`, parsing,
   initialization, cleanup, and timing; add logging/thread files as you implement them.
2. Derive object paths under `objs/`, preserving `parse/` and `init/` subdirectories.
3. Make the executable depend on those objects and pass the objects to the link
   command. Your existing Makefile does not yet connect these pieces.
4. Keep `cc`, `-Wall -Wextra -Werror -pthread`, and `-I include/`.
5. Make objects depend on the header, so header changes trigger recompilation.
6. Build and fix errors one at a time. For example, remove an unused local rather
   than weakening `-Werror`.

Understand the two operations:

```text
source.c --compile--> object.o
all object files --link--> codexion
```

Check with `make`, then `make` again. The second command should have no compile or
link work when nothing changed. Do not undertake an unrelated directory redesign.

## Step 2 — Verify the time interface

Check these declarations against their definitions:

```c
long long get_time_ms(void);
long long get_elapsed_ms(t_data *data);
bool      sleep_ms(long long duration_ms);
```

Add `<unistd.h>` for `usleep()` and ensure the sleep prototype is in your header.
`<sys/time.h>` supplies `struct timeval`. Use `#include "codexion.h"` in sources.

Trace each helper on paper:

| Helper | Reference point | Example |
|---|---|---|
| `get_time_ms()` | Clock epoch | Current absolute millisecond value |
| `get_elapsed_ms(data)` | Simulation start | Current 10500 minus start 10000 = 500 ms |
| `sleep_ms(50)` | This individual wait's start | Return after about 50 ms of waiting |

Your `1000LL` multiplication uses `long long`. The microsecond division drops the
fractional millisecond. Your chosen `-1` error value must be checked before
subtraction. A failed sleep must reach its caller rather than silently completing
the activity. This simple error convention assumes ordinary positive clock values;
the wall clock can move if the system time is adjusted.

## Step 3 — Run a small timing check

Use a temporary test driver, compiled separately from the project's `main.c`:

```text
before = get_time_ms(); check for failure
sleep_ms(50); check for failure
after = get_time_ms(); check for failure
print after - before
```

Repeat with 1, 10, and 50 ms. Allow scheduling variation; do not demand exact
equality. Check that zero duration returns promptly. For an elapsed-time check,
set a test `data.start_time` first, wait, and read `get_elapsed_ms(&data)`.

Keep test-only initialization separate from the final start gate. In the final
program, the simulation clock starts after thread creation, immediately before
the gate opens. Measure clock time, not CPU time: a sleeping thread still ages.

## Step 4 — Implement one action logger

Use the existing `log_status(t_coder *coder, const char *status)` interface.

```text
lock coder->data->monitor.log_output_mutex
obtain elapsed time
if successful: print timestamp, coder ID, and status as one line
unlock log_output_mutex on every path
```

Example shape: `50 1 is compiling`. Use `%lld` for the timestamp and `%d` for the
ID. Include `<stdio.h>` where needed. Keep diagnostics separate from action logs.

The mutex covers timestamp generation and the entire line. Never sleep while
holding it. Today, call the logger only with a valid initialized start time.

Decide how logging failures reach the simulation: either change the return type
to report failure, updating the header and callers, or arrange a shared failure
path. A `void` logger must not silently turn a failed timestamp into an action
line. Add coordinated stopping in Day 2; refine terminal logging in Day 5.

Exact action strings from the subject:

```text
has taken a dongle
is compiling
is debugging
is refactoring
burned out
```

Reserve `burned out` for the monitor. The temporary worker can log `is refactoring`
once without claiming that real dongles have been acquired.

## Step 5 — Create and join one worker

First have the worker immediately return `NULL`. Then add one log call.

1. Cast its `void *argument` back to `t_coder *`.
2. Pass `&data->coders[0]` to `pthread_create()`, not a loop variable's address.
3. Set `thread_created` only after successful creation.
4. Join only that successfully created thread.
5. Clean shared data only after successful joining establishes the worker is done.

For this one-worker exercise, set `start_time` before creating the worker. Day 2
replaces this temporary setup with a common gate. Do not put thread creation into
`init_data()`.

Pthread creation and joining return zero on success and an error number otherwise;
check `!= 0`, unlike the `-1` convention of `gettimeofday()`.
See the [creation](https://man7.org/linux/man-pages/man3/pthread_create.3.html) and
[joining](https://man7.org/linux/man-pages/man3/pthread_join.3.html) manuals.

## Step 6 — Connect main and verify

Keep the ownership sequence visible:

```text
zero data → parse → initialize → run_simulation → cleanup → return status
```

Use `./codexion 1 1000 10 10 10 1 0 fifo` for this temporary exercise. A worker
should run and exit. This is not yet final one-coder behavior.

## Common mistakes

- Declaring a helper without compiling its source into the executable.
- Printing an epoch timestamp instead of simulation elapsed time.
- Treating `sleep_ms()` failure as successful completion.
- Returning from the logger while its mutex is still locked.
- Freeing the coder array while its worker still uses the pointer.

## Completion checklist

- [ ] Existing sources build with required flags; a second make does not relink.
- [ ] Each time helper's reference point and failure result are understood.
- [ ] Short waits have been measured.
- [ ] One worker logs a complete line and is joined before cleanup.
- [ ] README notes explain current time versus elapsed time versus waiting.

Explain aloud: Why does `sleep_ms()` need its own start value? Who owns the coder
pointer? Why does `pthread_join()` come before cleanup?

If the build or timing checks remain broken, begin the next session there.

Subject reference: [codexion.pdf](codexion.pdf), global rules and mandatory part.
