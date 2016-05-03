#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "Window.cpp"

#ifndef __Helper_HPP__
#define __Helper_HPP__

MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex);

bool is_saddle(MeshVertex* v);

std::pair<std::pair<Vector3,double>,std::pair<Vector3,double> > angle(myWindow* w);
std::pair<myWindow*,myWindow* >newWindow(myWindow* parent,double p1, MeshEdge* e1, double d1, double angle1, double p2, MeshEdge* e2, double d2, double angle2, MeshFace* f);

#endif