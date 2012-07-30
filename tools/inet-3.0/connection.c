/*
 * Copyright (c) 1999, 2000, 2002  University of Michigan, Ann Arbor.
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
 * Author: Jared Winick (jwinick@eecs.umich.edu)
 *         Cheng Jin (chengjin@eecs.umich.edu)
 *         Qian Chen (qianc@eecs.umich.edu)
 *
 * "@(#) $Id: connection.c,v 1.1.1.1 2002/05/09 14:08:20 jwinick Exp $ (UM)";
*/
#include <stdio.h>
#include <stdlib.h>
#include "inet.h"
#include "random.h"
#include <math.h>

extern FILE *my_stderr;

/*********************************************/
/* build a connected network with an average */
/* node outdegree of avg_degree randomly     */
/*********************************************/
random_connection(node_type *node, int node_num, node_type *n2ep, int avg_degree, int seed)
{
  int i, j;
  int *G, *L, G_num, L_num, g, l, links;

  /*********************************************/
  /* reinitializes the random number generator */
  /*********************************************/
  random_reset();

  /***********************************************/
  /* allocate the nnp array of size ``node_num'' */
  /***********************************************/
  for (i=0; i<node_num; ++i)
  {
    node[i].nnp = (node_type **)malloc(sizeof(node_type *)*node_num);
    if (!node[i].nnp)
    {
      fprintf(stderr, "random_connection: no memory for node[%d].nnp (%d bytes)!\n", i, sizeof(node_type *)*node_num);
      exit(-1);
    }
   
    for (j=0; j<node_num; ++j)
      node[i].nnp[j] = 0;
  }  

  L = (int *)malloc(sizeof(int)*node_num);
  if (!L)
  {
    fprintf(stderr, "random_connection: no memory for L (%d bytes)!\n", sizeof(int)*node_num);
    exit(-1);
  }

  G = (int *)malloc(sizeof(int)*node_num);
  if (!G)
  {
    fprintf(stderr, "random_connection: no memory for G (%d bytes)!\n", sizeof(int)*node_num);
    exit(-1);
  }

  G[0] = 0;
  G_num = 1;
  L_num = node_num - 1;
  for (i=1; i<node_num; ++i)
    L[i-1] = i;

  /************************************************************/
  /* chain the expanded nodes together to ensure connectivity */
  /************************************************************/
  for (i=1; i<node_num; ++i)
  {
    g = random_uniform_int(seed, 0, G_num-1);  
     
    node[G[g]].nnp[node[G[g]].degree] = &node[L[0]];
    ++(node[G[g]].degree);

    node[L[0]].nnp[node[L[0]].degree] = &node[G[g]];
    ++(node[L[0]].degree);
    
    for (j=0; j<L_num-1; ++j)
      L[j] = L[j+1];

    ++G_num;
    --L_num;
  }
  
  /***********************************************/
  /* chain the node to be expanded with the rest */
  /***********************************************/
  g = random_uniform_int(seed, 0, G_num-1);

  n2ep->nnp[n2ep->degree] = &node[g];
  ++(n2ep->degree);
 
  node[g].nnp[node[g].degree] = n2ep;
  ++(node[g].degree);

  
  /***************************************/
  /* fill the remaining degrees randomly */
  /***************************************/
  links = node_num * avg_degree / 2 - (node_num - 1);
  i = 0;

  while (i<links)
  {
    g = random_uniform_int(seed, 0, node_num-1);
    l = random_uniform_int(seed, 0, node_num-1);

    if (g == l)
      continue;

    if ( g!=node_num && l!=node_num)
    { 
      for(j=0; j<node[g].degree; ++j)
        if (&node[l] == node[g].nnp[j])
          break;
      
      if ( j == node[g].degree )
      {
        node[g].nnp[node[g].degree] = &node[l];
        ++(node[g].degree);

        node[l].nnp[node[l].degree] = &node[g];
        ++(node[l].degree);

        ++i;
      }
    }
    else if ( g==node_num)
    {
      for(j=0; j<node[l].degree; ++j)
        if (n2ep = node[l].nnp[j])
          break;

      if ( j == node[l].degree )
      {
        n2ep->nnp[n2ep->degree] = &node[l];
        ++(n2ep->degree);
      
        node[l].nnp[node[l].degree] = n2ep;
        ++(node[l].degree);
      
        ++i;
      } 
    }
    else if ( l==node_num)
    {
      for(j=0; j<node[g].degree; ++j) 
        if (n2ep = node[g].nnp[j])          
          break;
        
      if ( j == node[g].degree )      
      {
        n2ep->nnp[n2ep->degree] = &node[g];    
        ++(n2ep->degree);  

        node[g].nnp[node[g].degree] = n2ep;   
        ++(node[g].degree);

        ++i;
      }
    } 
  }

  free(G);
  free(L);
}

