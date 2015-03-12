//
//  CentralizedBarrier.h
//  Centrailized_Barrier
//
//  Created by Qiuxiang Jin on 2/25/15.
//  Copyright (c) 2015 Georgia Institute of Technology. All rights reserved.
//

#ifndef __Centrailized_Barrier__CentralizedBarrier__
#define __Centrailized_Barrier__CentralizedBarrier__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define TRUE 1
#define FALSE 0

typedef struct Sense_Counter{
    int count;
    int sense;
    int numThread;
}SenseCounter;


void init_Central(int thread_num);
void centralBarrier();
#endif /* defined(__Centrailized_Barrier__CentralizedBarrier__) */
