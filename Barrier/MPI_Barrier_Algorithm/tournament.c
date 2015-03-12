//
//  tournament.c
//  MPI_Barrier_Algorithm
//
//  Created by Young on 2/25/15.
//  Copyright (c) 2015 Gatech. All rights reserved.
//

#include "tournament.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#define Arrive 100
#define Good_to_leave_barrier 101

/**
 * Queue datastructure to store infos.
 */
typedef struct queue_node{
  int _rank;
  struct queue_node* _next;
  struct queue_node* _prev;
}node_t;

typedef struct queue{
  node_t* _head;
  node_t* _tail;
  uint64_t _size;
}queue_t;

void init_queue(queue_t* q){
  q->_head = (node_t*)malloc(sizeof(node_t));
  q->_tail = (node_t*)malloc(sizeof(node_t));
  q->_head->_next = q->_tail;
  q->_head->_prev = NULL;
  q->_tail->_prev = q->_head;
  q->_tail->_next = NULL;
  q->_size = 0;
}

void enqueue(queue_t* q, int rank){
  node_t* newNode = (node_t*)malloc(sizeof(node_t));
  newNode->_rank = rank;
  newNode->_next = q->_tail;
  newNode->_prev = q->_tail->_prev;
  q->_tail->_prev->_next = newNode;
  q->_tail->_prev = newNode;
  ++q->_size;
}

int dequeue(queue_t* q){
  if(q->_size != 0){
    int rank = q->_head->_next->_rank;
    node_t* targetNode = q->_head->_next;
    q->_head->_next = targetNode->_next;
    targetNode->_next->_prev = q->_head;
    free(targetNode);
    --q->_size;
    return rank;
  }else{
    return -1;
  }
}

void erase_queue(queue_t* q){
  while(q->_size != 0){
    dequeue(q);
  }
  free(q->_head);
  free(q->_tail);
}

/**
 * For each processor, keep track of what processors it need to
 * send message to free from barrier.
 */
queue_t processors_to_free;

/**
 * Using Hypercubic permutation to exchange message.
 */
int HyperQubicTarget(int rank, int d){
  int target = 1;
  target <<= d;
  target ^= rank;
  return target;
}

void MPI_Tournament_barrier(MPI_Comm comm){
  
  /* Get info to make local decision for individual processor */
  int rank;
  int size;
  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);
  
  if(size == 1){
    return;
  }
  
  MPI_Status status;
  MPI_Request req;
  
  /* Deal with the case: if p is NOT power of 2 */
  const int numofRound = log2(size); /* number of round = floor(log2(size)) */
  const int numofParticipants = pow(2, numofRound); /* the actual number of node participating in tournament barrier algorithm */
  
  /**
   * The first 2^(numofRound) of processors are those who actually
   * Participating in the tournament barrier algorithm. The left
   * processors need to first communicate to The first 2^(numofRound) 
   * of processors to let them proceed.
   */
  if(rank < numofParticipants){
    /* Only those node who is participating in actual tournament communication may have a chance to have children */
    init_queue(&processors_to_free);
    /* If it has a child, wait for its child */
    if(rank < size - numofParticipants){
      int childRank = numofParticipants + rank;
      MPI_Recv(NULL, 0, MPI_UINT64_T, childRank, Arrive, comm, &status);
      enqueue(&processors_to_free, childRank);
    }
    
    /* The kernel of tournament barrier algorithm */
    for (int i = 0; i < numofRound; ++i) {
      int targetRank = HyperQubicTarget(rank, i);

      if( rank > targetRank ){
        /* Tell its conterpart that it has arrived */
        MPI_Send(NULL, 0, MPI_UINT64_T, targetRank, Arrive, comm);
        /* Blocking wait until its conterpart come to free */
        MPI_Recv(NULL, 0, MPI_UINT64_T, targetRank, Good_to_leave_barrier, comm, NULL);
        break;
      }
      else{
        /* Blocking wait for its conterpart */
        MPI_Recv(NULL, 0, MPI_UINT64_T, targetRank, Arrive, comm, &status);
        /* Put its conterpart into queue */
        enqueue(&processors_to_free, targetRank);
      }
      
    }
    
    /* Before leaving, free all the node in its queue */
    while (processors_to_free._size > 0) {
      int targetRank = dequeue(&processors_to_free);
      MPI_Isend(NULL, 0, MPI_UINT64_T, targetRank, Good_to_leave_barrier, comm, &req);
    }
    
    /* Before exit, clean globle queue */
    erase_queue(&processors_to_free);
    
  }
  else{
    int dest = rank - numofParticipants;
    /* Tell its parent that it've arrived */
    MPI_Send(NULL, 0, MPI_UINT64_T, dest, Arrive, comm);
    /* Blocking wait until its parent free it */
    MPI_Recv(NULL, 0, MPI_UINT64_T, MPI_ANY_SOURCE, Good_to_leave_barrier, comm, NULL);
   
  }
  
  return;
}