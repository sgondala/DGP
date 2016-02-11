#include "PointCloud.hpp"
#include "DGP/Graphics/Shader.hpp"
#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <iostream>

PointCloud::PointCloud(std::vector<Point> const & points_): points(points_)
{
	recomputeAABB();
}

PointCloud::PointCloud(std::vector<Vector3> const & positions, std::vector<Vector3> const & normals)
{
	alwaysAssertM(positions.size() == normals.size(), "PointCloud: Number of positions != number of normals");

	for (size_t i = 0; i < positions.size(); ++i)
		points.push_back(Point(positions[i], normals[i]));

	recomputeAABB();
}

bool
PointCloud::load(std::string const & path)
{
	// Simple format: Each line is either
	//   x y z
	//    OR
	//   x y z nx ny nz
	//
	// where (nx, ny, nz) is the normal

	std::ifstream in(path.c_str());
	if (!in)
	{
		DGP_ERROR << "Could not open file for reading: " << path;
		return false;
	}

	std::string line;
	while (getline(in, line))
	{
		// Skip empty lines
		line = trimWhitespace(line);
		if (line.empty())
			continue;

		std::istringstream line_in(line);
		Vector3 p;
		if (!(line_in >> p[0] >> p[1] >> p[2]))
		{
			DGP_ERROR << "Could not read point " << points.size() << " from line: " << line;
			return false;
		}

		// Normal is optional
		Vector3 n;
		if (!(line_in >> n[0] >> n[1] >> n[2]))  // doesn't properly handle malformed lines, but we'll ignore this for now
			n = Vector3::zero();

		points.push_back(Point(p, n));
	}

	recomputeAABB();

	return true;
}

bool
PointCloud::save(std::string const & path) const
{
	std::ofstream out(path.c_str(), std::ios::binary);
	if (!out)
	{
		DGP_ERROR << "Could not open file for writing: " << path;
		return false;
	}

	for (size_t i = 0; i < points.size(); ++i)
	{
		Vector3 const & p = points[i].getPosition();
		Vector3 const & n = points[i].getNormal();
		out << p[0] << ' ' << p[1] << ' ' << p[2] << ' ' << n[0] << ' ' << n[1] << ' ' << n[2] << '\n';
	}

	return true;
}

Graphics::Shader * createPointShader(Graphics::RenderSystem & rs)
{
	static std::string const VERTEX_SHADER =
"void main()\n"
"{\n"
"  gl_Position = ftransform();\n"
"  gl_FrontColor = gl_Color;\n"
"  gl_BackColor = gl_Color;\n"
"}\n";

	static std::string const FRAGMENT_SHADER =
"void main()\n"
"{\n"
"  gl_FragColor = gl_Color;\n"
"}\n";

	Graphics::Shader * shader = rs.createShader("Point Graphics::Shader");
	if (!shader)
		throw Error("Could not create point shader");

	// Will throw errors on failure
	shader->attachModuleFromString(Graphics::Shader::ModuleType::VERTEX, VERTEX_SHADER.c_str());
	shader->attachModuleFromString(Graphics::Shader::ModuleType::FRAGMENT, FRAGMENT_SHADER.c_str());

	return shader;
}

void
PointCloud::draw(Graphics::RenderSystem & rs, Real normal_len, ColorRGBA const & color) const
{
	// Make this static to ensure just one shader is created. Assumes rendersystem is constant, not the best design pattern.
	static Graphics::Shader * shader = createPointShader(rs);

	rs.pushShader();
	rs.pushColorFlags();
	rs.pushShapeFlags();

		rs.setShader(shader);
		rs.setColor(color);  // dark grey
		rs.setPointSize(2.0f);

		rs.beginPrimitive(Graphics::RenderSystem::Primitive::POINTS);
			for (size_t i = 0; i < points.size(); ++i)
				rs.sendVertex(points[i].getPosition());
		rs.endPrimitive();

		if (normal_len > 0)
		{
			rs.setColor(ColorRGB(0.5, 0.5, 1.0));  // blue

			rs.beginPrimitive(Graphics::RenderSystem::Primitive::LINES);
				for (size_t i = 0; i < points.size(); ++i)
				{
					Vector3 const & p = points[i].getPosition();
					Vector3 const & n = points[i].getNormal();

					rs.sendVertex(p);
					rs.sendVertex(p + normal_len * n);
				}
			rs.endPrimitive();
		}

	rs.popShapeFlags();
	rs.popColorFlags();
	rs.popShader();
}

