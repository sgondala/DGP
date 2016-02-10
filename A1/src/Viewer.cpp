#include "Viewer.hpp"
#include "DGP/Graphics/RenderSystem.hpp"

#ifdef DGP_OSX
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

Graphics::RenderSystem * Viewer::render_system = NULL;
PointCloud * Viewer::pcloud = NULL;
int Viewer::width = 640;
int Viewer::height = 480;
Camera Viewer::camera;
Matrix3 Viewer::rotation = Matrix3::identity();
bool Viewer::dragging = false;
int Viewer::last_x = -1;
int Viewer::last_y = -1;
bool Viewer::show_normals = true;
bool Viewer::show_bbox = false;

void
Viewer::setObject(PointCloud * o)
{
  pcloud = o;
}

void
Viewer::launch(int argc, char * argv[])
{
  // Create a GL context via a GLUT window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("A1::Viewer");

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

  return Camera(cframe, Camera::ProjectionType::PERSPECTIVE, left / DIST, right / DIST, bottom / DIST, top / DIST,
                1.7f, (DIST + 1) * scale, Camera::ProjectedYDirection::UP);
}

void
Viewer::draw()
{
  alwaysAssertM(render_system, "Rendersystem not created");

  render_system->setColorClearValue(ColorRGB(0, 0, 0));
  render_system->clear();

  if (pcloud)
  {
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::MODELVIEW); render_system->pushMatrix();
    render_system->setMatrixMode(Graphics::RenderSystem::MatrixMode::PROJECTION); render_system->pushMatrix();

      camera = cameraFromBBox(pcloud->getAABB(), width, height, rotation);
      render_system->setCamera(camera);

      Real scale = pcloud->getAABB().getExtent().length();
      pcloud->draw(*render_system, (show_normals ? 0.05 * scale : -1));

      if (show_bbox)
      {
        render_system->setColor(ColorRGB(1, 1, 0));
        drawOutlineBox(pcloud->getAABB());
      }

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
  else if (key == 'n' || key == 'N')
  {
    show_normals = !show_normals;
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
