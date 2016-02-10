#ifndef __A1_Viewer_hpp__
#define __A1_Viewer_hpp__

#include "Common.hpp"
#include "PointCloud.hpp"
#include "DGP/AxisAlignedBox3.hpp"
#include "DGP/Camera.hpp"
#include "DGP/Matrix3.hpp"
#include "DGP/Graphics/RenderSystem.hpp"

/* Displays an object using OpenGL and GLUT. */
class Viewer
{
  private:
    static Graphics::RenderSystem * render_system;
    static PointCloud * pcloud;

    static int width;
    static int height;
    static Camera camera;
    static Matrix3 rotation;
    static bool dragging;
    static int last_x, last_y;
    static bool show_normals;
    static bool show_bbox;

  public:
    /** Set the object to be displayed. The object must persist as long as the viewer does. */
    static void setObject(PointCloud * o);

    /**
     * Call this function to launch the viewer. It will not return under normal circumstances, so make sure stuff is set up
     * before you call it!
     */
    static void launch(int argc, char * argv[]);

  private:
    /** Callback for drawing the object. */
    static void draw();

    /** Callback when window is resized. */
    static void reshape(int w, int h);

    /** Callback when a key is pressed. */
    static void keyPress(unsigned char key, int x, int y);

    /** Callback when a mouse button is pressed. */
    static void mousePress(int button, int state, int x, int y);

    /** Callback when the mouse moves with a button pressed. */
    static void mouseMotion(int x, int y);

    /** Draw a bounding box as an outline. */
    static void drawOutlineBox(AxisAlignedBox3 const & bbox);

}; // class Viewer

#endif
