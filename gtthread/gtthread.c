
#include "gtthread.h"
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>

void gtthread_init(long period){
  
  thread_counter = 0; /* start with only 1 main thread */
  init_queue(&ready_queue);
  
  /* Allocating memory for tcb and its corresponding context first */
  Main_tcb = (gtthread_tcb*)malloc(sizeof(gtthread_tcb));
  Main_tcb->_context = (ucontext_t*)malloc(sizeof(ucontext_t));
  init_tcb(Main_tcb,&main_tid);
  enqueue(&ready_queue,Main_tcb);
  current_tcb = Main_tcb;
  
  /* initialize sigaction */
  action.sa_handler = &scheduler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGVTALRM,&action,NULL);
  
  /* initialize itimerval */
  itimer.it_interval.tv_sec = 0;
  itimer.it_interval.tv_usec = (suseconds_t)(period);
  itimer.it_value.tv_sec = 0;
  itimer.it_value.tv_usec = (suseconds_t)(period);
  
  /* initialize global blocker */
  sigemptyset(&global_blocker);
  sigaddset(&global_blocker, SIGVTALRM);
  
  time_slice = period; 
  isInitialized = true;
  setitimer(ITIMER_VIRTUAL,&itimer,NULL);
}

int gtthread_create(gtthread_t *thread,
                    void* (*start_routine)(void*),
                    void* arg){
  
  /* Like a critical section */
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  
  if(isInitialized == false){
    printf("Using thread without initializing\n");
    exit(0);
  }
  
  /* Create new tcb and its corresponding context */
  gtthread_tcb* new_tcb = (gtthread_tcb*)malloc(sizeof(gtthread_tcb));
  new_tcb->_context = (ucontext_t*)malloc(sizeof(ucontext_t));
  init_tcb(new_tcb,thread);
  makecontext(new_tcb->_context,wrapper_func,2,start_routine,arg);
  
  /* Push this new tcb into tails of the ready queue */
  enqueue(&ready_queue, new_tcb);
  
  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
  return 0;
}

void gtthread_exit(void* retval){
  
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  current_tcb->_isCompleted = true;
  current_tcb->_retVal = retval;
  if (current_tcb->_joinTcb != NULL) {
    current_tcb->_joinTcb->_joinVal = retval;
    current_tcb->_joinTcb->_isBlocked = false;
  }
  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
  
  scheduler();
}

int gtthread_join(gtthread_t thread, void** status){
  
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  gtthread_tcb* tcb_waiting_for = queue_search(&ready_queue, thread);
  if(tcb_waiting_for == NULL){
    /* No such tcb in the ready queue */
    printf("No such thread_id!\n");
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    return -1;
  }
  else{
    if (tcb_waiting_for->_isCompleted == true) {
      if(status != NULL)
        *status = tcb_waiting_for->_retVal;
      sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
      return 0;
    }
    else{
      current_tcb->_isBlocked = true;
      tcb_waiting_for->_joinTcb = current_tcb;
      sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
      
      scheduler();
      
      sigprocmask(SIG_BLOCK, &global_blocker, NULL);
      if(status!=NULL){
        *status = current_tcb->_joinVal;
      }
      sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
      return 0;
    }
  
  }

}

int gtthread_yield(void){
  scheduler();
  return 0;
}

int gtthread_equal(gtthread_t t1,
                   gtthread_t t2){
  if(t1 == t2)
    return 1;
  else
    return 0;
}

int gtthread_cancel(gtthread_t thread){
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  gtthread_tcb* target_tcb = queue_search(&ready_queue, thread);
  if(target_tcb == NULL) {
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    return -1;
  }
  
  if(target_tcb->_isCompleted == true){
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    return 0;
  }
  
  target_tcb->_isCompleted = true;
  --ready_queue._size;
  int* garbage = (void*)malloc(sizeof(int));
  target_tcb->_retVal = (void*)garbage;
  *garbage = GTTHREAD_CANCELED;
  
  if(target_tcb->_joinTcb != NULL){
    target_tcb->_joinTcb->_isBlocked = false;
    target_tcb->_joinTcb->_joinVal = (void*)garbage;
  }
  
  if(current_tcb->_tid == thread){
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    scheduler();
  }

  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
  return 0;
}

gtthread_t gtthread_self(void){
  return current_tcb->_tid;
}

int gtthread_mutex_init(gtthread_mutex_t* mutex){
  
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  mutex->_lock = false;
  mutex->_tid = -1; /* No owner initially */
  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
  return 0;
}

int gtthread_mutex_lock(gtthread_mutex_t* mutex){
  
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  
  if(mutex->_tid == current_tcb->_tid){
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    printf("Error: recursive lock!\n");
    return -1;
  }
  
  while(mutex->_lock == true){
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    scheduler();
    sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  }
  
  mutex->_lock = true;
  mutex->_tid = current_tcb->_tid;
  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
  
  return 0;
}

int gtthread_mutex_unlock(gtthread_mutex_t* mutex){
  
  sigprocmask(SIG_BLOCK, &global_blocker, NULL);
  if(mutex->_tid != current_tcb->_tid){
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    printf("Error: try to unlock lock belonging to other thread\n");
    return -1;
  }
  else if(mutex->_lock == false){
    sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
    printf("Error: try to unlock an unlocked lock\n");
    return -1;
  }
  else{
    mutex->_lock = false;
    mutex->_tid = -1;
  }
  
  sigprocmask(SIG_UNBLOCK, &global_blocker, NULL);
  return 0;
}

