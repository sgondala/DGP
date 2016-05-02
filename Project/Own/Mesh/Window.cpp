#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include <algorithm>

#ifndef __Window_CPP__
#define __Window_CPP__

struct Window{
	MeshEdge* e1;
	double d1,d2;
	double p1,p2; // Between 0 and 1
	MeshFace* incidentFacePtr; // Depending on face 
	// MeshVertex* lastVertex;
	double distanceOfLast;

	Window(){}

	Window(MeshVertex *v, MeshEdge *e, MeshFace *f){ // 
		e1 = e;
		incidentFacePtr = f;
		distanceOfLast = 0;
		p1 = 0;
		p2 = 1;
		d1 = (e->getEndpoint(0)->getPosition() - v->getPosition()).length();	
		d2 = (e->getEndpoint(1)->getPosition() - v->getPosition()).length();	
	}
	
	bool operator<(const Window& rhs) const{
		return std::min(d1 + distanceOfLast,d2 + distanceOfLast)<
		std::min(rhs.d1 + rhs.distanceOfLast,rhs.d2 + rhs.distanceOfLast);
	}
};

#endif

