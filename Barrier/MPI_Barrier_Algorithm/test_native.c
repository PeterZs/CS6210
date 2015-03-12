//
//  main.c
//  MPI_Barrier_Algorithm
//
//  Created by Young on 2/25/15.
//  Copyright (c) 2015 Gatech. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include "tournament.h"
#include "dissemination.h"
#include "timer.h"
#include <sys/time.h>
#include <time.h>
#define numofTrial 1000000

int main(int argc, char * argv[]) {
  
  MPI_Init(&argc, &argv);
  /* Start timer */
 // stopwatch_init();
 // struct stopwatch_t* timer = stopwatch_create();
 // stopwatch_start(timer);
 
  struct timeval start, end;
  gettimeofday(&start, NULL);	
  int i = 0; 
  for (i= 0; i < numofTrial; ++i) {
    MPI_Barrier(MPI_COMM_WORLD);
  }
  gettimeofday(&end, NULL);
  //MPI_Tournament_barrier(MPI_COMM_WORLD);
  //MPI_Barrier(MPI_COMM_WORLD);
  /* Stop timer and get the time */
  long double time = (end.tv_sec*1000000 + end.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
  printf("average time = %Lf \n",time/numofTrial);
  
  return MPI_Finalize();
}
