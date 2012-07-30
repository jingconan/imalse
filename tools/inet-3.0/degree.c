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
 * "@(#) $Id: degree.c,v 1.2 2002/05/28 21:24:48 jwinick Exp $ (UM)";
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "inet.h"
#include "random.h"

extern FILE *my_stderr;

/***************************************************/
/* Constants used in generate degree distributions */
/***************************************************/

#define START_DEGREE 2 /* related to constant below */

/********************************************************************************/
/* complementary cumulative distribution function of degree frequency vs. degree*/
/*(Eq. 1 in the TR)   \bar{F}(d) = e^c * d^(a*t + b)                            */
/********************************************************************************/
 
#define a    .00324
#define b  -1.223
#define c   -.711

/***********************************/
/* degree vs. rank growth powerlaw */
/* d = exp(p*t + q) * r^R          */
/***********************************/
#define R       -.87  
#define p       .0228  
#define q       6.5285
#define DR_Frac .02 /* the fraction of nodes this law applies to */

/*************************************/
/* pairsize within h hops growth law */
/* P(t,h) = exp(s(h)*t)*P(0,h)       */
/*                                   */
/* therefore,                        */
/* P(t,0) = exp(s(0)*t)*P(0,0)       */
/* t is computed as:                 */
/*   log(P(t,0)/P(0,0))/s(0)         */
/*************************************/
#define P00     3037   /* P(0,0) */     
#define s0      .0281  /* s(0) */

/**** NOT BEING USED IN INET-3.0 ****************/
/************************************************/
/* frequency vs. degree growth powerlaw         */
/* f = C(t)*d^O                                 */
/* C(t) will be computed from Exponential Law 1 */
/************************************************/
#define O       -2.2  /* exponent computed using all nodes */
#define O_no1   -2.42 /* exponent without degree 1 nodes */


extern FILE *my_stderr;

/****************************************************************************/
/* comparison function for qsorting nodes in to descending order of degrees */
/****************************************************************************/
int degree_compare(const void *a1, const void *a2)
{
  node_type *n1 = (node_type *)a1;
  node_type *n2 = (node_type *)a2;

  if (n1->degree < n2->degree)
    return 1;
  else if (n1->degree == n2->degree)
    return 0;
  else
    return -1;
}

/***************************************************************/
/* generate the nodes degrees where the power-law coefficients */
/* are computed using nodes with degree larger than 1          */
/***************************************************************/
void generate_degrees(node_type *node, int node_num, int degree_one, int seed)
{
#define DEBUG_generate_degrees

  int i;
  int degree, nodes, edges = 0, F;
  float t, C_t;
  int node_num_no1, degree_two = 0, rank;
  float D_FRAC[19], base;

  float ccdfIdeal;
  float ccdfActual;
  float diff;
  int numNodesToAdd;

  float ccdfSlope;

  /* calculate what month  (how many months since nov. '97) */
  /*  corresponds to this topology of size node_num.       */                                                        
  t = log((float)node_num/(float)P00)/s0;

  /* fprintf(stderr,"MONTH = %f\n", t); */

  /* using t calculated above, compute the exponent of the ccdf power law */
  ccdfSlope = (a * t) + b;

  /********************************************/
  /* reinitialize the random number generator */
  /********************************************/
  random_reset();

  /***************************/
  /* generate degree 1 nodes */
  /***************************/
  node_num_no1 = node_num - degree_one;
  nodes = 0;
  for (i=node_num-1; i>=node_num_no1; --i)
  {
    node[i].nid = i;
    node[i].degree = 1;
    node[i].free_degree = 1;
  }
  
  nodes += degree_one;

  // for each degree see if we need to add any of it to obtain the appropriate value on the ccdf
  degree = 2;
  while (nodes <= node_num && degree < node_num)
  {
    // this is where we should be at
    ccdfIdeal = exp(c) * pow((float)degree,ccdfSlope);
    // this is were we are before adding nodes of this degree
    ccdfActual = (1.0 - ((float)nodes/(float)node_num));
    
    // calc how many nodes of this degree we need to add to get the ccdf right
    diff = ccdfActual - ccdfIdeal;
    //fprintf(stderr,"%f  %f  %f\n",ccdfIdeal,ccdfActual,diff);
    if (diff * (float)node_num > 1.0)   // we actually need to add a node at this degree
    {
      numNodesToAdd = (int)(diff * (float)node_num);
      for (i = node_num - nodes-1; i >= node_num - nodes - numNodesToAdd; --i)
      {
	//fprintf(stderr, "- %d  %d\n", degree,i);
	node[i].nid = i;
	node[i].degree = degree;
	node[i].free_degree = degree;

      }
      nodes += numNodesToAdd;
    }  
    ++degree;
  }
  
  /* use the degree-rank relationship to generate the top 3 degrees */
  for(i=0; i<3; ++i)
  {
    node[i].nid = i;
    rank = i + 1;
    node[i].degree = pow((float)(rank), R)*exp(q)*pow(((float)node_num/(float)P00), (p/s0));
    node[i].free_degree = node[i].degree;
    
    //fprintf(stderr,"* %d\n",node[i].degree);
  }

  qsort(node, node_num, sizeof(node_type), degree_compare);

#ifdef DEBUG_generate_degrees
  fprintf(my_stderr, "node[0].degree = %d\n", node[0].degree);
#endif /* DEBUG_generate_degrees */

  /************************************/
  /* the sum of node degrees can't be */
  /* odd -- edges come in pairs       */
  /************************************/
  for(i=0,edges=0;i<node_num;++i)
    edges += node[i].degree;
  if ( edges % 2 == 1 )
  {
     node[0].degree++;
     node[0].free_degree++;
  }

  for (i=0; i<node_num; ++i)
    node[i].nid = i;

} 

