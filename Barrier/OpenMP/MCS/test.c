//
//  main.c
//  MCS_Barrier
//
//  Created by Qiuxiang Jin on 2/25/15.
//  Copyright (c) 2015 Georgia Institute of Technology. All rights reserved.
//

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <omp.h>
# include <sys/time.h>
# include <time.h>
# include "MCS.h"


#define THREAD_NUM 5
#define ROUND 50000

int main(int argc, char **argv)
{
    
    double result_array[32];
    double sum = 0;
    double average_time;
    int i;
    int total_thread;

    
    if(argc == 2){
        total_thread = atoi(argv[1]);
    }
    else if (argc == 1){
        total_thread =THREAD_NUM;
    }
    else{
        printf("Parameter Error!\n");
        exit(0);
    }
    
    omp_set_num_threads(total_thread);
    init_MCS(total_thread);
    
#pragma omp parallel
    {
        // Now we're in the parallel section
        struct timeval start, end;   //Used for count time
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        int round;
        long total_time;
        gettimeofday(&start, NULL);
	    
        for(round = 0 ; round < ROUND; round++){
           // #pragma omp barrier
	       MCS_barrier();
        }
        
        gettimeofday(&end, NULL);
        total_time = (end.tv_sec*1000000 + end.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
        result_array[thread_num] = total_time/(double)round;
        MCS_barrier();
        
    } // implied barrier

    for(i = 0; i < total_thread; i++){
    	sum += result_array[i];        
    }
    

    average_time = sum/total_thread;
    
    
    printf("Total average: %f\n", average_time);     

    return 0;
}
