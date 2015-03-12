//
//  CentralizedBarrier.c
//  Centrailized_Barrier
//
//  Created by Qiuxiang Jin on 2/25/15.
//  Copyright (c) 2015 Georgia Institute of Technology. All rights reserved.
//

#include "CentralBarrier.h"

SenseCounter senseCounter;
int* local_sense;

void init_Central(int thread_num){
    int i;
    senseCounter.numThread = thread_num;
    senseCounter.sense = TRUE;       //global sense intialized to TRUE
    senseCounter.count = thread_num;
    local_sense = malloc(thread_num*sizeof(int));
    for(i = 0; i < thread_num; i++){
       local_sense[i] = TRUE;       // Local sense initalized to TRUE
    }
}

void centralBarrier(){
    int num;
    int tID = omp_get_thread_num();
    
    local_sense[tID] = (local_sense[tID] == TRUE ? FALSE : TRUE);
    
    num = __sync_fetch_and_sub(&senseCounter.count, 1);
    
    if(num == 1){                                   // When the thread is the last one
        senseCounter.count = senseCounter.numThread;
	senseCounter.sense = local_sense[tID];
    }else if(num > 1){                                          // When the thread is not the last one
        while(local_sense[tID] != senseCounter.sense);
    }else{
	printf("Counter become <1, fatal fault!\n");
	exit(1);
    }
}
