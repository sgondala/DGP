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



std::pair<std::pair<Vector3,double>,std::pair<Vector3,double> > angle(myWindow* w){
  Vector3 v1 = w->e1->getEndpoint(0)->getPosition();
  Vector3 v2 = w->e1->getEndpoint(1)->getPosition();

  Vector3 v_orig1 = v1*w->p1 + v2*(1 - w->p1);
  Vector3 v_orig2 = v1*w->p2 + v2*(1 - w->p2);

  double length_side = (v_orig1 - v_orig2).length();
  double angle1 = DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((w->d1*w->d1 + length_side*length_side - w->d2*w->d2)/2*(w->d1*length_side)));
  	double angle2 = DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((w->d2*w->d2 + length_side*length_side - w->d1*w->d1)/2*(w->d2*length_side)));
  return std::make_pair(std::make_pair(v_orig1,angle1),std::make_pair(v_orig2,angle2));

}


Vector3 getPoint(Vector3 intersection, double incidentAngle
	MeshFace* incidentFace, MeshEdge* commonEdge, MeshVertex* nearestVertex){
	double lengthToVertex = (nearestVertex->getPosition() - intersection).length();
	MeshFace* newFace = NULL;
	for(auto it = nearestVertex->facesBegin(); it!=nearestVertex->facesEnd(); it++){
		if(*it!=incidentFace){
			newFace = *it;
		}
	}
	MeshVertex* v1 = NULL, *v2 = NULL;
	for(auto it = newFace->verticesBegin(); it!=newFace->verticesEnd(); it++){
		if(*it==nearestVertex){continue;}
		if(v1==NULL){
			v1 = *it;
		}
		else{
			v2 = *it;
		}				
	}
	Vector3 vec1 = nearestVertex->getPosition() - v1->getPosition();
	Vector3 vec2 = nearestVertex->getPosition() - v2->getPosition();
	double vertexAngle = Math::radiansToDegrees(Math::fastArcCos((vec1.dot(vec2))/(vec1.length()*vec2.length())));
	double thirdAngle = 180 - (incidentAngle + vertexAngle);
		

}

#endif