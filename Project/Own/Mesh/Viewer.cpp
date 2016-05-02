#include "Viewer.hpp"
#include "Mesh.hpp"
#include "DGP/Graphics/RenderSystem.hpp"
#include "DGP/Graphics/Shader.hpp"

#ifdef DGP_OSX
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

Graphics::RenderSystem * Viewer::render_system = NULL;
Mesh * Viewer::mesh = NULL;
int Viewer::width = 640;
int Viewer::height = 480;
Camera Viewer::camera;
Matrix3 Viewer::rotation = Matrix3::identity();
bool Viewer::dragging = false;
int Viewer::last_x = -1;
int Viewer::last_y = -1;
bool Viewer::show_bbox = false;
bool Viewer::show_edges = false;
MeshVertex const * Viewer::highlighted_vertex = NULL;

void
Viewer::setObject(Mesh * o)
{
  mesh = o;
}

void
Viewer::launch(int argc, char * argv[])
{
  // Create a GL context via a GLUT window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("A2::Viewer");

  // Create a rendersystem
  render_system = new Graphics::RenderSystem("RenderSystem");
  DGP_CONSOLE << render_system->describeSystem();

  // Set up callbacks
  glutDisplayFunc(draw);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyPress);
  glutMouseFunc(mousePress);
  glutMotionFunc(mouseMotion);

  // Start event processing loop
  glutMainLoop();
}

Camera
cameraFromBBox(AxisAlignedBox3 const & bbox, int width, int height, Matrix3 const & rotation)
{
  static Real const DIST = 10;
  static Real const ZOOM = 1.1;

  Real aspect_ratio = (width / (Real)height);
  Real left, right, bottom, top;
  if (aspect_ratio > 1)
  {
    left = -aspect_ratio;
    right = aspect_ratio;
    bottom = -1;
    top = 1;
  }
  else
  {
    left = -1;
    right = 1;
    bottom = -1.0f / aspect_ratio;
    top = 1.0f / aspect_ratio;
  }

  Vector3 rot_z = rotation * Vector3::unitZ();
  Vector3 rot_y = rotation * Vector3::unitY();

  Vector3 c = bbox.getCenter();
  Real scale = bbox.getExtent().length();
  CoordinateFrame3 cframe = CoordinateFrame3::fromViewFrame(c + DIST * scale * rot_z, c, rot_y);

  return Camera(cframe, Camera::ProjectionType::PERSPECTIVE,
                (left / DIST) / ZOOM,
                (right / DIST) / ZOOM,
                (bottom / DIST) / ZOOM,
                (top / DIST) / ZOOM,
                1.7f, (DIST + 1) * scale, Camera::ProjectedYDirection::UP);
}

bool
initMeshShader(Graphics::Shader & shader)
{
  static std::string const VERTEX_SHADER =
"varying vec3 normal;  // normal in camera space\n"
"\n"
"void main()\n"
"{\n"
"  gl_Position = ftransform();\n"
"\n"
"  normal = gl_NormalMatrix * gl_Normal;\n"
"\n"
"  gl_FrontColor = gl_Color;\n"
"  gl_BackColor = gl_Color;\n"
"}\n";

  static std::string const FRAGMENT_SHADER =
"uniform vec3 ambient_color;\n"
"uniform vec3 light_dir;  // must be specified in camera space, pointing towards object\n"
"uniform vec3 light_color;\n"
"uniform vec4 material;  // [ka, kl, <ignored>, <ignored>]\n"
"uniform float two_sided;\n"
"\n"
"varying vec3 normal;  // normal in camera space\n"
"\n"
"void main()\n"
"{\n"
"  vec3 N = normalize(normal);\n"
"  vec3 L = normalize(light_dir);\n"
"\n"
"  vec3 ambt_color = material[0] * gl_Color.rgb * ambient_color;\n"
"\n"
"  float NdL = -dot(N, L);\n"
"  vec3 lamb_color = (NdL >= -two_sided) ? material[1] * abs(NdL) * gl_Color.rgb * light_color : vec3(0.0, 0.0, 0.0);\n"
"\n"
"  gl_FragColor = vec4(ambt_color + lamb_color, gl_Color.a);\n"
"}\n";

  try
  {
    shader.attachModuleFromString(Graphics::Shader::ModuleType::VERTEX, VERTEX_SHADER.c_str());
    shader.attachModuleFromString(Graphics::Shader::ModuleType::FRAGMENT, FRAGMENT_SHADER.c_str());
  }
  DGP_STANDARD_CATCH_BLOCKS(return false;, ERROR, "%s", "Could not attach mesh shader module")

  shader.setUniform("light_dir", Vector3(-1, -1, -2));
  shader.setUniform("light_color", ColorRGB(1, 1, 1));
  shader.setUniform("ambient_color", ColorRGB(1, 1, 1));
  shader.setUniform("two_sided", 1.0f);
  shader.setUniform("material", Vector4(0.2f, 0.6f, 0.2f, 25));

  return true;
}

