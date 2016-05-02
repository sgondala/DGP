#include "Mesh.hpp"
#include "MeshVertex.hpp"
#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "Window.cpp"

#ifndef __Helper_HPP__
#define __Helper_HPP__

MeshEdge* getOtherEdge(MeshFace* face, MeshVertex* vertex);

bool is_saddle(MeshVertex* v);

#endif