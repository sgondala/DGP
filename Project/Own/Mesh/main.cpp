#include "Window.cpp"
#include "helperFunctions.hpp"
#include <set>


int
usage(int argc, char * argv[])
{
  DGP_CONSOLE << "";
  DGP_CONSOLE << "Usage: " << argv[0] << " <mesh>";
  DGP_CONSOLE << "";

  return -1;
}
//extern MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex);
//extern bool is_saddle(MeshVertex* v);
int
main(int argc, char * argv[])
{
  if (argc < 2)
    return usage(argc, argv);

  std::string in_path = argv[1];

  Mesh mesh;
  if (!mesh.load(in_path))
    return -1;
  
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