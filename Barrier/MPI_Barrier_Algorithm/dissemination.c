//
//  dissemination.c
//  MPI_Barrier_Algorithm
//
//  Created by Young on 2/27/15.
//  Copyright (c) 2015 Gatech. All rights reserved.
//

#include "dissemination.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define Arrive 0

MPI_Request sendRequest;
MPI_Request recvRequest;
MPI_Status sendStatus;
MPI_Status recvStatus;

void MPI_Dissemination_barrier(MPI_Comm comm){
  int size;
  int rank;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  
  const int numofRound = ceil(log2(size));
  
  int sendRank;
  int recvRank;
  for (int i = 0; i < numofRound; ++i) {
    sendRank = ( rank + (int)pow(2, i) ) % size;
    recvRank = rank - (int)pow(2, i);
    if(recvRank < 0){
      recvRank += size;
    }
    MPI_Isend(NULL, 0, MPI_UINT64_T, sendRank, Arrive, comm, &sendRequest);
    MPI_Irecv(NULL, 0, MPI_UINT64_T, recvRank, Arrive, comm, &recvRequest);
    MPI_Wait(&sendRequest, &sendStatus);
    MPI_Wait(&recvRequest, &recvStatus);
  }
  
}