/***************************************************************************/
/* function that tests the connectability of the given degree distribution */
/***************************************************************************/
int is_graph_connectable(node_type *node, int node_num)
{
  int i;
  int F_star, F = 0, degree_one = 0;

  for (i=0; i<node_num; ++i)
  {
    if (node[i].degree == 1)
      ++degree_one;
    else
      F += node[i].degree;
  }  

  //fprintf(my_stderr, "is_graph_connectable: F = %d, degree_one = %d\n", F, degree_one);

  F_star = F - 2*(node_num - degree_one - 1);
  if (F_star < degree_one)
    return 0;

  return 1;
}    
  
/*************************************/
/* the main node connection function */
/*************************************/
void connect_nodes(node_type *node, int node_num, int seed)
{
  int i, j, k, degree_g2;
  int *G, *L, G_num, L_num, g, l;
  int *p_array, p_array_num, *id, *flag;

  int added_node;
  int **weighted_degree_array;
  double distance;
  int *freqArray;

  /************************/
  /* satisfaction testing */
  /************************/
  if (!is_graph_connectable(node, node_num))
  {
    fprintf(my_stderr, "Warning: not possible to generate a connected graph with this degree distribution!\n");
  }

  p_array_num = 0;
  degree_g2 = 0;
  for (i=0; i<node_num; ++i)
  {
    node[i].nnp = (node_type **)malloc(sizeof(node_type *)*node[i].degree);
    if (!node[i].nnp)
    {
      fprintf(stderr, "connect_nodes: no memory for node[%d].nnp (%d bytes)!\n", i, sizeof(node_type *)*node[i].degree);
      exit(-1);
    }
    for (j=0; j<node[i].degree; ++j)
      node[i].nnp[j] = NULL;

    /* the probability array needs be of size = the sum of all degrees of nodes that have degrees >= 2 */
    if (node[i].degree > 1)
      p_array_num += node[i].degree;


    /* set the position of the first node of degree 1 */
    if (node[i].degree == 1 && node[i-1].degree != 1)
      degree_g2 = i;
  }
  
  //fprintf(my_stderr, "connect_nodes: degree_g2 = %d\n", degree_g2);

  G = (int *)malloc(sizeof(int)*degree_g2);
  if (!G)
  {
    fprintf(stderr, "connect_nodes: no memory for G (%d bytes)!\n", sizeof(int)*degree_g2);
    exit(-1);
  }

  L = (int *)malloc(sizeof(int)*degree_g2);
  if (!L)
  {
    fprintf(stderr, "connect_nodes: no memory for L (%d bytes)!\n", sizeof(int)*degree_g2);
    exit(-1);
  }

  //fprintf(my_stderr, "connect_nodes: allocating %d element array for p_array.\n", p_array_num);

  /* we need to allocate more memory than just p_array_num because of our added weights       */
  /* 40 is an arbitrary number, probably much higher than it needs to be, but just being safe */
  p_array = (int *)malloc(sizeof(int)*p_array_num*40);
  if (!p_array)
  {
    fprintf(stderr, "connect_nodes: no memory for p_array (%d bytes)!\n", (sizeof(int)*p_array_num));
    exit (-1);
  }

  id = (int *)malloc(sizeof(int)*degree_g2);
  if (!id)
  {
    fprintf(stderr, "connect_nodes: no memory for id (%d bytes)!\n", sizeof(int)*node_num);
    exit(-1);
  }

  flag = (int *)malloc(sizeof(int)*degree_g2);
  if (!flag)
  {
    fprintf(stderr, "no memory for flag (%d bytes)!\n", sizeof(int)*degree_g2);
    exit(-1);
  }

  /* weighted_degree_array is a matrix corresponding to the weight that we multiply  */
  /* the standard proportional probability by. so weighted_degree_array[i][j] is the */
  /* value of the weight in P(i,j). the matrix is topDegree x topDegree in size,     */
  /* where topDegree is just the degree of the node withh the highest outdegree.     */
  weighted_degree_array = (int**)malloc(sizeof(int*) * node[0].degree + 1);
  for (i = 0; i <= node[0].degree; ++i)
  {
    weighted_degree_array[i] = (int*)malloc(sizeof(int) * node[0].degree + 1);
    if (!weighted_degree_array[i])
    {
      fprintf(stderr, "no memory for weighted_degree index %d!\n", i);
      exit(-1);
    }
  }

  /* determine how many nodes of a given degree there are.  */
  freqArray = (int*)malloc(sizeof(int) * node[0].degree +1);

  // fill the freq array
  for (i = 0; i <= node[0].degree; ++i)
    freqArray[i] = 0;

  for (i = 0; i < node_num; ++i)
    freqArray[node[i].degree]++;
      

  /* using the frequency-degree relationship, calculate weighted_degree_array[i][j] for a all i,j */
  for (i = 1; i <= node[0].degree; ++i)
  {
    for (j = 1; j <=  node[0].degree; ++j)
    {
      if (freqArray[i] && freqArray[j]) // if these degrees are present in the graph
      {
	distance = pow(( pow( log((double)i/(double)j), 2.0) + pow( log((double)freqArray[i]/(double)freqArray[j]), 2.0)), .5);
	if (distance < 1)
	  distance = 1;
	
	weighted_degree_array[i][j] = distance/2 * j;
      }
    }
  }

 /********************************/
  /* randomize the order of nodes */
  /********************************/
  for (i=0; i<degree_g2; ++i)
    id[i] = i;

  i = degree_g2;
  while (i>0)
  {
    j = random_uniform_int(seed, 0, i-1); /* j is the index to the id array! */
    L[degree_g2 - i] = id[j];
 
    for (; j<i-1; ++j)
      id[j] = id[j+1];
    --i;
  }
      
  /* do not randomize the order of nodes as was done in Inet-2.2. */
  /* we just want to build the spanning tree by adding nodes in   */
  /* in monotonically decreasing order of node degree             */	
  for(i=0;i<degree_g2;++i)
    L[i] = i; 

  /************************************************/
  /* Phase 1: building a connected spanning graph */
  /************************************************/
  G_num = 1;
  G[0] = L[0];
  L_num = degree_g2 - 1;
 
  while (L_num > 0)
  {   
     j = L[1]; 
     added_node = j;

    /******************************/
    /* fill the probability array */
    /******************************/
    l = 0;
    for (i=0; i<G_num; ++i)
    {
      if (node[G[i]].free_degree)
      {
        if (node[G[i]].free_degree > node[G[i]].degree)
        {
          fprintf(my_stderr, "connect_nodes: problem, node %d(nid=%d), free_degree = %d, degree = %d, exiting...\n", G[i], node[G[i]].nid, node[G[i]].free_degree, node[G[i]].degree);
          exit(-1);
        }
	
	for(j=0; j < weighted_degree_array[ node[added_node].degree ][ node[G[i]].degree ]; ++j, ++l)
	  p_array[l] = G[i];
      }
    }
   
    /**************************************************************/
    /* select a node randomly according to its degree proportions */
    /**************************************************************/
    g = random_uniform_int(seed, 0, l-1); /* g is the index to the p_array */

    /*****************************************************/
    /* redirect -- i and j are indices to the node array */
    /*****************************************************/
    i = p_array[g];
    j = added_node;

    /*if (node[i].nid == 0)
      fprintf(stderr, "phase I: added node %d\n", node[j].nid);*/

    node[i].nnp[node[i].degree - node[i].free_degree] = node+j;
    node[j].nnp[node[j].degree - node[j].free_degree] = node+i;

    /* add l to G and remove from L */
    G[G_num] = j;
    for (l=1; l < L_num; ++l)
      L[l] = L[l+1];
  
    --(node[i].free_degree);
    --(node[j].free_degree);
    ++G_num;
    --L_num;
  }

  // fprintf(stderr, "DONE!!\n");
  /*************************************************************************/
  /* Phase II: connect degree 1 nodes proportionally to the spanning graph */
  /*************************************************************************/
  for (i=degree_g2; i<node_num; ++i)
  {
    /******************************/
    /* fill the probability array */
    /******************************/
    l = 0;
    for (j=0; j<degree_g2; ++j)
    {
      if (node[j].free_degree)
      {
	for (k = 0; k < weighted_degree_array[ 1 ][ node[j].degree ]; ++k, ++l)
          p_array[l] = j;
      }
    } 

    g = random_uniform_int(seed, 0, l-1); /* g is the index to the p_array */
    j = p_array[g];

    node[i].nnp[node[i].degree - node[i].free_degree] = node+j;
    node[j].nnp[node[j].degree - node[j].free_degree] = node+i;

    --(node[i].free_degree);
    --(node[j].free_degree);
  }

  // fprintf(stderr, "DONE!!\n");
  /**********************************************************/
  /* Phase III: garbage collection to fill all free degrees */
  /**********************************************************/
  for (i=0; i<degree_g2; ++i)
  {
    for (j=i+1; j<degree_g2; ++j)
      flag[j] = 1;
    flag[i] = 0; /* no connection to itself */
   
    /********************************************************************/
    /* well, we also must eliminate all nodes that i is already         */
    /* connected to. bug reported by shi.zhou@elec.qmul.ac.uk on 8/6/01 */
    /********************************************************************/
    for (j = 0; j < (node[i].degree - node[i].free_degree); ++j)
      if ( node[i].nnp[j] - node < degree_g2 )
        flag[ node[i].nnp[j] - node ] = 0;

    if ( !node[i].nnp[0] )
    {
      fprintf(my_stderr, "i = %d, degree = %d, free_degree = %d, node[i].npp[0] null!\n", i, node[i].degree, node[i].free_degree );
      exit(-1);
    }

    flag[node[i].nnp[0] - node] = 0; /* no connection to its first neighbor */

    if (node[i].free_degree < 0)
    {
      fprintf(my_stderr, "we have a problem, node %d free_degree %d!\n", i, node[i].free_degree);
      exit(-1);
    }
    
    while (node[i].free_degree)
    {      
      /******************************/
      /* fill the probability array */
      /******************************/
      l = 0;
      for (j=i+1; j<degree_g2; ++j)
      {
        if (node[j].free_degree && flag[j])
        {
	  for (k = 0; k < weighted_degree_array[ node[i].degree ][ node[j].degree ]; ++k, ++l)
	    p_array[l] = j;
        }
      }

      if (l == 0)
        break;

      g = random_uniform_int(seed, 0, l-1); /* g is the index to the p_array */
      j = p_array[g];

      /*if ( node[i].nid == 0 )
        fprintf(stderr, "phase III: added node %d!\n", node[j].nid);*/

      node[i].nnp[node[i].degree - node[i].free_degree] = node+j;
      node[j].nnp[node[j].degree - node[j].free_degree] = node+i;

      --(node[i].free_degree);
      --(node[j].free_degree);
  
      flag[j] = 0;
    }

    if (node[i].free_degree)
    {
      fprintf(my_stderr, "connect_nodes: node %d has degree of %d with %d unfilled!\n", node[i].nid, node[i].degree, node[i].free_degree);
    }
  }
}
