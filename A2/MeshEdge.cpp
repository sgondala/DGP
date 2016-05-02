#include "MeshEdge.hpp"
#include "MeshFace.hpp"
#include "MeshVertex.hpp"
#include "DGP/Vector4.hpp"
#include <math.h>


MeshEdge *
MeshEdge::nextAroundEndpoint(int i)
{
  debugAssertM(i == 0 || i == 1, "MeshEdge: Invalid endpoint index");

  if (numFaces() > 2)  // non-manifold
    return NULL;

  // Find which incident face has this endpoint as the origin of the edge when stepping round the face. The required edge
  // is then the predecessor of this edge around the face.
  for (FaceIterator fi = facesBegin(); fi != facesEnd(); ++fi)
  {
    Face * face = *fi;
    MeshEdge * prev = face->getPredecessor(this);
    if (prev->hasEndpoint(endpoints[i]))  // found it!
      return prev;
  }

  return NULL;
}

void
MeshEdge::updateQuadricCollapseError()
{
  Vertex* v0 = getEndpoint(0);
  Vertex* v1 = getEndpoint(1);
  DMat4 newQuadric = v0->getQuadric() + v1->getQuadric();

  DMat4 matrixForNewVertex = newQuadric;
  matrixForNewVertex.setRow(3, Vector4(0.0, 0.0, 0.0, 1.0));

  double determinant = matrixForNewVertex.determinant();
  if(abs(determinant)<0.000001){
    quadric_collapse_position = (v0->getPosition() + v1->getPosition())/2.0;
  } 
  else{
    Vector4 tempVec(0.0,0.0,0.0,1.0); // = Vector4(0,0,0,0);
    auto productVector = matrixForNewVertex.inverse() * tempVec;
    for(int i=0; i<3; i++){
      quadric_collapse_position[i] = productVector[i];
    }  
  }

  quadric_collapse_error = Vector4(quadric_collapse_position,1).
                  dot(newQuadric*Vector4(quadric_collapse_position,1));

  // quadric_collapse_error = 
  // TODO

  // Update both quadric_collapse_error and quadric_collapse_position, 
  // using the existing endpoint quadrics and the method of
  // Garland/Heckbert.
  //
  // NOTE: Remember to check if the quadric Q' is invertible. If not, you will have to use a fallback option such as the
  // midpoint of the edge (or in the worst case, set the error to a negative value to indicate this edge should not be
  // collapsed).
}