void
PointCloud::recomputeAABB()
{
	bbox.setNull();

	for (size_t i = 0; i < points.size(); ++i)
		bbox.merge(points[i].getPosition());
}

// void findPointsInBox(AxisAlignedBox3 &boxTemp,
// 	std::vector<Point> &pointsTemp, std::vector<Point> &points){
	
// 	for(int i=0; i<points.size(); i++){
// 		if(boxTemp.intersects(points[i].getPosition())){
// 			pointsTemp.push_back(points[i]);
// 		}
// 	}
// }

int getMinIndex(float* f){
	float minVal = std::min(f[0], std::min(f[1],f[2]));
	if(minVal==f[0]){return 0;}
	if(minVal==f[1]){return 1;}
	if(minVal==f[2]){return 2;}
}

void findPointsKDTree(AxisAlignedBox3 &boxTemp,
	std::vector<Point> &pointsTemp, 
	PointKDTree &kdTree){
	std::vector<const Point*> pointerToPoints;
	kdTree.rangeQuery(boxTemp, pointerToPoints);
	for(int i=0; i<pointerToPoints.size(); i++){
		pointsTemp.push_back(*pointerToPoints[i]);
	}
}

void
PointCloud::estimateNormals(){
	int radius = 2; // TODO ME How much to put the radius
	PointKDTree kdTree(points);
	for(int i=0; i<points.size(); i++){
		Point current = points[i];
		Vector3 currentPosition = current.getPosition();
		AxisAlignedBox3 boxTemp;
		Vector3 low, high;
		for(int i=0; i<3; i++){
			low[i] = currentPosition[i] - radius;
			high[i]  = currentPosition[i] + radius;
		}
		boxTemp.addPoint(low);
		boxTemp.addPoint(high);
		
		//Finding points in box
		std::vector<Point> pointsTemp;
		// Normal Way
		// findPointsInBox(boxTemp, pointsTemp, points);
		
		// KD Tree
		findPointsKDTree(boxTemp, pointsTemp, kdTree);

		//Finding a
		VectorN<3,float> a(0);
		for(int i=0; i<pointsTemp.size(); i++){
			a += pointsTemp[i].getPosition();
		}
		a /= pointsTemp.size();

		//Making matrix
		Matrix3 finalMatrix;
		finalMatrix.makeZero();
		for(int i=0; i<pointsTemp.size(); i++){
			// Vector3 ul = pointsTemp[i].getPosition() - a;
			MatrixMN<3,1,float> y;
			y.setColumn(0, pointsTemp[i].getPosition() - a);
			MatrixMN<1,3,float> yTrans;
			yTrans.setRow(0, pointsTemp[i].getPosition() - a);
			Matrix3 finalMatrixTemp = y*yTrans;
			finalMatrix += finalMatrixTemp;
		}

		float* eigenValues;
		eigenValues = (float*) malloc(sizeof(float)*3);
		VectorN<3,float> *eigenVectors;
		eigenVectors = (VectorN<3,float>*) malloc(sizeof(VectorN<3,float>)*3);
		finalMatrix.eigenSolveSymmetric(eigenValues, eigenVectors);

		int indexOfEigenVector = getMinIndex(eigenValues);
		VectorN<3,float> normalRequired = eigenVectors[indexOfEigenVector];
		points[i].setNormal(normalRequired);
	}

}



void
PointCloud::adaptiveDownsample()
{
	// TODO
}
