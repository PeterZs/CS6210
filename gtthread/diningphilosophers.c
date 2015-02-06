#include <stdio.h>
#include <stdbool.h>
#include "gtthread.h"
#include <stdlib.h>

typedef enum p_status{
  No_Chops,
  One_Chop,
  Eating
}p_status;

typedef struct chopsticks{
  bool _taken;
  gtthread_t _owner;
}chopsticks_t;

/* Global status */
chopsticks_t chopsticks[5];
int num_of_eating;
int num_of_left_chopsticks;
p_status p_status_array[5];
gtthread_mutex_t global_lock;
unsigned int seeds[5];

void grabchopsticks(gtthread_t tid, size_t l, size_t r){
  gtthread_mutex_lock(&global_lock);
  if(p_status_array[tid-1] == One_Chop){
    if(chopsticks[l]._owner == tid){
      if(chopsticks[r]._taken == false){
        chopsticks[r]._owner = tid;
        chopsticks[r]._taken = true;
        ++num_of_eating;
        --num_of_left_chopsticks;
        p_status_array[tid-1] = Eating;
        printf("Philosopher #%d gets right chopstick and eating\n",tid);
      }
    }
    else if(chopsticks[r]._owner == tid){
      if(chopsticks[l]._taken == false){
        chopsticks[l]._owner = tid;
        chopsticks[l]._taken = true;
        ++num_of_eating;
        --num_of_left_chopsticks;
        p_status_array[tid-1] = Eating;
        printf("Philosopher #%d gets left chopstick and eating\n",tid);
      }
    }
  }
  else if(p_status_array[tid-1] == No_Chops){
    if(num_of_left_chopsticks == 1 && num_of_eating == 0){
      // do nothing
    }
    else{
      if(chopsticks[r]._taken == false && chopsticks[l]._taken == false){
	chopsticks[r]._owner = tid;
        chopsticks[r]._taken = true;
        chopsticks[l]._owner = tid;
        chopsticks[l]._taken = true;
        ++num_of_eating;
        num_of_left_chopsticks -= 2;
        p_status_array[tid-1] = Eating;
        printf("Philosopher #%d gets both chopsticks and eating\n", tid);
      }
      else if(chopsticks[r]._taken == false){
        chopsticks[r]._owner = tid;
        chopsticks[r]._taken = true;
        --num_of_left_chopsticks;
        p_status_array[tid-1] = One_Chop;
        printf("Philosopher #%d gets right chopstick\n",tid);
      }
      else if(chopsticks[l]._taken == false){
        chopsticks[l]._owner = tid;
        chopsticks[l]._taken = true;
        --num_of_left_chopsticks;
        p_status_array[tid-1] = One_Chop;
        printf("Philosopher #%d gets left chopstick\n",tid);
      }
    }
  }
  gtthread_mutex_unlock(&global_lock);
}

void releasechopsticks(gtthread_t tid,size_t l,size_t r){
  gtthread_mutex_lock(&global_lock);
  chopsticks[l]._taken = false;
  chopsticks[l]._owner = -1;
  chopsticks[r]._taken = false;
  chopsticks[r]._owner = -1;
  --num_of_eating;
  num_of_left_chopsticks += 2;
  p_status_array[tid-1] = No_Chops;
  printf("Philosopher #%d releases both chopsticks\n",tid);
  printf("Philosopher #%d is thinking\n",tid);
  gtthread_mutex_unlock(&global_lock);
}

void* philosophers(void* arg){
  
  size_t l_index = 0;
  size_t r_index = 0;  
  long tid = gtthread_self();

  switch(tid){
    case 1:{
      l_index = 1;
      r_index = 0;
      break;
    }
    case 2:{
      l_index = 2;
      r_index = 1;
      break;
    }
    case 3:{
      l_index = 3;
      r_index = 2;
      break;
    }
    case 4:{
      l_index = 4;
      r_index = 3;
      break;
    }
    case 5:{
      l_index = 0;
      r_index = 4;
      break;
    }
    default:
      printf("ERROR: You're NOT using gtthread library implemented by Hong (Aaron) Yang!\n");
      break;
  }
  while(1){
    if(p_status_array[tid-1] == Eating){
      releasechopsticks(tid,l_index,r_index);
      int i = 0;
      for(; i < 30000*rand_r(&seeds[tid-1])/RAND_MAX;++i);
      gtthread_yield();
    }
    else{
      int i = 0;
      grabchopsticks(tid,l_index,r_index);
      printf("Philosopher #%d is hungry\n",tid);
      for(; i < 30000*rand_r(&seeds[tid-1])/RAND_MAX;++i);
      gtthread_yield();
    }
  }

}

int main(){

  num_of_eating = 0;
  num_of_left_chopsticks = 0;
  
  gtthread_init(1000);
  gtthread_t threads[5];

  gtthread_mutex_init(&global_lock);
  int i;
  for( i = 0; i < 5; ++i){
    chopsticks[i]._taken = false;
    chopsticks[i]._owner = -1;
    p_status_array[i] = No_Chops;
    seeds[i] = time(NULL);
  }
  for( i = 0; i < 5; ++i){
    gtthread_create(&threads[i],philosophers,NULL);    
  }  

  gtthread_exit(NULL);
  return 0;
}
