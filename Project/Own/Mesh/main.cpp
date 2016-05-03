#include "Window.cpp"
#include "helperFunctions.hpp"
#include <set>
#include <iostream>

int
usage(int argc, char * argv[])
{
  DGP_CONSOLE << "";
  DGP_CONSOLE << "Usage: " << argv[0] << " <mesh>";
  DGP_CONSOLE << "";
  return -1;
}

int
main(int argc, char * argv[])
{
  if (argc < 2)
    return usage(argc, argv);

  std::string in_path = argv[1];

  Mesh mesh;
  if (!mesh.load(in_path))
    return -1;
  
  std::set<myWindow*> pQueue;
  MeshVertex* src, *dst;
  int srcIndex, dstIndex;
  std::cin >> srcIndex >> dstIndex;

  int ind = 0;
  for(auto it = mesh.verticesBegin(); it!= mesh.verticesEnd(); it++){
    if(ind==srcIndex) src = &(*it);
    if(ind==dstIndex) dst = &(*it);
    Vector3 temp  = (*it).getPosition();
    std::cout << temp[0] << " " << temp[1] << " " << temp[2] << " " << std::endl;
    ind++;
  }

  // Get the first set of windows from the Mesh Vertex
  for(auto it = src->facesBegin(); it!= src->facesEnd(); it++){
    MeshEdge* e = getOtherEdge(*it, src);
    myWindow *temp = new myWindow(src,e,*it);
    pQueue.insert(temp);
  }



  // Dijkstra's algorithm starts  
  while(!pQueue.empty()){
    myWindow* parent = *(pQueue.begin());
    pQueue.erase(pQueue.begin());
    auto preReqs = angle(parent);
    // p1,e1,d1,angle1,2,face
    auto response1 = getPointPosition(preReqs.first.first, preReqs.first.second, parent->incidentFacePtr,parent->e1);
    auto response2 = getPointPosition(preReqs.second.first, preReqs.second.second, parent->incidentFacePtr,parent->e1);
    double p1 = std::get<0>(response1), p2 = std::get<0>(response2);
    MeshEdge* e1 = std::get<3>(response1), *e2 = std::get<3>(response2);
    double d1 = std::get<2>(response1), d2 = std::get<2>(response2);
    double angle1 = std::get<1>(response1), angle2 = std::get<1>(response2);
    MeshFace* fCommon = NULL;
    for(auto it = parent->e1->facesBegin(); it != parent->e1->facesEnd(); it++){
      if(*it!=parent->incidentFacePtr){
        fCommon = *it;
      }
    }
    if(fCommon==NULL){continue;}
    auto newWindows = newWindow(parent, p1, e1, d1, angle1, p2, e2, d2, angle2, fCommon);      
    std :: cout<< "!! " <<std::endl;
    add_window_edge(newWindows.first, &pQueue);
    std :: cout<< "22 " <<std::endl;
    if(newWindows.second != NULL) add_window_edge(newWindows.second, &pQueue);
  }


  // Assuming vertex has atleast 1 edge
  MeshEdge* dstEdge = *(dst->edgesBegin());
   int indexOfVertex = dstEdge->getEndpointIndex(dst);
   double p1 = 1, p2 = 0, dstP1 = 0, dstP2 = 0;
   for(auto it = (dstEdge->windows).begin(); it!= (dstEdge->windows).end(); it++){
    if(p1 > std::min((*it)->p1, (*it)->p2)){
      p1 = std::min((*it)->p1, (*it)->p2);
      dstP1 =  (p1 == (*it)->p1)? (*it)->d1 : (*it)->d2;
    }
    if(p2 < std::max((*it)->p1, (*it)->p2)){
      p2 = std::max((*it)->p1, (*it)->p2);
      dstP2 =  (p2 == (*it)->p1)? (*it)->d1 : (*it)->d2;
    }
   }
   if(indexOfVertex == 0){
    std::cout <<  dstP1 <<std::endl;
   }
   else{
    std::cout <<  dstP2 <<std::endl;
  }

}