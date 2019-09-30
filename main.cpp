#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream> 
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

#include "graph_binary.h"
#include "community.h"

using namespace std;

char *filename = NULL;
char *filename_w = NULL;
char *filename_part = NULL;
int type       = UNWEIGHTED;
int nb_pass    = 0;
int display_level = -2;
int k1 = 16;
int time1 = 0;
int time2 = 0;
int order = 2;
int prune_level = 1;
bool verbose = false;
bool display = false;

void
usage(char *prog_name, const char *more) {
  cerr << more;
  cerr << "usage: " << prog_name << " input_file " << endl << endl;
  cerr << "input_file: file containing the graph to decompose in communities." << endl;
  exit(0);
}

void
parse_args(int argc, char **argv) {
  if (argc<2)
    usage(argv[0], "Bad arguments number\n");

  for (int i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      switch(argv[i][1]) {
      case 'v':
	verbose=true;
	break;
      case 'd':
	display=true;
	break;
      default:
	usage(argv[0], "Unknown option\n");
      }
    } else {
      if (filename==NULL)
        filename = argv[i];
      else
        usage(argv[0], "More than one filename\n");
    }
  }
}

int
main(int argc, char **argv) {
  parse_args(argc, argv);
  Community c(filename, filename_w, type, order, prune_level);
  c.one_level();

  if(display){
    cout << "Number of nodes: " << c.g.nb_nodes << endl;
    cout << "Number of edges: " << c.g.nb_links << endl;
    c.display_stats();
  }
  if(verbose){
    for(unsigned int i = 0; i < c.n2c.size(); i++){
      cout << i << "\t" << c.n2c[i] << endl;
    }
  }

}
