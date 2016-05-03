#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "Window.cpp"
#include <math.h>

#ifndef __Helper_CPP__
#define __Helper_CPP__

// Gives the edge opp to vertex in the face
MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex){
	for(auto it = face->edgesBegin(); it!= face->edgesEnd(); it++){
		if(!((*it)->hasEndpoint(vertex))){
			return *it;
		}
	}
}

// Mahi
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


//Returns angles,endpoints of a window
std::pair<std::pair<Vector3,double>,std::pair<Vector3,double> > angle(myWindow* w){
  Vector3 v1 = w->e1->getEndpoint(0)->getPosition();
  Vector3 v2 = w->e1->getEndpoint(1)->getPosition();

  Vector3 v_orig1 = v1*(1 - w->p1) + v2*w->p1;
  Vector3 v_orig2 = v1*(1 - w->p2) + v2*(w->p2);

  double length_side = (v_orig1 - v_orig2).length();
  double angle1 = DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((w->d1*w->d1 + length_side*length_side - w->d2*w->d2)/2*(w->d1*length_side)));
  	double angle2 = DGP::Math::radiansToDegrees(DGP::Math::fastArcCos((w->d2*w->d2 + length_side*length_side - w->d1*w->d1)/2*(w->d2*length_side)));
  return std::make_pair(std::make_pair(v_orig1, 180 - angle1),std::make_pair(v_orig2,angle2));

}

double angleAtVertex(MeshVertex* v, MeshFace* f){
	MeshVertex* v1 = NULL, *v2 = NULL;
	for(auto it = f->verticesBegin(); it!=f->verticesEnd(); it++){
		if(*it==v){continue;}
		if(v1==NULL){
			v1 = *it;
		}
		else{
			v2 = *it;
		}		
	}
	Vector3 vec1 = v->getPosition() - v1->getPosition();
	Vector3 vec2 = v->getPosition() - v2->getPosition();
	double vertexAngle = Math::radiansToDegrees(Math::fastArcCos((vec1.dot(vec2))/(vec1.length()*vec2.length())));
	return vertexAngle;
}

// Takes 2 edges, common vertex, a point and angle of inner triangle
// Returns point value [0,1] writ endPoint[0], angle to endpoint0, distance
std::tuple<double, double, double> getPointPositionOnEdge(MeshEdge* incidentEdge,MeshEdge* otherEdge, MeshVertex* commonVertex, 
	MeshFace* face, double angle, Vector3 point){
	double vertexAngle = angleAtVertex(commonVertex, face);
	double lengthToPoint = (commonVertex->getPosition() - point).length();
	double distanceOnOtherEdge = sin(angle * 3.1415/180) * lengthToPoint / sin((vertexAngle + angle)*3.1415/180);
	double lengthOfOtherEdge = otherEdge->getLength();
	if(lengthOfOtherEdge < distanceOnOtherEdge){ // Lies beyond the point
		double returnVal = -1;
		double returnAngle = -1;
		double returnDistance = -1;
		return std::make_tuple(returnVal, returnAngle, returnDistance);
	}
	else{
		if(otherEdge->getEndpointIndex(commonVertex)==0){
			double returnVal = distanceOnOtherEdge/lengthOfOtherEdge;
			double returnAngle = 180 - (vertexAngle + angle);
			double returnDistance = sin(vertexAngle * 3.1415/180) *  lengthToPoint / sin((vertexAngle + angle)*3.1415/180);
			return std::make_tuple(returnVal, returnAngle, returnDistance);
		}
		else{
			double returnVal = 1 - distanceOnOtherEdge/lengthOfOtherEdge;
			double returnAngle = (vertexAngle + angle);
			double returnDistance = sin(vertexAngle * 3.1415/180) *  lengthToPoint / sin((vertexAngle + angle)*3.1415/180);
			return std::make_tuple(returnVal, returnAngle, returnDistance);
		}
	}
}

// Assuming they have a common vertex
MeshVertex* getCommonVertex(MeshEdge* e1, MeshEdge* e2){
	if((e1->getEndpoint(0) == e2->getEndpoint(0)) ||
		(e1->getEndpoint(0) == e2->getEndpoint(1))){
		return e1->getEndpoint(0);
	}
	return e1->getEndpoint(1);
}

