Project 2: Priority CPU Scheduling for xv6
Student Name: Paria Hesami 

Overview
This project implements a priority-based CPU scheduler with an aging mechanism in xv6-riscv. The default scheduler was modified so that, instead of selecting runnable processes in a simple round-robin style, the kernel now selects the runnable process with the highest effective priority. An aging mechanism was also added to reduce starvation of low-priority processes.

Files Modified
kernel/proc.h
kernel/proc.c
kernel/defs.h
kernel/syscall.h
kernel/syscall.c
kernel/sysproc.c
user/user.h
user/usys.pl
user/prioritysanity.c
user/prioritytest1.c
user/prioritytest2.c
Makefile

Implementation Details

1. Process Structure Changes
I modified struct proc in kernel/proc.h by adding the following fields:

- priority
  The current/effective priority used by the scheduler.

- base_priority
  The original priority assigned to the process. This is used to restore the process’s priority after it runs.

- wait_time
  Tracks how long a process has been waiting in the RUNNABLE state for aging purposes.

The valid priority range is 0 to 10, where a higher number means higher priority. The default priority for a newly allocated process is 5.

2. Process Initialization and Inheritance
In allocproc() in kernel/proc.c, each new process is initialized with:

- priority = 5
- base_priority = 5
- wait_time = 0

In kfork(), the child process inherits the parent’s base priority. The child’s effective priority is initialized to the same base priority, and its wait_time is reset to 0.

3. Priority Management System Calls
I implemented the following kernel helper functions in kernel/proc.c:

- setpriority(int pid, int priority)
- getpriority(int pid)

These functions are declared in kernel/defs.h and called by thin syscall wrappers in kernel/sysproc.c.

System call interface changes were added in:
- kernel/syscall.h
- kernel/syscall.c
- user/user.h
- user/usys.pl

Behavior:
- setpriority(pid, priority) returns 0 on success
- setpriority(pid, priority) returns -1 if the PID is invalid or the priority is outside the valid range 0 to 10
- getpriority(pid) returns the current effective priority of the process
- getpriority(pid) returns -1 if the PID is invalid

4. Scheduler Logic
The scheduler in kernel/proc.c was modified to scan the process table and select the RUNNABLE process with the highest effective priority.

Selection rule:
- a higher priority process is preferred
- if two runnable processes have the same priority, the process with the larger wait_time is preferred

This tie-break rule improves fairness and helps aged processes eventually get CPU time once priorities become equal.

5. Aging Mechanism
To reduce starvation, aging is applied to RUNNABLE processes while the scheduler scans the process table.

For each RUNNABLE process:
- wait_time is incremented
- when wait_time reaches the aging threshold, the process priority is increased by 1
- priority is capped at 10
- wait_time is then reset to 0

In my implementation, the aging threshold is 2 scheduler waits. I chose this threshold so that the aging effect is clearly visible during testing and demonstration.

When a process is selected to run:
- priority is reset to base_priority
- wait_time is reset to 0

This ensures that aging acts as a temporary boost rather than permanently changing the assigned priority.

Locking and Synchronization
All process-table traversal and scheduling logic was implemented inside kernel/proc.c.

This design was chosen to preserve xv6 kernel modularity:
- sysproc.c only handles syscall argument extraction and forwards the request to helper functions
- direct process-table traversal logic is not implemented in sysproc.c

Per-process fields such as:
- state
- priority
- base_priority
- wait_time

are accessed or modified while holding p->lock.

This ensures thread-safe access to process state and avoids races with other CPUs or kernel operations.

Testing Approach

1. prioritysanity
This test validates the system call interface.

It verifies:
- default priority is 5
- setpriority() works with a valid value
- getpriority() returns the updated value
- invalid priorities such as 11 are rejected
- invalid PIDs are rejected for both getpriority() and setpriority()
- the priority remains unchanged after an invalid set attempt

Observed result:
- default priority was 5
- setting priority to 8 succeeded
- setting priority to 11 returned -1
- the valid priority remained unchanged afterward
- getpriority(9999) returned -1
- setpriority(9999, 5) returned -1

2. prioritytest1
This test creates three CPU-bound child processes with different priorities:
- HIGH = 9
- MED = 5
- LOW = 1

Observed result:
- the HIGH process completed first
- then the MED process
- then the LOW process

This demonstrates that the scheduler correctly gives preference to higher-priority runnable processes.

3. prioritytest2
This test demonstrates the aging mechanism. It creates:
- HIGH = 7
- LOW1 = 1
- LOW2 = 1

All processes perform CPU-intensive work and periodically print progress.

Observed result:
- the HIGH process receives preference initially
- LOW1 and LOW2 eventually begin running before all work in the system is fully completed
- all processes complete successfully

This demonstrates that aging reduces starvation by allowing long-waiting low-priority processes to eventually receive CPU time.

Notes on Priority Output During Testing
In my implementation, a process’s effective priority is temporarily increased while it waits in the RUNNABLE state. When that process is selected to run, its priority is reset to its base_priority before execution continues.

As a result, printed output during execution may show the base priority value rather than the temporary aged value. Therefore, the main evidence of aging is the scheduling behavior itself: lower-priority processes eventually begin running after waiting, instead of starving indefinitely.

Usage Instructions

1. Build and run xv6:
   make clean
   make qemu

2. Run the test programs inside xv6:
   prioritysanity
   prioritytest1
   prioritytest2

Design Decisions
- I used both priority and base_priority so that aging could temporarily boost a process without permanently overwriting its originally assigned priority.
- I implemented process-table traversal in proc.c instead of sysproc.c to preserve modularity and keep syscall wrappers simple.
- I used per-process locking with p->lock whenever reading or modifying process scheduling fields.
- I used a tie-break based on wait_time when two processes have equal priority, which improves fairness once aging causes priorities to converge.
- I used an aging threshold of 2 scheduler waits so that starvation prevention is clearly observable during testing and demonstration.

Conclusion
This project successfully extends xv6 with:
- priority-based scheduling
- process priority system calls
- child priority inheritance
- an aging mechanism to reduce starvation

Testing confirms that higher-priority processes are preferred and that lower-priority processes eventually receive CPU time due to aging.
