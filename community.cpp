#include "community.h"

using namespace std;

Community::Community(char * filename, char * filename_w, int type, int order, int prune_level){
  g = Graph(filename, filename_w, type);
  size = g.nb_nodes;
  m = 1/g.total_weight;
  count = 1;

  neigh_weight.resize(size,-1);
  neigh_pos.resize(size);
  neigh_last=0;

  n2c.resize(size);
  in.resize(size);
  tot.resize(size);
  comms.resize(size);
  pr.resize(size, 0);

  for (int i=0 ; i<size ; ++i) {
    n2c[i] = i;
    tot[i] = g.weighted_degree(i);
    in[i]  = g.nb_selfloops(i);
    vector<unsigned int> v(1, i);
    comms[i] = v;
  }
  access_order = order;
  p_level = prune_level;

}

double
Community::modularity() {
  double q  = 0;
  double m2 = (double)g.total_weight;
  for (int i=0 ; i<size ; ++i) {
    if (tot[i]>0){
      q += (double)in[i]/m2 - ((double)tot[i]/m2)*((double)tot[i]/m2);
    }
  }
  return q;
}

unsigned int
Community::neigh_comm(unsigned int n){
  unsigned int comm = n2c[n];
  neigh_pos[0] = comm;
  neigh_weight[neigh_pos[0]] = 0;
  neigh_last = 1;
  pair<vector<unsigned int>::iterator, vector<float>::iterator> p;
  unsigned int cachable = 0;
  
  for(unsigned int j=0, end=comms[comm].size(); j<end; ++j){
    unsigned int node = comms[comm].at(j);
    p = g.neighbors(node);

    for(unsigned int i=0, deg=g.nb_neighbors(node); i<deg ; ++i){
      unsigned int neigh  = *(p.first+i);
      unsigned int neigh_comm = n2c[neigh];
      double neigh_w = *(p.second+i);
      
      if(comm!=neigh_comm){
	if(neigh_weight[neigh_comm]==-1){
	  neigh_weight[neigh_comm] = 0;
	  neigh_pos[neigh_last++] = neigh_comm;
	}else{
	  ++cachable;
	}
	neigh_weight[neigh_comm] += neigh_w;
      }
    }
  }

  return cachable;
}

void
Community::one_level(){
  const clock_t t1 = clock();
  vector<int>::iterator it;
  m2 = g.total_weight;
  inv_m2 = (double)(1/m2);
  inv_m2_pow = inv_m2 * inv_m2;
  _num = 0;

  clock_t s_agg, e_agg;
  double duration = 0;

  const unsigned int maxlen = g.access.size();
  unordered_map<uint32_t, double> cache;

  while(count<maxlen){
    unsigned int comm = n2c[g.access[maxlen-count]];
    ++count;

    if(g.nb_neighbors(comm) == 1){
      unsigned int best_comm = *((g.neighbors(comm)).first);
      aggregate(comm, best_comm, 1, 0);
    }else{
      int best_comm = comm;
      double best_increase = 0;
      double best_pruv = 0;
      double pruv = 0;
      double w = 0;
      double increase = 0;

      //log-likelihood computation
      unsigned int loop_end = 2;
      bool cache_flag = false;
      if( neigh_comm(comm)>0 ){
	loop_end = neigh_last;
      }else{
	cache_flag = true;
      }

      unsigned int cache2_last = 0;

      for(unsigned int i=1; i<loop_end; ++i){
	// motif-aware caching
	if(cache_flag){
	  uint32_t key;
	  if(tot[comm]<tot[neigh_pos[i]]){
	    key = (tot[comm]<<15)|tot[neigh_pos[i]];
	  }else{
	    key = (tot[neigh_pos[i]]<<15)|tot[comm];
	  }
	  if(cache.count(key)){
	    increase = cache.at(key);
	    //++_num;
	  }else{
	    increase = likelihood_ratio_gain2(comm, neigh_pos[i], neigh_weight[neigh_pos[i]], &pruv);
	    cache[key] = increase;
	  }
	}else{
	  increase = likelihood_ratio_gain2(comm, neigh_pos[i], neigh_weight[neigh_pos[i]], &pruv);
	}
	
	if(increase>best_increase){
	  best_comm = neigh_pos[i];
	  best_increase = increase;
	  best_pruv = pruv;
	  w = neigh_weight[best_comm];
	}
      }

      // initialization
      for(unsigned int i=0; i<neigh_last; ++i){
	neigh_weight[neigh_pos[i]] = -1;
      }
      neigh_last = 0;
      
      //incremental aggregation
      if(comm!=best_comm){
	aggregate(comm, best_comm, w, best_pruv);
      }
    } 

  }
  
  const clock_t t2 = clock();
  clustering_time =  (double)(t2-t1)/CLOCKS_PER_SEC;

}

