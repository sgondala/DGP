#include "PointKDTree.hpp"

Node::~Node() {
	delete lo;
	delete hi;
}

void fillLoAndHi(std::vector<const Point*> &loPoints, 
	std::vector<const Point*> &hiPoints, std::vector<const Point*> &points, 
	float midValue, int index){
	for(int i=0; i<points.size(); i++){
		if((points[i]->getPosition())[index] < midValue){
			loPoints.push_back(points[i]);
		}
		else{
			hiPoints.push_back(points[i]);
		}
	}
}

Node::Node(std::vector<const Point*> &points){
	static size_t const MAX_POINTS_PER_LEAF = 5;
	for(int i=0; i<points.size(); i++){
		bbox.addPoint((*points[i]).getPosition());
	}
	if(points.size()<=MAX_POINTS_PER_LEAF){
		lo = hi = NULL;
		for (int i=0; i<points.size(); i++){
			(this->points).push_back(points[i]);
		}
	}
	else {
		std::vector<const Point*> loPoints; //TODO ME Might have a leak here
		std::vector<const Point*> hiPoints;
		Vector3 high = bbox.getHigh();
		Vector3 low = bbox.getLow();
		int maxLength = std::max(high[0]-low[0], std::max(high[1] - low[1], high[2] - low[2]));
		if(maxLength == high[0] - low[0]){
			fillLoAndHi(loPoints, hiPoints, points, (high[0] + low[0])/2.0, 0);
		}
		else if(maxLength == high[1] - low[1]){
			fillLoAndHi(loPoints, hiPoints, points, (high[1] + low[1])/2.0, 1);
		}
		else {
			fillLoAndHi(loPoints, hiPoints, points, (high[2] + low[2])/2.0, 2);
		}
		lo = new Node(loPoints);
		hi = new Node(hiPoints);
		loPoints.clear();
		hiPoints.clear();
	}
}

PointKDTree::PointKDTree(std::vector<Point> const & points): root(NULL) {
	std::vector<const Point*> tempPoints; //Probable memory wastage
	for(int i=0; i<points.size(); i++){
		tempPoints.push_back(&points[i]);
	}
	root = new Node(tempPoints);
	tempPoints.clear();
}

void fillPoints(Node* root, std::vector<const Point*> &allPoints){
	if(root==NULL){return;}
	if((root->points).size()==0){
		fillPoints(root->lo, allPoints);
		fillPoints(root->hi, allPoints);
	}
	else{
		for(int i=0; i<(root->points).size(); i++){
			allPoints.push_back((root->points)[i]);
		}
	}
}

void PointKDTree::rangeQuery(AxisAlignedBox3 const & query, 
	std::vector<const Point *> & points_in_range) const{
	points_in_range.clear();

	// Bevarse version
	std::vector<const Point*> allPoints;
	fillPoints(root, allPoints);
	for(int i=0; i<allPoints.size(); i++){
		if(query.intersects(allPoints[i]->getPosition())){
			points_in_range.push_back(allPoints[i]);
		}
	}

	// Write a helper function rangeQuery(node, query, points_in_range):
	//   - If node->bbox does not intersect query, return
	//   - If node->lo && node->lo->bbox intersects query, rangeQuery(node->lo, query, points_in_range)
	//   - If node->hi && node->hi->bbox intersects query, rangeQuery(node->hi, query, points_in_range)
}