std::tuple<double,double,double,MeshEdge*> getPointPosition(Vector3 point, double angle,
	MeshFace* incidentFace, MeshEdge* edge){

	MeshFace* otherFace = NULL;
	for(auto it = edge->facesBegin(); it != edge->facesEnd(); it++){
		if(*it != incidentFace){
			otherFace = *it;
		}
	}

	for(auto it = otherFace->edgesBegin(); it!=otherFace->edgesEnd(); it++){
		if(*it == edge){continue;}
		MeshEdge* otherEdge = *it;
		MeshVertex* commonVertex = getCommonVertex(edge, otherEdge);
		double angleToBeSent;
		if(edge->getEndpointIndex(commonVertex)==0){
			angleToBeSent = 180 - angle;
		}
		else{
			angleToBeSent = angle; 
		}
		auto responseVal = getPointPositionOnEdge(edge, otherEdge, commonVertex, 
			otherFace, angleToBeSent, point);
		if(std::get<0>(responseVal) == -1){
			continue;
		}
		double returnVal = std::get<0>(responseVal);
		double returnAngle = std::get<1>(responseVal);
		double returnDistance = std::get<2>(responseVal);
		MeshEdge* returnEdge = *it;
		return std::make_tuple(returnVal, returnAngle, returnDistance, returnEdge);
	}
}





std::pair<myWindow*,myWindow* >newWindow(myWindow* parent,double p1, MeshEdge* e1, double d1, double angle1, double p2, MeshEdge* e2, double d2, double angle2, MeshFace* f){

	if(e1 == e2){
		return new myWindow(e1, f, d1 + parent->d1, d2 + parent->d2, p1, p2, parent->distanceOfLast);
	}else{
		int endpoint_consider = 0;
		if(parent->e1->getEndpoint(0) == e1->getEndpoint(0) || parent->e1->getEndpoint(1) == e1->getEndpoint(0)){
			angle1 = 180 - angle1;
			endpoint_consider = 1;
		}
		double l1 = d1 + parent->d1;
		Vector3 point1 = (1 - p1)*e1->getEndpoint(0) + p1*e1->getEndpoint(1);
		double l2 = (point1 - e1->getEndpoint(endpoint_consider)).length();
		double d1_window = std::sqrt(l1*l1 + l2*l2 - 2*l1*l2*cos(Math::degreesToRadians(angle1)));	

		myWindow* w1;
		if(!endpoint_consider)w1 = new myWindow(e1, f, d1_window, l1, 0, p1, parent->distanceOfLast);
		else w1 = new myWindow(e1, f, l1, d1_window, p1, 1, parent->distanceOfLast);

		endpoint_consider = 0;
		if(parent->e1->getEndpoint(0) == e2->getEndpoint(0) || parent->e1->getEndpoint(1) == e2->getEndpoint(0)){
			angle2 = 180 - angle2;
			endpoint_consider = 1;
		}
		l1 = d2 + parent->d2;
		point1 = (1 - p2)*e2->getEndpoint(0) + (p2)*e2->getEndpoint(1);
		l2 = (point1 - e2->getEndpoint(endpoint_consider)).length();
		d1_window = std::sqrt(l1*l1 + l2*l2 - 2*l1*l2*cos(Math::degreesToRadians(angle2)));	

		myWindow* w2 = NULL;
		if(!endpoint_consider)w2 = new myWindow(e2, f, d1_window, l1, 0, p2, parent->distanceOfLast);
		else w2 = new myWindow(e2, f, l1, d1_window, p2, 1, parent->distanceOfLast);

		return std::make_pair(w1, w2);

	}
	
}


