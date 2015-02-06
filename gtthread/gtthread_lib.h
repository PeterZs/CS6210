//
//  gtthread_datatype.h
//  gtthread_project1
//
//  Created by Young on 2/2/15.
//  Copyright (c) 2015 Gatech. All rights reserved.
//

#ifndef gtthread_project1_gtthread_datatype_h
#define gtthread_project1_gtthread_datatype_h

#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include <stdbool.h> /* C99 support boolen type */
#include <stddef.h>

typedef long gtthread_t;

typedef struct gtthread_tcb{
  ucontext_t* _context;
  gtthread_t _tid;
  bool _isCompleted; /* if yes, remove it from ready queue in exit function */
  bool _isBlocked; /* For thread_join */
  void* _retVal;
  void* _joinVal;
  struct gtthread_tcb* _joinTcb; /* record thread waiting on me */
}gtthread_tcb;

typedef struct queue_node{
  gtthread_tcb* _tcb;
  struct queue_node* _next;
  struct queue_node* _prev;
}queue_node;

typedef struct queue{
  queue_node* _head;
  queue_node* _tail;
  size_t _size; /* Only keep track of unmarked(uncompleted) # of pcbs */
}queue_t;

typedef struct gtthread_mutex_t{
  bool _lock;
  gtthread_t _tid;
}gtthread_mutex_t;

void init_queue(queue_t* q);

/* In both function context should be got explicitly */
void init_context(ucontext_t* context);
void init_tcb(gtthread_tcb* tcb, gtthread_t* thread); /* Call init_context in this function */

void enqueue(queue_t* q, gtthread_tcb* target_tcb);
void enqueue_node(queue_t* q, queue_node* target_node);
void queue_cycle_signal(queue_t* q); /* Maintain the same queue size */
void queue_cycle_remove(queue_t* q); /* Decrement queue size by 1 */
gtthread_tcb* queue_search(queue_t* q, gtthread_t tid); /* using in gtthread_join() */
queue_node* dequeue(queue_t* q);


void scheduler();
/* use wrapper_func() to record return value */
void wrapper_func(void* (*start_routine)(void*), void* arg);
#endif
