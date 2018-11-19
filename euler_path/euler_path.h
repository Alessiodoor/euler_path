#ifndef EULER_PATH_H
#define EULER_PATH_H

#include <igraph.h>
#include <stdio.h>
#include <stdlib.h>

//single linked element of a list which will be used to store the
//first path, the one calculated by build_path
typedef struct el{
  igraph_integer_t id;
  struct el* next;
}el_t;

// returns a boolean: true if the graph is feasable, false otherwise.
// the graph is feasable if it is eulerian or semi-eulerian.
int check_graph(igraph_t* gpt, igraph_integer_t* spt,
		igraph_integer_t* tpt);

// it builds the first path from s to t(by definition of the algorithm
// and of eulerian graph, the path will end in t in anycase) and it is
// stored reversely in a list whose pointer is the return value;
el_t* build_path(const igraph_integer_t* const spt,
		    igraph_stack_t** adjs);

// from the list built by build_path(), pointed by head, it will visit
// this trying to create new cycles, if possible, meanwhile stores the
// elements of the list in pathpt. At the end pathpt will contain all
// the eulerian path in reverse order
void build_cycle(igraph_stack_t** adjs, el_t* head,
		 igraph_vector_t* pathpt);

// it prints on the stream pointed by fpt the elements stored in
// pathpt from the last one to te first(in position order of the
// vector). 
void print_path(FILE* fpt, igraph_vector_t* pathpt);

#endif