/**************************************************/
/* generate the nodes degrees where the power-law */
/* coefficients are computed using all nodes      */
/**************************************************/
void generate_degrees1(node_type *node, int node_num, int degree_one, int seed)
{
#define DEBUG_generate_degrees

  int i;
  int edges, percent;
  float F;

  percent = (int) ((float)node_num*DR_Frac);
  degree_one = 0;
  /********************************************/
  /* reinitialize the random number generator */
  /********************************************/
  random_reset();
 
  /***********************************/
  /* generate nodes of all degrees   */
  /***********************************/
  edges = 0;
  for (i=0; i<node_num; ++i)
  {
    F = random_uniform_real(seed, 0.0, .999999); 
    
    node[i].nid = i;
    node[i].degree = (int) pow( 1.0/(1.0 - F), (1.0/(-O-1)) );

    node[i].free_degree = node[i].degree;

    if (node[i].degree == 1)
      degree_one++;

    if (node[i].degree == 0 )
      --i; /* ignore degree 0 */
    else 
      edges += node[i].degree;
  }
  
  qsort(node, node_num, sizeof(node_type), degree_compare);

  /*********************************************/
  /* fix the top ``percent'' degrees according */
  /* to the degree vs. rank power law          */
  /*********************************************/
  for (i=0; i < percent; ++i)
  {
    edges -= node[i].degree;
    node[i].degree = pow((float)(i+1), R)*exp(q)*pow(((float)node_num/(float)P00), (p/s0));
    node[i].free_degree = node[i].degree;
    edges += node[i].degree;
  }

#ifdef DEBUG_generate_degrees
  fprintf(my_stderr, "node[0].degree = %d\n", node[0].degree);
#endif /* DEBUG_generate_degrees */

  /************************************/
  /* the sum of node degrees can't be */
  /* odd -- edges come in pairs       */
  /************************************/
  if ( edges % 2 == 1 )
  {
    node[0].degree++;
    node[0].free_degree++;
  }

/*#ifdef DEBUG_generate_degrees
  for (i=0; i<node_num; ++i)
    fprintf(my_stderr, "%d\t%d\t%d\n", node[i].nid, node[i].degree, node[i].free_degree);
#endif DEBUG_generate_degrees*/

#ifdef DEBUG_generate_degrees
  fprintf(stderr, "generate_degrees: degree_one = %d\n", degree_one);
#endif /* DEBUG_generate_degrees */
} 

