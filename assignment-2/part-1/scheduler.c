#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED

#include "scheduler.h"

#include <assert.h>
#include <curses.h>
#include <ucontext.h>

#include "util.h"

// This is an upper limit on the number of tasks we can create.
#define MAX_TASKS 128

// This is the size of each task's stack memory
#define STACK_SIZE 65536

#define READY 0
#define WAITING 1
#define SLEEPING 2
#define BLOCKED 3
#define DONE 4

// This struct will hold the all the necessary information for each task
typedef struct task_info {
  // This field stores all the state required to switch back to this task
  ucontext_t context;

  // This field stores another context. This one is only used when the task
  // is exiting.
  ucontext_t exit_context;

  // TODO: Add fields here so you can:
  //   a. Keep track of this task's state.
  int state;

  //   b. If the task is sleeping, when should it wake up?
  size_t wake_up_time;

  //   c. If the task is waiting for another task, which task is it waiting for?
  task_t wait_for_task;

  //   d. Was the task blocked waiting for user input? Once you successfully
  //      read input, you will need to save it here so it can be returned.
  int input;

} task_info_t;

int current_task = 0; //< The handle of the currently-executing task
int num_tasks = 1;    //< The number of tasks created so far
task_info_t tasks[MAX_TASKS]; //< Information for every task

/**
 * Initialize the scheduler. Programs should call this before calling any other
 * functiosn in this file.
 */
void scheduler_init() {
  // TODO: Initialize the state of the scheduler
    int i;

    for(i=0; i<num_tasks; i++) {
        tasks[i].state = READY;
        tasks[i].wake_up_time = 0;
    }
}


/**
 * This function will execute when a task's function returns. This allows you
 * to update scheduler states and start another task. This function is run
 * because of how the contexts are set up in the task_create function.
 */
void task_exit() {
  // TODO: Handle the end of a task's execution here
    // set state of the task to done (4)
    tasks[current_task].state = DONE;

    // invoke scheduler
    scheduler(current_task + 1);
}

/**
 * Create a new task and add it to the scheduler.
 *
 * \param handle  The handle for this task will be written to this location.
 * \param fn      The new task will run this function.
 */
void task_create(task_t* handle, task_fn_t fn) {
  // Claim an index for the new task
  int index = num_tasks;
  num_tasks++;

  // Set the task handle to this index, since task_t is just an int
  *handle = index;

  // We're going to make two contexts: one to run the task, and one that runs at the end of the task so we can clean up. Start with the second

  // First, duplicate the current context as a starting point
  getcontext(&tasks[index].exit_context);

  // Set up a stack for the exit context
  tasks[index].exit_context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].exit_context.uc_stack.ss_size = STACK_SIZE;

  // Set up a context to run when the task function returns. This should call task_exit.
  makecontext(&tasks[index].exit_context, task_exit, 0);

  // Now we start with the task's actual running context
  getcontext(&tasks[index].context);

  // Allocate a stack for the new task and add it to the context
  tasks[index].context.uc_stack.ss_sp = malloc(STACK_SIZE);
  tasks[index].context.uc_stack.ss_size = STACK_SIZE;

  // Now set the uc_link field, which sets things up so our task will go to the exit context when the task function finishes
  tasks[index].context.uc_link = &tasks[index].exit_context;

  // And finally, set up the context to execute the task function
  makecontext(&tasks[index].context, fn, 0);

  // set state of task to ready (0)
  tasks[index].state = READY;

  // set wake_up_time to 0 (default)
  tasks[index].wake_up_time = 0;
}

/**
 * Wait for a task to finish. If the task has not yet finished, the scheduler should
 * suspend this task and wake it up later when the task specified by handle has exited.
 *
 * \param handle  This is the handle produced by task_create
 */
void task_wait(task_t handle) {
  // TODO: Block this task until the specified task has exited.
    tasks[current_task].state = BLOCKED; // set state to blocked
    tasks[current_task].wait_for_task = handle; // set task that it's waiting for

    // invoke scheduler
    scheduler(handle);
}

/**
 * The currently-executing task should sleep for a specified time. If that time is larger
 * than zero, the scheduler should suspend this task and run a different task until at least
 * ms milliseconds have elapsed.
 *
 * \param ms  The number of milliseconds the task should sleep.
 */
void task_sleep(size_t ms) {
  // TODO: Block this task until the requested time has elapsed.
  // Hint: Record the time the task should wake up instead of the time left for it to sleep. The bookkeeping is easier this way.
    size_t current_time;
    tasks[current_task].state = SLEEPING; // set state to sleeping
    current_time = time_ms();
    tasks[current_task].wake_up_time = current_time + ms; // set wake up time for task

    // invoke scheduler
    scheduler(current_task + 1);
}

/**
 * Read a character from user input. If no input is available, the task should
 * block until input becomes available. The scheduler should run a different
 * task while this task is blocked.
 *
 * \returns The read character code
 */
int task_readchar() {
  // TODO: Block this task until there is input available.
  // To check for input, call getch(). If it returns ERR, no input was available.
  // Otherwise, getch() will returns the character code that was read.
    int c = getch();
    if(c != ERR) {
        tasks[current_task].input = c;
		return tasks[current_task].input;
    }

    tasks[current_task].state = WAITING;
    scheduler(current_task + 1);
    return tasks[current_task].input;

  return ERR;
}

void scheduler(task_t task) {
    int i, ct, ndt = 0, f = 0;

    // loop over tasks array
    for(i=0; i<=num_tasks; i++) {
        // start loop at task to be waited for or task after sleeping task
        if(f == 0) {
            i = task;
            f = 1;
        }

        // restart loop when the end of the loop is reached
        if(i == num_tasks) {
            i = 0;
        }

        // if task is ready, set current_task to it and run it
        if(tasks[i].state == READY) {
            ct = current_task;
            current_task = i;
            if(ct == i) { // if switching to same context, return
                return;
            }
            swapcontext(&tasks[ct].context, &tasks[i].context);
        }
        // if task is waiting for input
        else if(tasks[i].state == WAITING) {
            int c = getch();
			if(c != ERR) {
                tasks[i].input = c; // set task input to c
                tasks[i].state = READY;

                ct = current_task;
                current_task = i;
                if(ct == i) { // if switching to same context, return
                    return;
                }
                swapcontext(&tasks[ct].context, &tasks[i].context);
            }
        }
        // if task is sleeping
        else if(tasks[i].state == SLEEPING) {
            size_t current_time = time_ms();
            if(current_time >= tasks[i].wake_up_time) {
                tasks[i].state = READY;
            }
        }
        // if task is blocked (waiting for a task to finish)
        else if(tasks[i].state == BLOCKED) {
            for(int j=0; j<num_tasks; j++) {
                if(tasks[j].state == DONE && tasks[i].wait_for_task == j) {
                    tasks[i].state = READY;
                }
            }
        }

        ndt = 0;
        // if all tasks are done, end loop
        for(int j=1; j<num_tasks; j++) {
            if(tasks[j].state == DONE) {
                ndt++;
            }
        }
        if(ndt == num_tasks-1) {
            break;
        }
    }

    // switch to main task ?
    ct = current_task;
    current_task = 0;
    if(ct == 0) {
        return;
    }
    swapcontext(&tasks[ct].context, &tasks[0].context);
}