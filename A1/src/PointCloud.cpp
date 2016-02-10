#include "PointCloud.hpp"
#include "DGP/Graphics/Shader.hpp"
#include <fstream>
#include <sstream>

PointCloud::PointCloud(std::vector<Point> const & points_)
: points(points_)
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

Graphics::Shader *
createPointShader(Graphics::RenderSystem & rs)
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

void
PointCloud::estimateNormals()
{
  // TODO
}

void
PointCloud::adaptiveDownsample()
{
  // TODO
}
