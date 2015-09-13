/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*  Rotating Cube: Euler Angle Specification              */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>
#include <iostream>

#define XEYE 0.00
#define YEYE 0.00
#define ZEYE 3.00
#define RSTEP     5.00

#define BAND      0.25

using namespace std;

typedef enum { BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW } gColor;
typedef enum { ROTATION, HOME, NONE } operationType;
typedef enum { X, Y, Z } axisType;
typedef enum { DOWN, UP } directionType;
float rx, ry, rz;

axisType axis = Z;
directionType direction = UP;
operationType operation = NONE;

int windowHeight, windowWidth;

GLUquadricObj *cylinder;

/*============================================================================*/

void homePosition( )

  /*--------------------------------------*/
  /* Rotate the cube to the home position */
  /*--------------------------------------*/

{
  rx = 0.0;
  ry = 0.0;
  rz = 0.0;
}

/*============================================================================*/

void operate()
{
  switch (operation)
    {
    case NONE         : break;
    case ROTATION     :
                      switch (axis)
                        {
                          case X :
			           if (direction == UP) rx += RSTEP;
                                   else rx -= RSTEP;
                                   break;
                          case Y :
                                   if (direction == UP) ry += RSTEP;
                                   else ry -= RSTEP;
                                   break;
                          case Z :
                                   if (direction == UP) rz += RSTEP;
                                   else rz -= RSTEP;
                                   break;
			}
                      break;
    case HOME       : homePosition();
    }

  glutPostRedisplay( );

  cout << "Rx = " << rx << " Ry = " << ry << " Rz = " << rz << endl;


}


void gSetColor( gColor color )

{
  switch ( color ) {
    case BLACK   : glColor3f( 0.0, 0.0, 0.0 ); break;
    case BLUE    : glColor3f( 0.0, 0.0, 1.0 ); break;
    case CYAN    : glColor3f( 0.0, 1.0, 1.0 ); break;
    case GREEN   : glColor3f( 0.0, 1.0, 0.0 ); break;
    case MAGENTA : glColor3f( 1.0, 0.0, 1.0 ); break;
    case RED     : glColor3f( 1.0, 0.0, 0.0 ); break;
    case WHITE   : glColor3f( 1.0, 1.0, 1.0 ); break;
    case YELLOW  : glColor3f( 1.0, 1.0, 0.0 ); break; } }



/*============================================================================*/

void drawFace( float x1, float y1, float z1,
               float x2, float y2, float z2,
               gColor color )

  /*----------------------------------------------------------*/
  /* Draw a hollow rectangular face, with opposite corners at */
  /* ( x1, y1, z1 ) and ( x2, y2, z2 ),  and in color 'color' */
  /*----------------------------------------------------------*/

{ gSetColor( color );

  if      ( x1 == x2 ) {
    glBegin( GL_POLYGON );
    glVertex3f( x1,        y1,        z1        );
    glVertex3f( x1,        y1 + BAND, z1        );
    glVertex3f( x1,        y1 + BAND, z2        );
    glVertex3f( x1,        y1,        z2        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x1,        y2,        z2        );
    glVertex3f( x1,        y2 - BAND, z2        );
    glVertex3f( x1,        y2 - BAND, z1        );
    glVertex3f( x1,        y2,        z1        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x1,        y1 + BAND, z1        );
    glVertex3f( x1,        y2 - BAND, z1        );
    glVertex3f( x1,        y2 - BAND, z1 + BAND );
    glVertex3f( x1,        y1 + BAND, z1 + BAND );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x1,        y2 - BAND, z2        );
    glVertex3f( x1,        y1 + BAND, z2        );
    glVertex3f( x1,        y1 + BAND, z2 - BAND );
    glVertex3f( x1,        y2 - BAND, z2 - BAND );
    glEnd( ); }
  else if ( y1 == y2 ) {
    glBegin( GL_POLYGON );
    glVertex3f( x1,        y1,        z1        );
    glVertex3f( x1 + BAND, y1,        z1        );
    glVertex3f( x1 + BAND, y1,        z2        );
    glVertex3f( x1,        y1,        z2        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x2,        y1,        z2        );
    glVertex3f( x2 - BAND, y1,        z2        );
    glVertex3f( x2 - BAND, y1,        z1        );
    glVertex3f( x2,        y1,        z1        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x1 + BAND, y1,        z1        );
    glVertex3f( x2 - BAND, y1,        z1        );
    glVertex3f( x2 - BAND, y1,        z1 + BAND );
    glVertex3f( x1 + BAND, y1,        z1 + BAND );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x2 - BAND, y1,        z2        );
    glVertex3f( x1 + BAND, y1,        z2        );
    glVertex3f( x1 + BAND, y1,        z2 - BAND );
    glVertex3f( x2 - BAND, y1,        z2 - BAND );
    glEnd( ); }
  else if ( z1 == z2 ) {
    glBegin( GL_POLYGON );
    glVertex3f( x1,        y1,        z1        );
    glVertex3f( x1 + BAND, y1,        z1        );
    glVertex3f( x1 + BAND, y2,        z1        );
    glVertex3f( x1,        y2,        z1        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x2,        y2,        z1        );
    glVertex3f( x2 - BAND, y2,        z1        );
    glVertex3f( x2 - BAND, y1,        z1        );
    glVertex3f( x2,        y1,        z1        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x1 + BAND, y1,        z1        );
    glVertex3f( x2 - BAND, y1,        z1        );
    glVertex3f( x2 - BAND, y1 + BAND, z1        );
    glVertex3f( x1 + BAND, y1 + BAND, z1        );
    glEnd( );
    glBegin( GL_POLYGON );
    glVertex3f( x2 - BAND, y2,        z1        );
    glVertex3f( x1 + BAND, y2,        z1        );
    glVertex3f( x1 + BAND, y2 - BAND, z1        );
    glVertex3f( x2 - BAND, y2 - BAND, z1        );
    glEnd( ); } }

