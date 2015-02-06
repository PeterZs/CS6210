//
//  gtthread_lib.c
//  gtthread_project1
//
//  Created by Young on 2/2/15.
//  Copyright (c) 2015 Gatech. All rights reserved.
//

#include "gtthread_lib.h"
#include "gtthread.h"
#include <stdlib.h>
#include <stdio.h>
void init_queue(queue_t* q){
  q->_size = 0;
  q->_head = (queue_node*)malloc(sizeof(queue_node));
  q->_tail = (queue_node*)malloc(sizeof(queue_node));
  q->_head->_next = q->_tail;
  q->_head->_prev = NULL;
  q->_tail->_next = NULL;
  q->_tail->_prev = q->_head;
} /* No need to initialize head and tail's tcb, they're simply senital node */

void init_context(ucontext_t* context){
  getcontext(context);
  context->uc_link = NULL;
  sigemptyset(&context->uc_sigmask); /* Unblock every signal */
  context->uc_stack.ss_sp = (void*)malloc(STACK_SIZE);
  context->uc_stack.ss_flags = 0;
  context->uc_stack.ss_size = STACK_SIZE;
}

void init_tcb(gtthread_tcb* tcb, gtthread_t* thread){
  init_context(tcb->_context);
  *thread =  thread_counter++; /* Everytime assign to a new tcb, increment it */
  tcb->_tid = *thread;
  tcb->_isCompleted = false;
  tcb->_isBlocked = false;
  tcb->_retVal = NULL;
  tcb->_joinVal = NULL;
  tcb->_joinTcb = NULL;
}


void enqueue_node(queue_t* q, queue_node* target_node){
  target_node->_next = q->_tail;
  target_node->_prev = q->_tail->_prev;
  q->_tail->_prev->_next = target_node;
  q->_tail->_prev = target_node;
  ++(q->_size);
}

void enqueue(queue_t* q, gtthread_tcb* target_tcb){
  queue_node* node_to_insert = (queue_node*)malloc(sizeof(queue_node));
  node_to_insert->_tcb = target_tcb;
  enqueue_node(q,node_to_insert);
}


queue_node* dequeue(queue_t* q){
  queue_node* node_to_remove = q->_head->_next;
  node_to_remove->_next->_prev = q->_head;
  q->_head->_next = node_to_remove->_next;
  --(q->_size);
  return node_to_remove;
}

void queue_cycle_signal(queue_t* q){
  queue_node* cycle_node = dequeue(q);
  enqueue_node(q,cycle_node);
}

void queue_cycle_remove(queue_t* q){
  queue_cycle_signal(q);
  --(q->_size);
}

gtthread_tcb* queue_search(queue_t* q, gtthread_t tid){
  
  queue_node* current_node = q->_head->_next;
  while( current_node != q->_tail ){
    if( current_node->_tcb->_tid == tid ){
      break;
    }
    current_node = current_node->_next;
  }
  if( current_node != q->_tail ){
    return current_node->_tcb;
  }
  return NULL;
  
}

void wrapper_func(void* (*start_routine)(void*), void* arg){
  void* return_value = start_routine(arg);
  gtthread_exit(return_value);
}

void pause_timer(){
  struct itimerval zero_timer = {0};
  setitimer(ITIMER_VIRTUAL,&zero_timer,&itimer);
}

void resume_timer(){
  setitimer(ITIMER_VIRTUAL,&itimer,NULL);
}

void scheduler(){ 
  /**
   * There is no need to block signal in the scheduler, since
   * it'll be blocked/unblocked automatically.
   */
  
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  pause_timer();
  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);

  if( ready_queue._size > 1 ){
  
    if(current_tcb->_isCompleted == true){
      queue_cycle_remove(&ready_queue);
    }else{
      queue_cycle_signal(&ready_queue);
    }
    
    gtthread_tcb* prev_current_tcb = current_tcb;
    
    /**
     * Invariant: the first node in the ready queue is always the running
     * tcb...
     */
    while( ready_queue._head->_next->_tcb->_isCompleted == true ||
           ready_queue._head->_next->_tcb->_isBlocked == true){
      /* if program works correctly, there won't be a dead loop */
      queue_cycle_signal(&ready_queue);
    }
    
    current_tcb = ready_queue._head->_next->_tcb;
    resume_timer();
    swapcontext(prev_current_tcb->_context, current_tcb->_context);
    
  }
  else{
    /**
     * Since current_tcb is the only tcb, and it's finished...
     */
    if(current_tcb->_isCompleted == true)
      exit(0);
  }
   
}



