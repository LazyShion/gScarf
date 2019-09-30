#include "graph.h"

using namespace std;

Graph::Graph(char *filename, int type){
  ifstream finput;
  finput.open(filename,fstream::in);
  
  int nb_links=0;

  while (!finput.eof()){
    unsigned int src, dest;
    float weight=1.;

    if (type==WEIGHTED){
      finput >> src >> dest >> weight;
    }else{
      finput >> src >> dest;
    }
    
    if(finput){
      if(links.size()<=max(src,dest)+1){
        links.resize(max(src,dest)+1);
      }
      
      links[src].push_back(make_pair(dest,weight));
      if(src!=dest){
        links[dest].push_back(make_pair(src,weight));
      }
      nb_links++;
    }
  }

  finput.close();
}

void
Graph::renumber(int type){
  vector<int> linked(links.size(),-1);
  vector<int> renum(links.size(),-1);
  int nb=0;
  
  for(unsigned int i=0; i<links.size(); i++){
    for(unsigned int j=0; j<links[i].size(); j++){
      linked[i]=1;
      linked[links[i][j].first]=1;
    }
  }
  
  for(unsigned int i=0; i<links.size(); i++){
    if (linked[i]==1)
      renum[i]=nb++;
  }

  for(unsigned int i=0; i<links.size(); i++){
    if(linked[i]==1){
      for(unsigned int j=0; j<links[i].size(); j++){
	links[i][j].first = renum[links[i][j].first];
      }
      links[renum[i]]=links[i];
    }
  }
  links.resize(nb);
}

void
Graph::clean(int type){
  for(unsigned int i=0; i<links.size(); i++){
    map<int, float> m;
    map<int, float>::iterator it;

    for(unsigned int j=0; j<links[i].size(); j++){
      it = m.find(links[i][j].first);
      if(it==m.end())
	m.insert(make_pair(links[i][j].first, links[i][j].second));
      else if(type==WEIGHTED)
      	it->second+=links[i][j].second;
    }
    
    vector<pair<int,float> > v;
    for(it = m.begin(); it!=m.end(); it++)
      v.push_back(*it);
    links[i].clear();
    links[i]=v;
  }
}

void
Graph::display(int type){
  for(unsigned int i=0; i<links.size(); i++){
    for(unsigned int j=0; j<links[i].size(); j++){
      int dest   = links[i][j].first;
      float weight = links[i][j].second;
      if(type==WEIGHTED)
	cout << i << " " << dest << " " << weight << endl;
      else
	cout << i << " " << dest << endl;
    }
  }
}

void
Graph::display_binary(char *filename, char *filename_w, int type){
  ofstream foutput;
  foutput.open(filename, fstream::out | fstream::binary);

  unsigned int s = links.size();

  // outputs number of nodes
  foutput.write((char *)(&s),4);

  // outputs cumulative degree sequence
  long tot=0;
  int deg=0;
  for (unsigned int i=0 ; i<s ; i++){
    deg = (int)links[i].size();
    tot+=(long)deg;
    sort.insert(make_pair(deg, i));
    foutput.write((char *)(&tot),8);
  }

  // outputs links and make access vector
  multimap<float, int>::iterator it=sort.begin(), end=sort.end();
  for(unsigned int i=0 ; i<s ; i++){
    access.push_back(it->second);
    ++it;

    multimap<int, int> node; 
    for(unsigned int j=0; j<links[i].size(); j++){
      int dest_id = links[i][j].first;
      node.insert(make_pair(links[dest_id].size(), dest_id));
    }

    for(multimap<int, int>::iterator it=node.begin(), end=node.end(); it!=end; ++it){
      int dest = (*it).second;
      foutput.write((char *)(&dest), 4);
    }

  }

  // outputs the weight for each edge
  if(type==WEIGHTED){
    for(unsigned int i=0; i<s; i++){
      for(unsigned int j=0; j<links[i].size(); j++){
	float weight = links[i][j].second;
	foutput.write((char *)(&weight), 4);
      }
    }
  }

  // outputs access vector maps
  boost::archive::binary_oarchive oarchive(foutput);
  oarchive << access;
  foutput.close();
}