/*============================================================================*/

void drawCube( )
{

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluPerspective(115.0, 1.0, 0.8, 100.0 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  gluLookAt( XEYE,YEYE,ZEYE, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glRotatef(rx, 1.0, 0.0, 0.0);
  glRotatef(ry, 0.0, 1.0, 0.0);
  glRotatef(rz, 0.0, 0.0, 1.0);

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  drawFace( -1.0, -1.0,  1.0,  1.0,  1.0,  1.0, BLUE     );  /* front  */
  drawFace( -1.0, -1.0, -1.0,  1.0,  1.0, -1.0, BLUE );  /* back   */
  drawFace( -1.0,  1.0, -1.0,  1.0,  1.0,  1.0, GREEN   );  /* top    */
  drawFace( -1.0, -1.0, -1.0,  1.0, -1.0,  1.0, GREEN    );  /* bottom */
  drawFace( -1.0, -1.0, -1.0, -1.0,  1.0,  1.0, RED  );  /* left   */
  drawFace(  1.0, -1.0, -1.0,  1.0,  1.0,  1.0, RED    );  /* right  */

  glutSwapBuffers( );
  glFlush();
}

/*============================================================================*/


void Key( unsigned char key, int, int )
{
   if (key=='\033') std::exit( 0 );
}

/*============================================================================*/

void reshape(int w, int h)
// Callback for processing reshape events.
{
  windowWidth = w;
  windowHeight = h;
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
}




/*============================================================================*/



void mouse( int button, int state, int, int )
// Routine for processing mouse events.
{
   if ( button == GLUT_LEFT_BUTTON )
    switch ( state )
    {
    case GLUT_DOWN : direction = DOWN; operate(); break;
    case GLUT_UP   : break;
    }
   else if ( button == GLUT_RIGHT_BUTTON )
    switch ( state )
    {
    case GLUT_DOWN : direction = UP; operate(); break;
    case GLUT_UP   : break;
    }
}



/*============================================================================*/



void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 1 : operation = HOME; operate(); break;
 case 2 : std::exit( 0 );
 }
}


void rotationSubMenu( int item )
// Callback for processing rotation submenu.
{
  operation = ROTATION;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void setMenus( )
// Routine for creating menus.
{
  int rotationSubMenuCode;

  rotationSubMenuCode = glutCreateMenu( rotationSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Rotate Cube ...",  rotationSubMenuCode );
  glutAddMenuEntry( "Home Position",  1 );
  glutAddMenuEntry( "Exit",  2 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}






/*============================================================================*/

int main( int argc, char** argv )

{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowSize( 600, 600 );
  glutInitWindowPosition( 200, 100 );
  glutCreateWindow( "Rotating Cube in Euler Parameters" );
  glutDisplayFunc( drawCube );
  glutReshapeFunc(reshape);
  glutKeyboardFunc( Key );
  glutMouseFunc( mouse );

  setMenus();

  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glColor3f( 0.0, 0.0, 0.0 );

  glEnable( GL_DEPTH_TEST );

  homePosition( );

  cylinder = gluNewQuadric();
  gluQuadricDrawStyle(cylinder,(GLenum)GLU_FILL);

  glutMainLoop( );

  return 0;
}

/*============================================================================*/
