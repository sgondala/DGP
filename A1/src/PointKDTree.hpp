#ifndef __A1_PointKDTree_hpp__
#define __A1_PointKDTree_hpp__

#include "Common.hpp"
#include "Point.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Noncopyable.hpp"
#include "DGP/Vector3.hpp"

    /** A node of the kd-tree. */
struct Node {
  /** Constructor. */
  Node() : lo(NULL), hi(NULL) {}

  /** Destructor. */
  ~Node();

  Node(std::vector<Point*> &points);

  AxisAlignedBox3 bbox;         ///< Bounding box of the node.
  Node * lo;                    ///< Child of the node with smaller coordinates.
  Node * hi;                    ///< Child of the node with larger coordinates.
  std::vector<Point *> points;  ///< Set of points in the node (empty unless leaf node).
};



/* A kd-tree on points. */
class PointKDTree : private Noncopyable
{
  public:
    /** The root node of the kd-tree. */
    Node *root;

    /** TODO: Construct from a set of points. The points must last as long this kd-tree does. */
    PointKDTree(std::vector<Point> const & points);

    /** Destructor. */
    ~PointKDTree() { clear();}

    /** Reset the tree to an empty state. */
    void clear() { delete root; root = NULL; }

    /**
     * TODO: Do a range query, returning the set of points within the query box.
     *
     * @param query The query box.
     * @param points_in_range Used to return the set of points within the query box.
     *
     * Note that if needed, the index of a returned point p (of type Point *) can be recovered as (p - &points[0]), where
     * points is the array used to construct the kd-tree.
     */
    void rangeQuery(AxisAlignedBox3 const & query, std::vector<Point *> & points_in_range) const;

}; // class PointKDTree

#endif
