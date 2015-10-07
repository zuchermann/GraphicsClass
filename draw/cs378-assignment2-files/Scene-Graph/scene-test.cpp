/*-----------------------------------------------------------------------*/
/*  CS-378                  Computer Graphics             Tom Ellman     */
/*-----------------------------------------------------------------------*/
/*                          Scene Graph Demo                             */
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
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    sceneGraph->draw();
    glFlush();
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
   glutCreateWindow ("Scene Graph Demo");

   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutKeyboardFunc (escExit);

   init();

   glutMainLoop();
   return 0;
}
