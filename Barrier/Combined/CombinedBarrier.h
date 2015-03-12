//
//  CombinedBarrier.h
//  CombinedBarrier (MCS tree (OpenMP) + Dissemination (MPI))
//
//  Created by Qiuxiang Jin and Hong Yang on 2/25/15.
//  Copyright (c) 2015 Georgia Institute of Technology. All rights reserved.
//

#ifndef Combined_Barrier_MCS_h
#define Combined_Barrier_MCS_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <mpi.h>


#define TRUE    1
#define FALSE   0

// Define the TreeNode Data Structure for MCS tree
typedef struct TreeNode{
    //Elements for the 4-ary tree
    int haveChild[4];  // Has Chidren
    int childNotReady[4];  // Children not Ready
    int *parent;           // Pointer to the parent to notify
    
    //Elements for the 2-ary tree
    int sense;         //Initally true
    int parentSense;   //Initally false
    int *leftChild;     // Pointer to the left chidren to wake up
    int *rightChild;    // Pointer to the right chidren to wake up
}TreeNode;


// Initialize the MCS tree barrier
void init_CombinedBarrier(int thread_num);

// Evoke barrier
void CombinedBarrier(MPI_Comm comm);

// MPI barrier
void MPI_Dissemination_barrier(MPI_Comm comm);  //Used inside the Combined Barrier

#endif












