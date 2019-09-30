#ifndef COMMUNITY_H
#define COMMUNITY_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <queue>
#include <ctime>
#include <unordered_map>
#include "graph_binary.h"

#define RANDOM 0
#define DESCEND 1
#define ASCEND 2

using namespace std;

class Community {
 public:
  vector<unsigned int> neigh_weight;
  vector<unsigned int> neigh_pos;
  unsigned int neigh_last, count;
  int access_order;
  unsigned int p_level;
  double m;
  double time0, time1, time2, time3;
  int a;
  double clustering_time;
  double m2;
  double inv_m2;
  double inv_m2_pow;
  unsigned int _num;

  Graph g;
  int size;
  vector<int> n2c;
  vector<vector<unsigned int> > inverse_n2c;
  vector<unsigned int> in, tot;
  vector<vector<unsigned int> > comms;
  vector<double> pr;

  Community (char *filename, char *filename_w, int type, int order, int prune_level);

  //clustering functions
  inline void aggregate(unsigned int comm, unsigned int best, double e, double best_pr);
  
  // for LRMod //
  inline double likelihood_ratio();
  inline double likelihood_ratio_cluster(double in, double tot);
  inline double likelihood_ratio_gain(unsigned int* u, unsigned int* v, double* e, double* src);
  inline double likelihood_ratio_gain2(unsigned int u, unsigned int v, double e, double* return_pruv);
  //////////////

  unsigned int neigh_comm(unsigned int node);
  double modularity();
  void one_level();
  void display_stats();

  //clustering statistics
  inline void get_inverse_n2c();
  inline unsigned int num_clusters();
  inline unsigned int num_nodes_cluster(unsigned int i);
  inline void display_cluster_contents();
  inline double sum_inner_wedges(unsigned int i);
  inline double sum_outer_wedges(unsigned int i);
  inline pair<unsigned int, unsigned int> num_adj_clsedge(unsigned int i);
  inline list<unsigned int> list_clusters();
};

inline void
Community::aggregate(unsigned int comm, unsigned int best, double e, double best_pr){
  int index = comms[best].size();
  comms[best].insert(comms[best].end(), comms[comm].begin(), comms[comm].end());

  for(unsigned int i=index; i<comms[best].size(); ++i){
    n2c[comms[best].at(i)] = n2c[best];
  }

  in[best] += 2*e + in[comm];
  tot[best] += tot[comm];
  pr[best] = best_pr;
  in[comm] = 0;
  tot[comm] = 0;
  pr[comm] = 0;

}

////////////////////////
// LRMod              //
////////////////////////
// get total log likelihood ratio from all clusters
inline double
Community::likelihood_ratio(){
  double lr = 0.0;
  for(int i=0; i<size; i++){
    if(tot[i]>0){
      lr += likelihood_ratio_cluster(in[i], tot[i]);
    }
  }
  return lr;
}

// get log likelihood ratio for a cluster
inline double
Community::likelihood_ratio_cluster(double in, double tot){
  if(in == 0 || in == m2){
    return 0.0;
  }else{
    double tp = in*inv_m2;
    double ep = tot*inv_m2*tot*inv_m2;
    return in*(log(tp)-log(ep)) + (m2-in)*(log(1-tp)-log(1-ep));
  }
}

// [Old version] get log likelihood ratio gain between clusters
inline double
Community::likelihood_ratio_gain(unsigned int* u, unsigned int* v, double* e, double* LR_comm){
  return likelihood_ratio_cluster((in[*u]+in[*v]+2*(*e)), (tot[*u]+tot[*v]))-*LR_comm - likelihood_ratio_cluster(in[*v], tot[*v]);
}

// get log likelihood ratio gain between clusters
inline double
Community::likelihood_ratio_gain2(unsigned int u, unsigned int v, double e, double* return_pruv){
  double deltaQ = 2*((e*inv_m2) - (tot[u]*tot[v])*inv_m2_pow);
  if(deltaQ<0){
    return -1;
  }
  
  // Pruv
  double tpuv = (in[u]+in[v]+2*e)*inv_m2;
  double totuv = tot[u]+tot[v];
  *return_pruv = tpuv * log(( tpuv/(totuv*totuv*inv_m2_pow))); 

  return *return_pruv - pr[u] - pr[v] - deltaQ;
}


///////////
// stats //
///////////

// get inversed n2c vector
inline void
Community::get_inverse_n2c(){
  inverse_n2c.resize(0, vector<unsigned int>(0));

  for(unsigned int i=0, end=n2c.size(); i<end; ++i){
    unsigned int cluster_id = n2c[i];
    
    if(inverse_n2c.size()<=cluster_id){
      inverse_n2c.resize(cluster_id+1, vector<unsigned int>(0));
    }
    inverse_n2c[cluster_id].emplace_back(i);
  }

  // initialize
  neigh_last=0;
  neigh_weight.clear();
  neigh_weight.resize(size, -1);

}

// count number of clusters
inline unsigned int 
Community::num_clusters(){
  unsigned int count = 0;
  for(unsigned int i=0, end=inverse_n2c.size(); i<end; ++i){
    if(inverse_n2c[i].size()>0)
      count++;
  }
  return count;
}

// count the sum of weighted edges accross two clusters
inline double 
Community::sum_outer_wedges(unsigned int i){
  return tot[i]-in[i];
}

// count the number of nodes for a cluster
inline unsigned int
Community::num_nodes_cluster(unsigned int i){
  return inverse_n2c[i].size();
}

// count the sum of weighted inner edges
inline double 
Community::sum_inner_wedges(unsigned int i){
  return in[i];
}

// display cluster mapping
inline void
Community::display_cluster_contents(){
  for(unsigned int i=0, end=inverse_n2c.size(); i<end; ++i){
    if(inverse_n2c[i].size()>0){
      cout << "Cluster " << i << " " << endl;
      for(unsigned int j=0, endj=inverse_n2c[i].size(); j<endj; ++j){
	cout << inverse_n2c[i][j] << " ";
      }
      cout << endl;
      cout << "Size " << inverse_n2c[i].size() << endl;
    }
  }
}

// get number of adj.clusters and edgecuts
inline pair<unsigned int, unsigned int>
Community::num_adj_clsedge(unsigned int i){
  pair<unsigned int, unsigned int> p;
  unsigned int num_edgecut = neigh_comm(i);

  for(unsigned int i=0; i<neigh_last; ++i){
    neigh_weight[neigh_pos[i]] = -1;
  }

  p.first = neigh_last-1; //num. of adj. clusters
  p.second = num_edgecut; //num. of edgecuts
  neigh_last=0;
  return p;
}

inline list<unsigned int>
Community::list_clusters(){
  list<unsigned int> list; 
  for(unsigned int i=0, end=inverse_n2c.size(); i<end; ++i){
    if(inverse_n2c[i].size()>0){
      list.emplace_back(i);
    }
  } 
  return list;
}

#endif // COMMUNITY_H
