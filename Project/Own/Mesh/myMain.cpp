#include "Window.cpp"
#include <set>

int main(){
	std::set<Window> pQueue;
	MeshVertex* src, *dst;
	
	// Get the first set of windows from the Mesh Vertex
	for(auto it = src->facesBegin(); it!= facesEnd(); it++){
		MeshEdge* e = getOtherEdge(*it, src);
		pQueue.insert(Window(src,e,*it));
	}

	// Dijkstra's algorithm starts
	while(!pQueue.empty()){
		Window w = *pQueue.begin();
		pQueue.erase(pQueue.begin());
		
	}

}