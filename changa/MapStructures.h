#include <iostream>

#include "Vector3D.h"
#include "TaggedVector3D.h"
using namespace std;

template <class T>
class Volume{
	
  public:
  T lesser[3];          // lesser corner
  T greater[3];         // greater corner 
  
  void halve(int whichAxis, Volume<T> &secondhalf);
  void split(int whichAxis, T *where, Volume<T> &secondhalf);
  int decideSplitAxis();

  Volume(){}
  Volume(T *l, T *g){
    for (int i= 0; i < 3; i++){
     lesser[i] = l[i];
     greater[i] = g[i];
    }
  }
};

class Cluster{
  public:
  int load;     // total load induced by this cluster
  int proc;     // which processor this cluster will be assigned to - one cluster per processor
  Volume <int>* partition;      // the lowest level process partition that this cluster belongs to
  float centroid[3];    // cluster centroid - calculated like center of mass, with unit mass for each participating TP. FIXME - refine
  int count;    // this cluster comprises 'count' tree pieces

  CkVec<int> tps;   // to check metis efficacy only. remove later on FIXME

  Cluster& operator= (Cluster& in){
    load = in.load;
    proc = in.proc;
    partition = in.partition;
    count = in.count;
    for(int i=0; i < 2; i++)
      centroid[i] = in.centroid[i];
    return *this;
  }

  Cluster(){
    for(int i = 0; i < 3; i++)
      centroid[i] = 0.0;
      count = 0;
      proc = -1;
  }
};

class ClusterSection{   // indicates which section of the clusterArray is to be worked upon
  public:
  int beg, end;
  ClusterSection(int b, int e): beg(b), end(e){}
};

template <class T>
int Volume<T>::decideSplitAxis(){

  int which = 0;
  T max = abs(greater[0] - lesser[0]);
  
  for(int i = 1; i < 3; i++)
    if(abs(greater[i] - lesser[i]) >= max){
      max = abs(greater[i] - lesser[i]);
      which = i;
    }
  return which;
}

template <class T>
void Volume<T>::halve(int axis, Volume<T> &secondhalf){

  secondhalf = *this;
  greater[axis] = (lesser[axis]+greater[axis])/2;
  secondhalf.lesser[axis] = greater[axis]+1;
}

template <class T>
void Volume<T>::split(int axis, T* where, Volume<T> &secondhalf){
  secondhalf = *this;
  greater[axis] = where[axis];
  secondhalf.lesser[axis] = where[axis];
}

