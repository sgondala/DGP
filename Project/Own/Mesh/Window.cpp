#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include <algorithm>

#ifndef __myWindow_CPP__
#define __myWindow_CPP__

struct myWindow{
	MeshEdge* e1;
	double d1,d2;
	double p1,p2; // Between 0 and 1
	MeshFace* incidentFacePtr; // Depending on face 
	// MeshVertex* lastVertex;
	double distanceOfLast;

	myWindow(){}

	myWindow(MeshVertex *v, MeshEdge *e, MeshFace *f){ // 
		e1 = e;
		incidentFacePtr = f;
		distanceOfLast = 0;
		p1 = 0;
		p2 = 1;
		d1 = (e->getEndpoint(0)->getPosition() - v->getPosition()).length();	
		d2 = (e->getEndpoint(1)->getPosition() - v->getPosition()).length();	
	}

	myWindow(MeshEdge *e, MeshFace *f, double d11, double d12, double p11, double p12, double dOL){
		e1 = e;
		incidentFacePtr = f;
		distanceOfLast = dOL;
		p1 = p11;
		p2 = p12;
		distanceOfLast =  dOL;
		d1 = d11;
		d2 = d12;
	}
	
	bool operator<(const myWindow& rhs) const{
		return std::min(d1 + distanceOfLast,d2 + distanceOfLast)<
		std::min(rhs.d1 + rhs.distanceOfLast,rhs.d2 + rhs.distanceOfLast);
	}
};

#endif

