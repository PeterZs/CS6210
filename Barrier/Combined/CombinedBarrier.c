//
//  CombinedBarrier.h
//  CombinedBarrier (MCS tree (OpenMP) + Dissemination_barrier (MPI))
//
//  Created by Qiuxiang Jin and Hong Yang on 2/25/15.
//  Copyright (c) 2015 Georgia Institute of Technology. All rights reserved.
//

#include "CombinedBarrier.h"
#define Arrive 0


// Define global variable
int nodeNum;
TreeNode** mcsTreeRef;

MPI_Request sendRequest;
MPI_Request recvRequest;
MPI_Status sendStatus;
MPI_Status recvStatus;

void init_CombinedBarrier(int thread_num){
    int i,j;
    nodeNum = thread_num;
    mcsTreeRef = (TreeNode**) malloc(nodeNum*sizeof(TreeNode*));
    for(i = 0; i < nodeNum; i++){
        mcsTreeRef[i] = (TreeNode*) malloc(sizeof(TreeNode));

    }
    
    for(i = 0; i < nodeNum; i++){
        
        mcsTreeRef[i]->sense = TRUE;
        mcsTreeRef[i]->parentSense = FALSE;
        
        for(j = 0; j < 4; j++){      //deal with haveChildren
            if(4*i + j + 1 < thread_num){
                mcsTreeRef[i]->haveChild[j] = TRUE;
                mcsTreeRef[i]->childNotReady[j] = TRUE;
            }
            else{
                mcsTreeRef[i]->haveChild[j] = FALSE;
                mcsTreeRef[i]->childNotReady[j] = FALSE;
            }
        }
        
        if(i != 0){                 //Set the notification parent
            mcsTreeRef[i]->parent = &mcsTreeRef[(i - 1) / 4]->childNotReady[(i - 1)%4];
        }
        else{
            mcsTreeRef[i]->parent = NULL;
        }
        
        
       
        if(2 * i + 1 < nodeNum){
                mcsTreeRef[i]->leftChild = &mcsTreeRef[2 * i + 1]->parentSense;
        }else{
                mcsTreeRef[i]->leftChild = NULL;
        }
            
        if(2 * i + 2 < nodeNum){
                mcsTreeRef[i]->rightChild = &mcsTreeRef[2 * i + 2]->parentSense;
        }else{
                mcsTreeRef[i]->rightChild = NULL;
        }
        
    
    }
}

void CombinedBarrier(MPI_Comm comm){
    int i;
    int tID = omp_get_thread_num();
    
    //Wait for all the children to be ready
    for(i = 0; i < 4; i++){
        while(mcsTreeRef[tID]->childNotReady[i] == TRUE);
    }
    
    //Reset childNotReady for next barrier
    for(i = 0; i < 4; i++){
        mcsTreeRef[tID]->childNotReady[i] = mcsTreeRef[tID]->haveChild[i];
    }
    
    //Inform parent for its readiness if it is not the root of the tree
    if(tID != 0){
        *(mcsTreeRef[tID]->parent) = FALSE;
    }
    
    // When the node is not root
    if(tID != 0){
        while(mcsTreeRef[tID]->sense != mcsTreeRef[tID]->parentSense);  // wait for parent to wake up
    }else{                                  // The root in the MCS tree is responsible for MPI messaging
        MPI_Dissemination_barrier(comm);    // Used MPI for synchronization between processes
    }
    
    // Wake up its children when if it has
    if(mcsTreeRef[tID]->leftChild != NULL){
        *(mcsTreeRef[tID]->leftChild) = mcsTreeRef[tID]->sense;
    }
  
    if(mcsTreeRef[tID]->rightChild != NULL){
        *(mcsTreeRef[tID]->rightChild) = mcsTreeRef[tID]->sense;
    }
    
    //Reverse its own sense
    mcsTreeRef[tID]->sense = (mcsTreeRef[tID]->sense == TRUE ? FALSE : TRUE);
    
}

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

