//
//  MCS.h
//  MCS_Barrier
//
//  Created by Qiuxiang Jin on 2/25/15.
//  Copyright (c) 2015 Georgia Institute of Technology. All rights reserved.
//

#ifndef MCS_Barrier_MCS_h
#define MCS_Barrier_MCS_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define TRUE    1
#define FALSE   0

// Define the TreeNode Data Structure for MCS tree
typedef struct TreeNode{
    //Structure for the 4-ary tree
    int haveChild[4];  // Has Chidren
    int childNotReady[4];  // Children not Ready
    
    //Structure for the 2-ary tree
    int sense;         //Initally true
    int parentSense;   //Initally false
    
    //parent to notificate and children to wake up
    int *parent;
    int *leftChild;
    int *rightChild;
    
}TreeNode;


// Initialize the MCS tree barrier
void init_MCS(int thread_num);

// Evoke barrier;
void MCS_barrier();


#endif












