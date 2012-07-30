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
 * "@(#) $Id: placement.c,v 1.1.1.1 2002/05/09 14:08:19 jwinick Exp $ (UM)";
*/
#include <stdio.h>
#include <stdlib.h>
#include "inet.h"
#include "random.h"

#define RANDOM_UNIFORM 0

/******************************************************************/
/* place additional nodes in regions where more nodes are located */
/* 1. divide the grid into node_num x node_num squares            */
/* 2. sort the squares based on the number of nodes (descending)  */
/* 3. pick squares starting with the most populated while         */
/*    ignore neighboring squares of already chosen squares        */
/******************************************************************/
void strategic_placement(node_type *node, int node_num, int grid_size, int seed){
}


/***************************************/
/* randomly placed nodes inside a grid */
/***************************************/
void random_placement(node_type *node, int node_num, int grid_size, int seed)
{
  int i;
 
  /*********************************************/
  /* reinitializes the random number generator */
  /*********************************************/
  random_reset();

  for (i=0; i<node_num; ++i)
  {
    node[i].x = random_uniform_int(seed, 0, grid_size);
    node[i].y = random_uniform_int(seed, 0, grid_size);
  }
}

/********************************/
/* main node placement function */
/********************************/
void place_nodes(node_type *node, int node_num, int grid_size, int placement, int seed)
{
  int i;

  /*********************************************/
  /* reinitializes the random number generator */
  /*********************************************/
  random_reset();

  /********************************************************************/
  /* First thing first, generating the (x,y) coordinate for each node */
  /********************************************************************/
  switch (placement)
  {
    case RANDOM_UNIFORM:
      random_placement(node, node_num, grid_size, seed);
      break;
    default:
      fprintf(stderr, "place_nodes: placement method %d not supported!\n", placement);
      exit(-1); 
  }

}
