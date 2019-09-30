#include <sys/mman.h>
#include <fstream>
#include "graph_binary.h"
#include "math.h"

Graph::Graph() {
  nb_nodes     = 0;
  nb_links     = 0;
  total_weight = 0;
}

Graph::Graph(char *filename, char *filename_w, int type) {
  ifstream finput;
  finput.open(filename,fstream::in | fstream::binary);

  finput.read((char *)&nb_nodes, 4);

  degrees.resize(nb_nodes);
  finput.read((char *)&degrees[0], (long)nb_nodes*8);

  nb_links=degrees[nb_nodes-1];
  links.resize(nb_links);
  finput.read((char *)(&links[0]), (long)nb_links*4);  

  if(type==WEIGHTED){
    weights.resize(nb_links);
    finput.read((char *)(&weights[0]), (float)nb_links*4);
  }else{
    weights.resize(nb_links, 1);
  }

  boost::archive::binary_iarchive iarchive(finput);
  iarchive >> access;
  finput.close();

  total_weight=0;
  for (unsigned int i=0 ; i<nb_nodes ; i++) {
    total_weight += (double)weighted_degree(i);
  }

}
