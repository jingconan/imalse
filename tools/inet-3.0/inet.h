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
 * "@(#) $Id: inet.h,v 1.1.1.1 2002/05/09 14:08:20 jwinick Exp $ (UM)";
*/
typedef struct _node_type{
  int nid; /* node id */
  int degree; /* node degree */
  int free_degree; /* unconnected node degree */
  int x, y; /* x, y coordinate */
  struct _node_type **nnp;
} node_type;

void place_nodes(node_type *, int, int, int, int);
void generate_degrees(node_type *, int, int, int);
void generate_degrees1(node_type *, int, int, int);
void connect_nodes(node_type *, int, int);
void expand_nodes(node_type *, int, int, int, int, int);
void generate_output(node_type *, int);

void random_placement(node_type *, int, int, int);
void strategic_placement(node_type *, int, int, int);

int euclidean(node_type *, node_type *);
