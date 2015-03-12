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
# include <mpi.h>
# include <sys/time.h>
# include <time.h>
# include "CombinedBarrier.h"


#define THREAD_NUM 3
#define ROUND 28

int main(int argc, char **argv)
{
    
    int my_process_id,i;
    double individual_time[32];
    double sum = 0;
    int total_thread;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_process_id);
    
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
    init_CombinedBarrier(total_thread);

    
#pragma omp parallel
    {
        // Now we're in the parallel section
        struct timeval start, end;   //Used for count time
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        int round;
        long total_time;
        
        CombinedBarrier(MPI_COMM_WORLD);
		
        gettimeofday(&start, NULL);
        for(round = 0 ; round < ROUND; round++){
            // #pragma omp barrier
            CombinedBarrier(MPI_COMM_WORLD);
        }
		//double time = stopwatch_stop(timer);
        gettimeofday(&end, NULL);
        
        total_time = (end.tv_sec*1000000 + end.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
        individual_time[thread_num] = total_time/(double)ROUND;
        
        CombinedBarrier(MPI_COMM_WORLD);
    } // implied barrier
    
    for(i = 0; i < total_thread; i++){
        sum += individual_time[i];
    }
    
    printf("process %d: %f\n", my_process_id, sum/total_thread);
    return MPI_Finalize();
}
