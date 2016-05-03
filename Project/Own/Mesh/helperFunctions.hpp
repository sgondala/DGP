#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "Window.cpp"
#include <tuple>
#include <set>
#include <list>
#ifndef __Helper_HPP__
#define __Helper_HPP__

MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex);

bool is_saddle(MeshVertex* v);

std::pair<std::pair<Vector3,double>,std::pair<Vector3,double> > angle(myWindow* w);
std::pair<myWindow*,myWindow*> newWindow(myWindow* parent,double p1, MeshEdge* e1, double d1, double angle1, double p2, MeshEdge* e2, double d2, double angle2, MeshFace* f);
double angleAtVertex(MeshVertex*, MeshFace*);
std::tuple<double, double, double> getPointPositionOnEdge(MeshEdge* incidentEdge,MeshEdge* otherEdge, MeshVertex* commonVertex, 
	MeshFace* face, double angle, Vector3 point);

std::tuple<double, double, double, MeshEdge*> getPointPosition(Vector3 point, double angle,
	MeshFace* incidentFace, MeshEdge* edge);

MeshVertex* getCommonVertex(MeshEdge* e1, MeshEdge* e2);
std::list<myWindow*> add_window_edge(myWindow* w, std::set<myWindow*> *pQueue);

#endif