/*
 * Copyright (c) 1999, 2000 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University 
 * may not be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Cheng Jin (chengjin@eecs.umich.edu)
 *         Qian Chen (qianc@eecs.umich.edu)
 *
 * "@(#) $Id: expansion.c,v 1.1.1.1 2002/05/09 14:08:20 jwinick Exp $ (UM)";
*/
#include <stdio.h>
#include <stdlib.h>
#include "inet.h"
#include "random.h"


/**************************************************************/
/* constant, most of them are from empirical observations     */
/* http://boardwatch.internet.com/isp/summer99/backbones.html */
/* feel free to change as you see fit                         */
/**************************************************************/
#define NETWORK_SIZE 20 /* size of an expanded network */
#define AVG_ENNOD   3  /* average expanded network node outdegree  */

/***********************************************/
/* the following macros define how neighboring */
/* nodes are connected to the expanded nodes   */
/***********************************************/
#define RP_LC 0 /* Random Placement Local Connection */
#define SP_LC 1 /* Strategic Placement Local Connection */

extern FILE *my_stderr;

/**********************************/
/* neighbor distribution function */
/**********************************/
distribute_neighbors(node_type *node, int node_num, node_type *rnp)
{
  int i, j;
  int *nn;
  int *nd_counter;
  int dist, min_dist, min_index, degree;
  node_type **tnp;

  nn = (int *)malloc(sizeof(int)*rnp->degree);
  if (!nn)
  {
    fprintf(stderr, "distribute_neighbors: no memory for nn (%d bytes)\n", sizeof(int)*rnp->degree);
    exit(-1);
  }

  nd_counter = (int *)malloc(sizeof(int)*(node_num+1));
  if (!nd_counter)
  {
    fprintf(stderr, "distribute_neighbors: no memory for nd_counter (%d bytes)\n", sizeof(int)*(node_num+1));
    exit(-1);
  }

  
  for (i=0; i<node_num; ++i) nd_counter[i] = 0;

  /*********************************************************/
  /* determine which neighbor goes to which expansion node */
  /*********************************************************/
  for (i=0; i<rnp->degree; ++i)
  {
    min_dist = euclidean(rnp->nnp[i], rnp);
    min_index = node_num;

    for (j=0; j<node_num; ++j)
    {
      dist = euclidean(rnp->nnp[i], &node[j]);
      if (dist < min_dist)
      {
        min_dist = dist;
        min_index = j;
      }
    }

    nn[i] = min_index;
    nd_counter[min_index]++;
  }

  /******************************/
  /* update the expansion nodes */
  /******************************/
  for (i=0; i<node_num; ++i)
  {
    tnp = (node_type **)malloc(sizeof(node_type *)*(node[i].degree+nd_counter[i]));
    if (!tnp)
    {
      fprintf(stderr, "distribute_neighbors: no memory for tnp (%d bytes)!\n", sizeof(node_type *)*(node[i].degree+nd_counter[i]));
      exit(-1);
    }

    for (j=0; j<node[i].degree; ++j)
      tnp[j] = node[i].nnp[j];
    
    for (; j<node[i].degree+nd_counter[i]; ++j)
      tnp[j] = 0;

    free(node[i].nnp);
    node[i].nnp = tnp;
  }
  /*****************/
  /* the root node */
  /*****************/
  tnp = (node_type **)malloc(sizeof(node_type *)*nd_counter[node_num]);
  if (!tnp)
  { 
    fprintf(stderr, "distribute_neighbors: no memory for tnp (%d bytes)!\n", sizeof(node_type *)*nd_counter[i]);
    exit(-1);
  } 


  min_index = 0;
  for (i=0; i<rnp->degree; ++i)
  {
    if (nn[i]<node_num)
    {
      degree = node[nn[i]].degree;
      node[nn[i]].nnp[degree] = rnp->nnp[i];
      ++node[nn[i]].degree;
    }
    else
      tnp[min_index++] = rnp->nnp[i];
  }

  if (min_index != nd_counter[node_num])
  {
    fprintf(stderr, "distribute_neighbors: fatal error! min_index=%d, nd_counter[%d]=%d!\n", min_index, node_num, nd_counter[node_num]);
    exit(-1);
  }

  free(rnp->nnp);
  rnp->nnp = tnp;
  rnp->degree = min_index;

  free(nn);
  free(nd_counter);
}

/*******************************/
/* the main expansion function */
/*******************************/
void expand_nodes(node_type *node, int node_num, int nodes2net, int grid_size, int expansion, int seed)
{ /* reminder: node should be sorted according to outdegree at all times */

  int i, j;
  int offset;

  /*********************************************/
  /* reinitializes the random number generator */
  /*********************************************/
  random_reset();

  for (i=0; i<nodes2net; ++i)
  {
    offset = node_num + i*NETWORK_SIZE;

    /******************************/
    /* place the additional nodes */  
    /******************************/
    if (expansion == RP_LC)
      random_placement(node+offset, NETWORK_SIZE, grid_size, seed);
    else
      strategic_placement(node+offset, NETWORK_SIZE, grid_size, seed);

    /*********************************************/
    /* reassign node ids for the expansion nodes */
    /*********************************************/
    for (j = offset; j<offset+NETWORK_SIZE; ++j)
    {
      node[j].nid = j;
      node[j].degree = 0; /* outdegree too */
    }

    /**************************************************************/
    /* form a connected network among the nodes randomly          */
    /* no definite pattern was obvious from observation           */
    /* http://boardwatch.internet.com/isp/summer99/backbones.html */
    /**************************************************************/
    random_connection(node+offset, NETWORK_SIZE, node[i], AVG_ENNOD, seed);

    /****************************************************/
    /* distribute neighboring nodes base on distance    */
    /****************************************************/
    distribute_neighbors(node+offset, NETWORK_SIZE, &node[i]); 
  }
}