//This function splits windows with common interval 
list<myWindow* > add_window_edge(myWindow* w, set<myWindow*> &pQueue){
	list<myWindow*> l1;
	double length_edge = w->e1->getLength();
	for (auto i = w->e1->windows.begin(); i != w->e1->windows.end(); ++i)
	{
		
		if(w->p1 < (*i)->p1 &&  w->p2 > (*i)->p2){
		   std::pair<double, double> source0 = w->getSourcePoint();
			double ip1x = ((*i)->p1 - w->p1)*length_edge;
			double d11 = sqrt((source0->first - ip1x)*(source0->first - ip1x)) + (source0->second*source0->second));
			myWindow* w11 = new myWindow(w->e1, w->f, w->d1, d11, w->p1, (ip1x + w->p1*length_edge)/length_edge, w->distanceOfLast);
			w->p1 = (*i)->p2;
			w->d1 = (*i)->d2;
			w11->e1->windows.push_back(w11);
			pQueue.insert(w11);

		}else if(w->p1 > (*i)->p1 &&  w->p2 < (*i)->p2){
			std::pair<double, double> source0 = (*i)->getSourcePoint();
			double wp1x = (w->p1 - (*i)->p1)*length_edge;
			double d11 = sqrt((source0->first - wp1x)*(source0->first - wp1x)) + (source0->second*source0->second));
			myWindow* w11 = new myWindow(w->e1, w->f, (*i)->d1, d11, (*i)->p1, (wp1x + (*i)->p1*length_edge)/length_edge, (*i)->distanceOfLast);
			(*i)->p1 = w->p2;
			(*i)->d1 = w->d2;
			w11->e1->windows.push_back(w11);
			pQueue.insert(w11);
			if(!pQueue.find(*i1))pQueue.insert(*i1);

		}
		else if(w->p1 < (*i)-> p1 && w->p2 > (*i)-> p1){
			double wp1x = 0, wp1y = 0;
			double wp2x = (w->p2 - w->p1)*length_edge, wp2y = 0;
			std::pair<double, double> source0 = w->getSourcePoint();
			double ip1x = ((*i)->p1 - w->p1)*length_edge, ip1y = 0;
			double ip2x = ((*i)->p2 - w->p1)*length_edge, ip2y = 0;
			std::pair<double, double> source1 = (*i)->getSourcePoint();
			source1->first += ip1x;

			double alpha = source1->first - source0->first;
			double beta = (*i1)->distanceOfLast - w->distanceOfLast;
			double gamma = (source0->first)*(source0->first) + (source0->second)*(source0->second) - (source1->first)*(source1->first) - (source1->second)*(source1->second);
			gamma -= beta*beta;

			double A = (alpha*alpha - beta*beta);
			double B = (gamma*alpha + 2*source1->first*beta*beta);
			double C = 0.25*gamma*gamma - ((source1->first)*(source1->first) + (source1->second)*(source1->second))*beta*beta;

			double sol1,sol2;
			if(B*B - 4*A*C >= 0)sol1 = (-B + sqrt(B*B - 4*A*C))/(2*A);
			if(B*B - 4*A*C >= 0)sol2 = (-B - sqrt(B*B - 4*A*C))/(2*A);
			double new_d;
			if( sol1 >= ip1x && sol1 <= wp2x){
				double new_p2 = (sol1 + w->p1*length_edge)/length_edge;
				w->p2 = new_p2;
				new_d = sqrt((source0->first - sol1)*(source0->first - sol1) + source0->second*source0->second);
				w->d2 = new_d;
				(*i)->p1 = new_p2;
				(*i)->d1 = new_d;
			}else if(sol2 >= ip1x && sol2 <= wp2x){
				double new_p2 = (sol2 + w->p1*length_edge)/length_edge;
				w->p2 = new_p2;
				new_d = sqrt((source0->first - sol2)*(source0->first - sol2) + source0->second*source0->second);
				w->d2 = new_d;
				(*i)->p1 = new_p2;
				(*i)->d1 = new_d;
			}
			if(!pQueue.find(*i1))pQueue.insert(*i1);


		}else if(w->p1 > (*i)-> p1 && w->p2  < (*i)->p2){
			double ip1x = 0, ip1y = 0;
			double ip2x = ((*i)->p2 - (*i)->p1)*length_edge, ip2y = 0;
			std::pair<double, double> source0 = (*i)->getSourcePoint();
			double wp1x = (w->p1 - (*i)->p1)*length_edge, wp1y = 0;
			double wp2x = (w->p2 - (*i)->p1)*length_edge, wp2y = 0;
			std::pair<double, double> source1 = w->getSourcePoint();
			source1->first += wp1x;

			double alpha = source1->first - source0->first;
			double beta = w->distanceOfLast - (*i1)->distanceOfLast;
			double gamma = (source0->first)*(source0->first) + (source0->second)*(source0->second) - (source1->first)*(source1->first) - (source1->second)*(source1->second);
			gamma -= beta*beta;

			double A = (alpha*alpha - beta*beta);
			double B = (gamma*alpha + 2*source1->first*beta*beta);
			double C = 0.25*gamma*gamma - ((source1->first)*(source1->first) + (source1->second)*(source1->second))*beta*beta;

			double sol1,sol2;
			if(B*B - 4*A*C >= 0)sol1 = (-B + sqrt(B*B - 4*A*C))/(2*A);
			if(B*B - 4*A*C >= 0)sol2 = (-B - sqrt(B*B - 4*A*C))/(2*A);
			double new_d;
			if( sol1 >= wp1x && sol1 <= ip2x){
				double new_p2 = (sol1 + (*i)->p1*length_edge)/length_edge;
				(*i)->p2 = new_p2;
				new_d = sqrt((source0->first - sol1)*(source0->first - sol1) + source0->second*source0->second);
				(*i)->d2 = new_d;
				w->p1 = new_p2;
				w->d1 = new_d;
			}else if(sol2 >= wp1x && sol2 <= ip2x){
				double new_p2 = (sol2 + (*i)->p1*length_edge)/length_edge;
				(*i)->p2 = new_p2;
				new_d = sqrt((source0->first - sol2)*(source0->first - sol2) + source0->second*source0->second);
				(*i)->d2 = new_d;
				w->p1 = new_p2;
				w->d1 = new_d;
			}
			if(!pQueue.find(*i1))pQueue.insert(*i1);

		}
		
	}
	w->e1->windows.push_back(w);
	pQueue.insert(w);
}

#endif