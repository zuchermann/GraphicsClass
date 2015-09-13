/*-----------------------------------------------------------------------*/
/*  CS-378                  Computer Graphics             Tom Ellman     */
/*-----------------------------------------------------------------------*/
/*                          Flat Picking Demo                            */
/*-----------------------------------------------------------------------*/
/* Original program by Edward Angel.                                     */
/* Modified by Tom Ellman.                                               */
/*-----------------------------------------------------------------------*/

#include <cstdlib>
#include <iostream>
#include <GL/glut.h>

using namespace std;

#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50
#define SELECT_BUFFER_SIZE 512
#define PICK_RANGE 10

int windowHeight = INITIAL_WIN_H;
int windowWidth = INITIAL_WIN_W;

void init()
{
   glClearColor (1.0, 1.0, 1.0, 0.0);
}

void drawObjects(GLenum mode)
{
    if(mode == GL_SELECT) glLoadName(1);
    glColor3f(1.0, 0.0, 0.0);
    glRectf(windowWidth/5.0, windowHeight/5.0, 3*windowWidth/5.0, 3*windowHeight/5.0);
    if(mode == GL_SELECT) glLoadName(2);
    glColor3f(0.0, 0.0, 1.0);
    glRectf(2*windowWidth/5.0, 2*windowHeight/5.0, 4*windowWidth/5.0, 4*windowHeight/5.0);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawObjects(GL_RENDER);
    glFlush();
}


void processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint names, *ptr;

   cout << "Number of Hits: " << hits << endl;
   ptr = (GLuint *) buffer;
   for (i = 0; i < hits; i++)
   {
      names = *ptr;
      ptr += 3;
      for (j = 0; j < names; j++)
      { 
         if (*ptr==1) cout << "Red Rectangle" << endl;
		 else cout << "Blue Rectangle" << endl;
         ptr++;
      }
   }
   cout << endl;
}



void mouse(int button, int state, int x, int y)
{
   GLint viewport[4];
   GLuint selectBuffer[SELECT_BUFFER_SIZE];
   GLint hits;

   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
   {
	   glGetIntegerv (GL_VIEWPORT, viewport);
	   glSelectBuffer (SELECT_BUFFER_SIZE, selectBuffer);
	   glRenderMode(GL_SELECT);
	   glInitNames();
	   glPushName(0);

	   glMatrixMode (GL_PROJECTION);
	   glPushMatrix ();
	   glLoadIdentity ();
	   gluPickMatrix ( x, (windowHeight - y), PICK_RANGE, PICK_RANGE, viewport);
	   gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
	   drawObjects(GL_SELECT);

	   glMatrixMode (GL_PROJECTION);
	   glPopMatrix ();
	   glFlush ();

	   hits = glRenderMode(GL_RENDER);
	   processHits (hits, selectBuffer);

	   glutPostRedisplay();
   }
}


void reshape(int w, int h)
// Callback for processing reshape events.
{
  windowHeight = h;
  windowWidth = w;
  glViewport(0,0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, w, 0.0, h);
}


void escExit( GLubyte key, int, int )
// Callback for processing keyboard events.
{ if ( key == 27 /* ESC */ ) std::exit( 0 ); }

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (INITIAL_WIN_W, INITIAL_WIN_H);
   glutInitWindowPosition (INITIAL_WIN_X, INITIAL_WIN_Y);
   glutCreateWindow ("Flat Picking Demo");

   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutMouseFunc (mouse);
   glutKeyboardFunc (escExit);

   init();

   glutMainLoop();
   return 0;
}