void 
Community::display_stats(){
  // initialize
  get_inverse_n2c();
  list<unsigned int> clusters = list_clusters();
  
  unsigned int sum_edgecuts = 0;
  unsigned int max_num_adj_clusters = 0;
  unsigned int min_num_adj_clusters = UINT_MAX;
  double avg_num_adj_clusters = 0;
  double avg_num_nodes_cluster = 0;
  double sd_num_nodes_cluster = 0;
  double var_num_nodes_cluster = 0;
  double avg_num_inner_wedges = 0;
  double sd_num_inner_wedges = 0;
  double var_num_inner_wedges = 0;
  unsigned int sum_num_outer_wedges = 0;
  unsigned int max_num_outer_wedges = 0;
  unsigned int min_num_outer_wedges = UINT_MAX;
  unsigned int max_size_cluster = 0;
  double avg_num_outer_wedges = 0;

  list<unsigned int>::iterator it = clusters.begin(), end = clusters.end();
  while(it!=end){
    unsigned int cluster = *it;
    pair<unsigned int, unsigned int> p = num_adj_clsedge(cluster);
    double outer_wedge = sum_outer_wedges(cluster);
    
    sum_edgecuts += p.second;
    avg_num_adj_clusters += p.first;
    avg_num_nodes_cluster += num_nodes_cluster(cluster);
    avg_num_inner_wedges += sum_inner_wedges(cluster);
    sum_num_outer_wedges += outer_wedge;
    
    if(max_size_cluster < num_nodes_cluster(cluster)){
      max_size_cluster = num_nodes_cluster(cluster);
    }

    if(max_num_adj_clusters < p.first){
      max_num_adj_clusters = p.first;
    }

    if(min_num_adj_clusters > p.first){
      min_num_adj_clusters = p.first;
    }

    if(max_num_outer_wedges < outer_wedge){
      max_num_outer_wedges = outer_wedge;
    }

    if(min_num_outer_wedges > outer_wedge){
      min_num_outer_wedges = outer_wedge;
    }

    ++it;
  }

  unsigned int size = clusters.size();
  avg_num_adj_clusters = (double)avg_num_adj_clusters/(double)size;
  avg_num_nodes_cluster = (double)avg_num_nodes_cluster/(double)size;
  avg_num_inner_wedges = (double)avg_num_inner_wedges/(double)size;
  avg_num_outer_wedges = (double)sum_num_outer_wedges/(double)size;

  it = clusters.begin();
  // variance
  while(it!=end){
    double pow1 = (num_nodes_cluster(*it) - avg_num_nodes_cluster);
    double pow2 = (sum_inner_wedges(*it) - avg_num_inner_wedges);
    var_num_nodes_cluster += pow1*pow1;
    var_num_inner_wedges += pow2*pow2;
    ++it;
  }
  var_num_nodes_cluster = var_num_nodes_cluster/(double)size;
  sd_num_nodes_cluster = sqrt(var_num_nodes_cluster);
  sd_num_inner_wedges = sqrt(var_num_inner_wedges);

  cout << "Runtime for clustering (sec.):" << clustering_time << endl;
  cout << "Modularity Q:" << modularity() << endl;
  cout << "Number of clusters:" << size << endl; 
  cout << "Average cluster size:" << avg_num_nodes_cluster << endl;
  cout << "Largest cluster size:" << max_size_cluster << endl;
  
}
