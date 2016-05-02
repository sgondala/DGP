#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "Window.cpp"

#ifndef __Helper_CPP__
#define __Helper_CPP__

MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex){
	for(auto it = face->edgesBegin(); it!= face->edgesEnd(); it++){
		if(!((*it)->hasEndpoint(vertex))){
			return *it;
		}
	}
}


bool is_saddle(MeshVertex* v){
      double angle = 0;
      for (auto fi = v->facesBegin(); fi != v->facesEnd(); ++fi){
        MeshVertex* v1;
        MeshVertex* v2;
        MeshVertex* v3;
        
        std::list<MeshVertex* >::iterator i1 = (*fi)->verticesBegin();
        int flag = 0;
        v1 = *i1;
        if(v1 == v)flag = 1;
        i1++;
        v2 = *i1;
        if(v2 == v)flag = 2;
        i1++;
        v3 = *i1;
        if(v3 == v)flag = 3;

        Vector3 v1position = v1->getPosition();
        Vector3 v2position = v2->getPosition();
        Vector3 v3position = v3->getPosition();
        if(flag == 1){
          Vector3 p1 = v2position - v1position;
          Vector3 p2 = v3position - v1position;
          angle += DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((p1.dot(p2))/(p1.length()*p2.length())));
        }else if(flag == 2){
          Vector3 p1 = v1position - v2position;
          Vector3 p2 = v3position - v2position;
          angle += DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((p1.dot(p2))/(p1.length()*p2.length())));
        }else if(flag == 3){
          Vector3 p1 = v1position - v3position;
          Vector3 p2 = v2position - v3position;
          angle += DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((p1.dot(p2))/(p1.length()*p2.length())));
        }
      }
      return (angle > 360.0);
    }

#endif