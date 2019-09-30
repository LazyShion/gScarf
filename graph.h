#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>

//boost
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>

#define WEIGHTED   0
#define UNWEIGHTED 1

using namespace std;

class Graph {
 public:
  vector<vector<pair<int,float> > > links;
  map<int, vector<int> >::iterator it, end;
  unsigned int nb_nodes;
  unsigned long nb_links;
  vector<unsigned long> degrees;  
  vector<int> access;
  multimap<float, int> sort;

  Graph (char *filename, int type);
  
  void clean(int type);
  void renumber(int type);
  void display(int type);
  void display_binary(char *filename, char *filename_w, int type);

};

#endif // GRAPH_H
