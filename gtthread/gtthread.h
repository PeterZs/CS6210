#ifndef __GTTHREAD_H
#define __GTTHREAD_H
#include "gtthread_lib.h"
#include <stddef.h>

#define STACK_SIZE 128000000
#define GTTHREAD_CANCELED 16

/**
 * Global variables are initialized in gtthread_init() function
 */
size_t thread_counter;     /* start with only 1 main thread */
queue_t ready_queue;       /* global queue maintained by library */
gtthread_tcb* Main_tcb;    /* Keep track of main thread's tcb */
gtthread_tcb* current_tcb; /* keep track of current running tcb */

struct itimerval itimer;   /* Global timer */
sigset_t global_blocker;   /* Block signal in every library procedure,
                            * it won't block entire thread
                            */
struct sigaction action;   /* Global handler of signal pointing to schedule() func */
bool isInitialized;        /* Global flag for initialization */
gtthread_t main_tid;
long time_slice;



/* Must be called before any of the below functions. Failure to do so may
 * result in undefined behavior. 'period' is the scheduling quantum (interval)
 * in microseconds (i.e., 1/1000000 sec.). */
void gtthread_init(long period);

/* see man pthread_create(3); the attr parameter is omitted, and this should
 * behave as if attr was NULL (i.e., default attributes) */
int  gtthread_create(gtthread_t *thread,
                     void *(*start_routine)(void *),
                     void *arg);

/* see man pthread_join(3) */
int  gtthread_join(gtthread_t thread, void **status);

/* gtthread_detach() does not need to be implemented; all threads should be
 * joinable */

/* see man pthread_exit(3) */
void gtthread_exit(void *retval);

/* see man sched_yield(2) */
int gtthread_yield(void);

/* see man pthread_equal(3) */
int  gtthread_equal(gtthread_t t1, gtthread_t t2);

/* see man pthread_cancel(3); but deferred cancelation does not need to be
 * implemented; all threads are canceled immediately */
int  gtthread_cancel(gtthread_t thread);

/* see man pthread_self(3) */
gtthread_t gtthread_self(void);


/* see man pthread_mutex(3); except init does not have the mutexattr parameter,
 * and should behave as if mutexattr is NULL (i.e., default attributes); also,
 * static initializers do not need to be implemented */

int  gtthread_mutex_init(gtthread_mutex_t *mutex);
int  gtthread_mutex_lock(gtthread_mutex_t *mutex);
int  gtthread_mutex_unlock(gtthread_mutex_t *mutex);

/* gtthread_mutex_destroy() and gtthread_mutex_trylock() do not need to be
 * implemented */

#endif // __GTTHREAD_H
