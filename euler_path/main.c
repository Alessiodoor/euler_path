#include "euler_path.h"
#include "cmdline.h"
#include <string.h>

int main(int argc, char* argv[]){
  // ggo
  static struct gengetopt_args_info ai;
  if (cmdline_parser (argc, argv, &ai) != 0) {
    exit(1);
  }
  printf("File name:%s\n", ai.filename_arg);
  printf("File type:%s\n", ai.filetype_arg);
	
  // declarations
  igraph_t g; // the graph
  igraph_integer_t nnodes; // number of nodes
  igraph_integer_t s, t; // start and end of the path
  int is_eulerian; // 1 if the graph is eulerian or semi-eulerian
  igraph_stack_t** adjs; // adjs list, array of stack, in adjs[i]
			 // there are the nodes which can be reached
			 // by and edge not already visited
  el_t* head; // head of the path(first node of the path)
  igraph_vector_t path; // final path, sequence of nodes, it will be
			// printed on a stream
  FILE* file;

  // IO from file
  file = fopen(ai.filename_arg, "r");
  if(!file){
    printf("File doesn't exist\n");
    return 2;
  }
  // check if the file format is not valid
  if((strcmp(ai.filetype_arg,"e") != 0) && (strcmp(ai.filetype_arg,"c") != 0) &&
     (strcmp(ai.filetype_arg,"l") != 0) && (strcmp(ai.filetype_arg,"d") != 0) &&
     (strcmp(ai.filetype_arg,"gdb") != 0) && (strcmp(ai.filetype_arg,"ml") != 0) &&
     (strcmp(ai.filetype_arg,"gml") != 0) && (strcmp(ai.filetype_arg,"p") != 0) &&
     (strcmp(ai.filetype_arg,"dl") != 0)){
    printf("File type is invalid\n");
    fclose(file);
    return 3;
  }

  if(strcmp(ai.filetype_arg,"e") == 0){//edge list format
    igraph_read_graph_edgelist(&g, file, 0, IGRAPH_DIRECTED);
    printf("Reading edgelist file format...\n");
  }
  if(strcmp(ai.filetype_arg,"c") == 0){//ncol format
    igraph_read_graph_ncol(&g, file, NULL, 0, IGRAPH_ADD_WEIGHTS_NO, IGRAPH_DIRECTED);
    printf("Reading ncol file format...\n");
  }
  if(strcmp(ai.filetype_arg,"l") == 0){//lgl format
    igraph_read_graph_lgl(&g, file, 0, IGRAPH_ADD_WEIGHTS_NO, IGRAPH_DIRECTED);
    printf("Reading lgl file format...\n");
  }
  if(strcmp(ai.filetype_arg,"d") == 0){//dimacs format
    igraph_read_graph_dimacs(&g, file, NULL, NULL, NULL, NULL, NULL, 1);
    printf("Reading dimacs file format...\n");
  }
  if(strcmp(ai.filetype_arg,"gdb") == 0){//graphdb format
    igraph_read_graph_graphdb(&g, file, IGRAPH_DIRECTED);
    printf("Reading graphdb file format...\n");
  }
  if(strcmp(ai.filetype_arg,"ml") == 0){//graphml format
    igraph_read_graph_graphml(&g, file, 0);
    printf("Reading graphml file format...\n");
  }
  if(strcmp(ai.filetype_arg,"gml") == 0){//gml format
    igraph_read_graph_gml(&g, file);
    printf("Reading gml file format...\n");
  }
  if(strcmp(ai.filetype_arg,"p") == 0){//pajeck format
    igraph_read_graph_pajek(&g, file);
    printf("Reading pajeck file format...\n");
  }
  if(strcmp(ai.filetype_arg,"dl") == 0){//dl format
    igraph_read_graph_dl(&g, file, IGRAPH_DIRECTED);
    printf("Reading dl file format...\n");
  }
  fclose(file);

  // check if the graph is eulerian or not
  is_eulerian = check_graph(&g, &s, &t);
  if(!is_eulerian){
    fprintf(stdout, "graph isn't eulerian or semi-eulerian\n");
    igraph_destroy(&g);
    return 1;
  }

  // initializing the adj of every node
  nnodes = igraph_vcount(&g);
  adjs = (igraph_stack_t**) malloc(nnodes * sizeof(igraph_stack_t*));

  // this double inserted for even tough doesn't look like a
  // O(|E|) actually it takes that time because it visits every edge
  // only once and operates on that once pushing the adjacent
  // vertex in the right stack.
  for(igraph_integer_t i = 0; i < nnodes; ++i){
    // initialize the stack for the i position
    adjs[i] = (igraph_stack_t*) malloc(sizeof(igraph_stack_t));
    igraph_stack_init(adjs[i], 0);
    igraph_vector_t e_adjs; // outcoming edges
    igraph_vector_init(&e_adjs, 0); 
    igraph_incident(&g, &e_adjs, i, IGRAPH_OUT);
    // save the outcoming incident nodes in the stack
    for(igraph_integer_t j = 0; j < igraph_vector_size(&e_adjs); ++j){
      igraph_integer_t from;
      igraph_integer_t to;
      igraph_edge(&g, VECTOR(e_adjs)[j], &from, &to);
      igraph_stack_push(adjs[i], to);
    }
    igraph_vector_destroy(&e_adjs);
  }
  
  // create first path that will be stored in a list pointed by head
  head = build_path(&s, adjs);
  printf("\n");	
  igraph_vector_init(&path, 0);
  build_cycle(adjs, head, &path);

  // cleaning memory
  for(igraph_integer_t i = 0; i < nnodes; ++i){
    igraph_stack_destroy(adjs[i]);
    free(adjs[i]);
  }
  free(adjs);
  igraph_destroy(&g);
  
  // output, using ggo option if it is present, otherwise the default 
  // output stream option in stdout
  if(strcmp(ai.outfile_arg, "stdout") == 0)
    print_path(stdout, &path); 
  else{
    file = fopen(ai.outfile_arg, "w");
    if (!file){
      printf("Error while opening the file!\n");
      igraph_vector_destroy(&path);
      return 4;
    }
    printf("Printing the eulerian path on the file...\n");
    print_path(file, &path);
    printf("Eulerian path has been printed\n");
    fclose(file);
  }
  
  igraph_vector_destroy(&path);
  
  return 0;
}
