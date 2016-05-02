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
			return &(*it);
		}
	}
}

#endif