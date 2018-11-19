#include "euler_path.h"

int check_graph(igraph_t* gpt, igraph_integer_t* spt,
		igraph_integer_t* tpt){
  igraph_vector_t indegree, outdegree;
  igraph_bool_t connected;
  int is_eulerian = 1;
  *spt = -1;
  *tpt = -1;

  igraph_vector_init(&indegree, 0);
  igraph_vector_init(&outdegree, 0);
  // if the graph has 0 node building an eulerian path is impossible
  if(igraph_vcount(gpt) == 1)
    is_eulerian = 0;
  igraph_is_connected(gpt, &connected, IGRAPH_WEAK);
  if(!connected)
    is_eulerian = 0;
  igraph_degree(gpt, &indegree, igraph_vss_all(),
		IGRAPH_IN, IGRAPH_LOOPS);
  igraph_degree(gpt, &outdegree, igraph_vss_all(),
		IGRAPH_OUT, IGRAPH_LOOPS);

  // checking for every node indegree and outdegree
  // i is basically the index of the nodes
  for(igraph_integer_t i = 0; i < igraph_vector_size(&indegree)
	&& is_eulerian; ++i){
    if(VECTOR(indegree)[i] == VECTOR(outdegree)[i])
      continue;
    // because of how eulerian and semi-eulerian graphes are defined
    // it is impossible to have one of them if a vertex of the graph
    // has the following indegree and outdegree
    if((VECTOR(indegree)[i] > VECTOR(outdegree)[i] + 1) ||
       (VECTOR(outdegree)[i] > VECTOR(indegree)[i] + 1))
      is_eulerian = 0;
    // find s and t
    if(VECTOR(indegree)[i] == VECTOR(outdegree)[i] + 1){
      if(*tpt == -1)
	*tpt = i;
      else
	is_eulerian = 0;
    }
    if(VECTOR(outdegree)[i] == VECTOR(indegree)[i] + 1){
      if(*spt == -1)
	*spt = i;
      else
	is_eulerian = 0;
    }
  }
  // if it is an eulerian graph s and t are irrelevant because the
  // cycle can start from any node of the graph
  if(*spt == -1 && *tpt == -1){
    *spt = 0;
    *tpt = 0;
  }
  // if there's only t or s it is an invalid graph by definition
  if(*spt == -1 || *tpt == -1)
    is_eulerian = 0;
  igraph_vector_destroy(&indegree);
  igraph_vector_destroy(&outdegree);
  return is_eulerian;
}

el_t* build_path(const igraph_integer_t* const spt,
		 igraph_stack_t** adjs){
  el_t* head = (el_t*) malloc(sizeof(el_t));
  head -> id = *spt;
  head -> next = NULL;
  igraph_integer_t current = *spt;
  while(!igraph_stack_empty(adjs[current])){ 
    current = igraph_stack_pop(adjs[current]);//pop the first adj
					      //element of the
                                              //current node
    // set the next node(the new current) as the first element of the
    // list 
    el_t* new_head = (el_t*) malloc(sizeof(el_t));
    new_head -> id = current;
    new_head -> next = head;
    head = new_head;
  }
  return head;
}

void build_cycle(igraph_stack_t** adjs, el_t* head,
		 igraph_vector_t* pathpt){

  igraph_integer_t current;
    
  while(head != NULL){
    // tanking the first element of the list, the last visited node
    current = head -> id;
    // if it still has adjacents that means an eulerian cycle can be
    // build from it, so the adjacent node which can be reached using
    // the edge found before is put as first element of the list, "we
    // move to the new node"
    if(!igraph_stack_empty(adjs[current])){
      el_t* new_head = (el_t*) malloc(sizeof(el_t));
      new_head -> id = igraph_stack_pop(adjs[current]);
      new_head -> next = head;
      head = new_head;
    }else{
      // build cycle is void so the elements must be destroyed here
      // beacause if they're destroyed in the main only the elements
      // composing the path will be, beacuse all elements are inserted
      // as the first element of the list pointed by head. In the main
      // head will still point only to the first path, so the list must
      // be destroyed here while elements are removed
      el_t* tmp = head;
      head = head -> next;
      igraph_vector_push_back(pathpt, current);
      free(tmp);
    }
  }
}

void print_path(FILE* fpt, igraph_vector_t* pathpt){
  // every 80 character \n is printed, we suppose that there aren't
  // more than 9999 although this print will print more than 80
  // charachters for row.
  int count = 0;
  for(igraph_integer_t i = igraph_vector_size(pathpt) - 1; i >= 0; --i){
    fprintf(fpt, "%-5d", (int)VECTOR(*pathpt)[i]);
    ++ count;
    if((count % 16) == 0)
      fprintf(fpt, "\n");
  }
  fprintf(fpt, "\n");
}
