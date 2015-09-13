/*-----------------------------------------------------------------------*/
/*  CS-378                  Computer Graphics             Tom Ellman     */
/*-----------------------------------------------------------------------*/
/*                          Hierarchic Picking Demo                      */
/*-----------------------------------------------------------------------*/
/* Original program by Edward Angel.                                     */
/* Modified by Tom Ellman.                                               */
/*-----------------------------------------------------------------------*/

#include <cstdlib>
#include <iostream>
#include <GL/glut.h>
#include "scene-graph.h"
#include "house.h"

using namespace std;

#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50
#define SELECT_BUFFER_SIZE 512
#define PICK_RANGE 10

int windowHeight = INITIAL_WIN_H;
int windowWidth = INITIAL_WIN_W;

PlaceNode* sceneGraph;

void init()
{
   glClearColor(1.0, 1.0, 1.0, 0.0);

   locationStack.push(Location(0.0,0.0));

   sceneGraph = buildFancyHouse(100.0,100.0,400.0,
	                            RED,true,BLUE,true,GREEN,true,
								BLUE,false,GREEN,false,RED,false);
   //sceneGraph = buildHouse(100.0,100.0,400.0,RED,true,BLUE,true,GREEN,true);
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    sceneGraph->draw();
    glFlush();
}


void processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint names, *ptr;

   cout << "Number of Hits: " << hits << endl << endl;
   ptr = (GLuint *) buffer;
   for (i = 1; i <= hits; i++)
   {
	  cout << "Hit Number: " << i << endl;
      names = *ptr; 
	  ptr++;
	  cout << "Number of Names: " << names << endl;
	  cout << "Minimum Depth: " << *ptr << endl; 
	  ptr++;
	  cout << "Maximum Depth: " << *ptr << endl; 
	  ptr++;
      for (j = 0; j < names; j++)
      { 
		 cout << "Hit Name: " << *ptr << endl;
         ptr++;
      }
	  cout << endl;
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

	   glMatrixMode (GL_PROJECTION);
	   glPushMatrix ();
	   glLoadIdentity ();
	   gluPickMatrix ( x, (windowHeight - y), PICK_RANGE, PICK_RANGE, viewport);
	   gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);

	   sceneGraph->draw();

	   glMatrixMode (GL_PROJECTION);
	   glPopMatrix ();
	   glFlush ();

	   hits = glRenderMode(GL_RENDER);
	   processHits(hits, selectBuffer);

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
