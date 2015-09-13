/*-----------------------------------------------------------------------*/
/*  CS-378                  Computer Graphics               Tom Ellman   */
/*-----------------------------------------------------------------------*/
/*                         Rotating Rectangles Program.                  */
/*-----------------------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cfloat>
#include <cassert>
#include <iostream>

using namespace std;

#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50
#define ANCHORCURSORSIZE 10
#define DELAY 0


// Variables for keeping track of the rectangle.
int rectLeft = 300;
int rectRight = 500;
int rectBottom = 300;
int rectTop = 500;
float rectTheta;
float rectCenterX, rectCenterY;
float rectHalfWidth, rectHalfHeight;

// Variable for indicating user is in drawing mode.
bool drawing = false;
// Variable for inidicating animation is running.
bool animating = false;

// Update the rectangle parameters for a new rectangle.
void updateRectangle()
{
  rectTheta = 0.0;
  rectCenterX = (rectLeft + rectRight)/2.0;
  rectCenterY = (rectBottom + rectTop)/2.0;
  rectHalfWidth = (rectRight - rectLeft)/2.0;
  rectHalfHeight = (rectTop - rectBottom)/2.0;
}

// Data structure for RGB triples.
class rgbType
{
 public:
  rgbType() : red(0.0), green(0.0), blue(0.0)  {}
  rgbType(double r, double g, double b) : red(r), green(g), blue(b)  {}
  double red, green, blue;
};

// Keeping track of the start and end points of the rubber band.
int xAnchor, yAnchor, xStretch, yStretch;

// Keeping track of the screen window dimensions.
int windowHeight, windowWidth;

void blank( )
// Callback for clearing the normal screen window.
{
    glClear( GL_COLOR_BUFFER_BIT );
    glFlush();
}

void spinDisplay()
{
  clock_t t=clock();
  do {} while( (float)(clock()-t)/CLOCKS_PER_SEC < DELAY);

  rectTheta += 1.0;
  if (rectTheta > 360.0) rectTheta -= 360.0;
  glutPostRedisplay();
}

void drawRectangle()
// Callback for drawing the rectangle at its current orientation.
{
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix();
  glTranslatef(rectCenterX,rectCenterY,0.0);
  glRotatef(rectTheta, 0.0, 0.0, 1.0);
  glColor3f(1.0, 1.0, 0.0);
  glRectf(-rectHalfWidth, -rectHalfHeight, rectHalfWidth, rectHalfHeight);
  glPopMatrix();

  glutSwapBuffers();
  glFlush();
}


void reshape(int w, int h)
// Callback for processing reshape events.
{
  windowWidth = w;
  windowHeight = h;

  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);

}

void rubberBand(int x, int y)
// Callback for processing mouse motion.
{
   if (!drawing) return;
   glClear(GL_COLOR_BUFFER_BIT);
   y = windowHeight - y;
   xStretch = x;
   yStretch = y;
   glBegin(GL_LINE_LOOP);
   glVertex2i( xAnchor, yAnchor);
   glVertex2i( xStretch, yAnchor);
   glVertex2i( xStretch, yStretch);
   glVertex2i( xAnchor, yStretch);
   glEnd();
   glFlush();
   glutSwapBuffers();
}



void escExit( GLubyte key, int, int )
// Callback for processing keyboard events.
{ if ( key == 27 /* ESC */ ) std::exit( 0 ); }

void mouse( int, int state, int x, int y )
// Routine for processing mouse events.
{
   if (animating)
   {
      glutIdleFunc(NULL);
      animating = false;
   }
   else if (drawing)
   {
   y = windowHeight - y;
   switch ( state )
    {
     case GLUT_DOWN : rectLeft = x;
	              rectBottom = y;
		      xAnchor = x;
		      yAnchor = y;
		      break;
     case GLUT_UP   : rectRight = x;
	              rectTop = y;
		      updateRectangle();
		      glutPostRedisplay();
		      break;
    }
   }

}

void mainMenu( int item )
// Callback for processing main menu.
{
  switch ( item )
  {
    case 0 : drawing = true;
             animating = false;
             glutIdleFunc(NULL);
             break;
    case 1 : animating = true;
             drawing = false;
             glutIdleFunc(spinDisplay);
             break;
    case 2 : std::exit(0);
  }
}

void setMenu( )
// Routine for creating menus.
{
  glutCreateMenu( mainMenu );
  glutAddMenuEntry( "Draw Rectangle", 0 );
  glutAddMenuEntry( "Animate", 1 );
  glutAddMenuEntry( "Exit", 2 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

int main(int argc, char** argv)

{
  // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

  // Initialize glut with command line parameters.
  glutInit( &argc, argv );

  // Choose RGB display mode and double buffering.
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

  // Set initial window size, position, and title.
  glutInitWindowSize( INITIAL_WIN_W, INITIAL_WIN_H );
  glutInitWindowPosition( INITIAL_WIN_X, INITIAL_WIN_Y );
  windowWidth = INITIAL_WIN_W;
  windowHeight = INITIAL_WIN_H;
  glutCreateWindow("Rotating Squares");

  // Set up OpenGL for 2d grahics.
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluOrtho2D( 0.0, (double) INITIAL_WIN_W, 0.0, (double) INITIAL_WIN_H );
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  // Use the standard 2d hack:
  glTranslatef( 0.375, 0.375, 0.0 );

  // Set the color for clearing the normal screen window.
  glClearColor( 0.0, 0.0, 0.0, 0.0 );

  // Set the callbacks for the normal screen window.
  glutDisplayFunc( drawRectangle );
  glutMouseFunc( mouse );
  glutMotionFunc( rubberBand );
  glutReshapeFunc( reshape );
  glutKeyboardFunc( escExit );
  glutIdleFunc(NULL);

  // Set up the menus.
  setMenu();

  // Initialize the rectangle parameters.
  updateRectangle();

  // Start the GLUT main loop.
  glutMainLoop( );

}