void
Viewer::draw()
{
  alwaysAssertM(render_system, "Rendersystem not created");

  render_system->setColorClearValue(ColorRGB(0, 0, 0));
  render_system->clear();

  if (mesh)
  {
    // Initialize the shader
    static Graphics::Shader * mesh_shader = NULL;  // Ok since Viewer fns are all static. Else should have a shader per Viewer.
    if (!mesh_shader)
    {
      mesh_shader = render_system->createShader("Mesh shader");
      if (!mesh_shader)
        return;

      if (!initMeshShader(*mesh_shader))
        return;
    }

    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::MODELVIEW); render_system->pushMatrix();
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::PROJECTION); render_system->pushMatrix();

      camera = cameraFromBBox(mesh->getAABB(), width, height, rotation);
      render_system->setCamera(camera);

      render_system->pushShader();

        render_system->setShader(mesh_shader);
        render_system->setColor(ColorRGB(1, 1, 1));
        mesh->draw(*render_system, /* draw_edges = */ show_edges, /* use_vertex_data = */ false, /* send_colors = */ false);

        if (show_bbox)
        {
          render_system->setShader(NULL);
          render_system->setColor(ColorRGB(1, 1, 0));
          drawOutlineBox(mesh->getAABB());
        }

        if (highlighted_vertex)
        {
          render_system->setShader(NULL);
          render_system->setColor(ColorRGB(1, 0, 0));
          render_system->setPointSize(10);

          render_system->beginPrimitive(Graphics::RenderSystem::Primitive::POINTS);
            render_system->sendVertex(highlighted_vertex->getPosition());
          render_system->endPrimitive();
        }

      render_system->popShader();

    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::PROJECTION); render_system->popMatrix();
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::MODELVIEW); render_system->popMatrix();
  }

  glutSwapBuffers();
}

void
Viewer::reshape(int w, int h)
{
  width = w;
  height = h;
  glViewport(0, 0, width, height);

  glutPostRedisplay();
}

Matrix3
dragToRotation(int x1, int y1, int x2, int y2, int width, int height, Camera const & camera)
{
  static Real const ROT_SPEED = 5;

  // Pixel coordinates increase top to bottom, so dy is downwards in camera space
  int dx = x2 - x1;
  int dy = y2 - y1;
  if (dx == 0 && dy == 0)
    return Matrix3::identity();

  Vector3 axis = dy * camera.getRightDirection() + dx * camera.getUpDirection();

  int size = (width < height ? width : height);
  Real angle = -ROT_SPEED * Vector2(dx, -dy).length() / size;

  return Matrix3::rotationAxisAngle(axis, angle);
}

void
Viewer::keyPress(unsigned char key, int x, int y)
{
  if (key == 27)
  {
    exit(0);
  }
  else if (key == 'b' || key == 'B')
  {
    show_bbox = !show_bbox;
    glutPostRedisplay();
  }
  else if (key == 'e' || key == 'E')
  {
    show_edges = !show_edges;
    glutPostRedisplay();
  }
  else if (key == 'd' || key == 'd')
  {
    highlighted_vertex = mesh->decimateQuadricEdgeCollapse();
    glutPostRedisplay();
  }
}

void
Viewer::mousePress(int button, int state, int x, int y)
{
  dragging = (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN);

  last_x = x;
  last_y = y;
}

void
Viewer::mouseMotion(int x, int y)
{
  if (!dragging)
    return;

  Matrix3 inc_rot = dragToRotation(last_x, last_y, x, y, width, height, camera);
  rotation = inc_rot * rotation;

  last_x = x;
  last_y = y;

  glutPostRedisplay();
}

void
Viewer::drawOutlineBox(AxisAlignedBox3 const & bbox)
{
  if (!render_system)
    return;

  render_system->pushShader();
    render_system->setShader(NULL);

    render_system->beginPrimitive(Graphics::RenderSystem::Primitive::LINES);
      Vector3 p, q;
      for (int i = 0; i < 12; ++i)
      {
        bbox.getEdge(i, p, q);
        render_system->sendVertex(p);
        render_system->sendVertex(q);
      }
    render_system->endPrimitive();

  render_system->popShader();
}
