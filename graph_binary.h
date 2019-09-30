#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_iarchive.hpp>

#define WEIGHTED   0
#define UNWEIGHTED 1

using namespace std;

class Graph {
 public:
  unsigned int nb_nodes;
  unsigned long nb_links;
  double total_weight;  
  unsigned int max_deg;

  vector<unsigned long> degrees;
  vector<unsigned int> links;
  vector<float> weights;
  vector<int> access;

  Graph();
  Graph(char *filename, char *filename_w, int type);

  inline unsigned int nb_neighbors(unsigned int node);
  inline double nb_selfloops(unsigned int node);
  inline double weighted_degree(unsigned int node);
  inline pair<vector<unsigned int>::iterator, vector<float>::iterator > neighbors(unsigned int node);
};

inline unsigned int
Graph::nb_neighbors(unsigned int node){
  if (node==0){
    return degrees[0];
  }else{
    return degrees[node]-degrees[node-1];
  }
}

inline double
Graph::nb_selfloops(unsigned int node){
  pair<vector<unsigned int>::iterator, vector<float>::iterator > p = neighbors(node);
  for(unsigned int i=0, end=nb_neighbors(node); i<end; ++i){
    if(*(p.first+i)==node){
	return (double)*(p.second+i);
    }
  }
  return 0.;
}

inline double
Graph::weighted_degree(unsigned int node){
  if(weights.size()==0){
    return (double)nb_neighbors(node);
  }else{
    pair<vector<unsigned int>::iterator, vector<float>::iterator> p = neighbors(node);
    double res = 0;
    for (unsigned int i=0, end=nb_neighbors(node); i<end; ++i){
      res += (double)*(p.second+i);
    }
    return res;
  }
}

inline pair<vector<unsigned int>::iterator, vector<float>::iterator>
  Graph::neighbors(unsigned int node){

  if(node==0)
    return make_pair(links.begin(), weights.begin());
  else
    return make_pair(links.begin()+degrees[node-1], weights.begin()+degrees[node-1]);
}

#endif // GRAPH_H
