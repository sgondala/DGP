#include "Window.cpp"
#include "helperFunctions.hpp"
#include <set>

//extern MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex);
//extern bool is_saddle(MeshVertex* v);
int main(){
  std::set<myWindow> pQueue;
  MeshVertex* src, *dst;
  
  // Get the first set of windows from the Mesh Vertex
  for(auto it = src->facesBegin(); it!= src->facesEnd(); it++){
    MeshEdge* e = getOtherEdge(*it, src);
    pQueue.insert(myWindow(src,e,*it));
  }

  // Dijkstra's algorithm starts  
  while(!pQueue.empty()){
    myWindow w = *pQueue.begin();
    pQueue.erase(pQueue.begin());
    
  }

}