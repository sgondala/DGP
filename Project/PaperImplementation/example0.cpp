/* read mesh from file and 
 - if one vertex is specified, for all vertices of the mesh print their distances to this vertex
 - if two vertices are specified, print the shortest path between these vertices 

	Danil Kirsanov, 01/2008 
*/
#include <iostream>
#include <fstream>

#include "geodesic_algorithm_exact.h"

int main(int argc, char **argv) 
{
	if(argc < 2)
	{
		std::cout << "usage: mesh_file_name [first_vertex] [second_vertex]" << std::endl; //try: "hedgehog_mesh.txt 3 14" or "flat_triangular_mesh.txt 1"
		return 0;
	}

	std::vector<double> points;	
	std::vector<unsigned> faces;

	bool success = geodesic::read_mesh_from_file(argv[1],points,faces);
	if(!success)
	{
		std::cout << "something is wrong with the input file" << std::endl;
		return 0;
	}

	geodesic::Mesh mesh;
	mesh.initialize_mesh_data(points, faces);		//create internal mesh data structure including edges

	geodesic::GeodesicAlgorithmExact algorithm(&mesh);	//create exact algorithm for the mesh

	unsigned source_vertex_index = (argc == 2) ? 0 : atol(argv[2]);

	geodesic::SurfacePoint source(&mesh.vertices()[source_vertex_index]);		//create source 
	std::vector<geodesic::SurfacePoint> all_sources(1,source);					//in general, there could be multiple sources, but now we have only one

	if(argc > 3)	//target vertex specified, compute single path
	{
		unsigned target_vertex_index = atol(argv[3]);
		geodesic::SurfacePoint target(&mesh.vertices()[target_vertex_index]);		//create source 

		std::vector<geodesic::SurfacePoint> path;	//geodesic path is a sequence of SurfacePoints
		double const distance_limit = geodesic::GEODESIC_INF;			// no limit for propagation
		std::vector<geodesic::SurfacePoint> stop_points(1, target);	//stop propagation when the target is covered
		algorithm.propagate(all_sources, distance_limit, &stop_points);	//"propagate(all_sources)" is also fine, but take more time because covers the whole mesh

		algorithm.trace_back(target, path);		//trace back a single path 
		
		print_info_about_path(path);

	}
	return 0;
}	