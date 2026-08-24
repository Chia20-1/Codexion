# Day 3 — Condition variables and timing

## Study order

1. Run `01_time_ms.c` and check that elapsed time is close to 250 ms.
2. Run `02_condition_wait.c` and identify the shared predicate.
3. Run `03_signal_and_broadcast.c` several times. Notice that the worker IDs
   are not a guaranteed wake-up order.
4. Run `04_timed_wait.c` and identify where its absolute deadline is built.
5. Run `05_burnout_monitor.c` and watch the monitor replace its old deadline
   when compiling starts.

Build any exercise with:

```sh
cc -g3 -O0 -Wall -Wextra -Werror -pthread 01_time_ms.c -o 01_time_ms
```

Check the threaded exercises with:

```sh
valgrind --tool=helgrind ./02_condition_wait
```

## Mutex versus condition variable

```text
mutex              protects shared state
condition variable lets a thread sleep until that state may have changed
```

A condition variable is never the condition itself. The condition is a
predicate over shared state, such as:

```c
shared->available == 1
shared->permits > 0
now_ms() >= shared->burnout_deadline
```

The predicate must be read and modified while holding its mutex.

## The wait pattern

```c
pthread_mutex_lock(&shared->mutex);
while (!predicate_is_true(shared))
	 pthread_cond_wait(&shared->condition, &shared->mutex);
/* use the protected state */
pthread_mutex_unlock(&shared->mutex);
```

`pthread_cond_wait()` performs three important operations:

1. It atomically releases the mutex and goes to sleep.
2. A signal or broadcast makes the thread eligible to wake.
3. It reacquires the mutex before returning.

Always recheck the predicate in a `while`, not an `if`. A wake-up only means
that the state *may* have changed. Another worker may consume the resource
first, and POSIX also permits spurious wake-ups.

## Signal versus broadcast

- `pthread_cond_signal()` wakes one waiting thread.
- `pthread_cond_broadcast()` wakes all waiting threads.

Neither function transfers mutex ownership. Woken threads still compete to
reacquire the mutex, one at a time.

A condition variable also does not remember an old signal. The shared
predicate carries the persistent information. For example, setting
`available = 1` before signalling means a worker arriving later can still see
that the resource is available.

## Timed waits

`pthread_cond_timedwait()` takes an absolute `struct timespec`, not a duration.
Calculate the absolute deadline once and reuse it when the loop rechecks the
predicate. Recalculating `now + timeout` after every wake-up can accidentally
extend the total wait forever.

The default condition-variable clock is `CLOCK_REALTIME`. These exercises use
`gettimeofday()`, which is based on the same wall clock. In production code,
`CLOCK_MONOTONIC` is often safer for elapsed durations because changing the
system clock cannot move it backwards or forwards.

```
struct timeval pairs with gettimeofday()
```

```
struct timespec pairs with clock_gettime() and pthread_cond_timedwait()
```

## Codexion deadline model

```text
burnout deadline = last_compile_start + time_to_burnout
```

When compiling starts:

1. Lock the state mutex.
2. Update `last_compile_start`.
3. Signal or broadcast the condition variable.
4. Unlock the mutex.

The monitor wakes, observes the new timestamp, calculates a new absolute
deadline, and sleeps again. Finishing a compile does not reset this deadline.

## Peer-check questions

Before moving to Day 4, explain these without reading the code:

1. Why must `pthread_cond_wait()` receive a mutex?
2. Why does it release that mutex while sleeping?
3. Why is the predicate checked with `while`?
4. What is the difference between `signal` and `broadcast`?
5. Why is a timed-wait deadline absolute?
6. When does Codexion reset a coder's burnout deadline?

