/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*  Demo of Perspective v. Orthographic Projection        */
/*--------------------------------------------------------*/


#include <GL/glut.h>
#include <cfloat>

#define BAND      0.25
#define ESTEP     0.25
#define TSTEP     10.0

#define XEYE_HOME 0.00
#define YEYE_HOME 0.00
#define ZEYE_HOME 3.00
#define THETA_HOME 125.0

using namespace std;

typedef enum { BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW } gColor;
typedef enum { POSITION, PROJECTION, FIELD, HOME } operationType;
typedef enum { X, Y, Z } axisType;
typedef enum { DOWN, UP } directionType;
typedef enum { ORTHOGRAPHIC, PERSPECTIVE } projectionType;
float xeye, yeye, zeye, theta;

axisType axis = Z;
directionType direction = UP;
operationType operation = PROJECTION;
projectionType projection = ORTHOGRAPHIC;

int windowHeight, windowWidth;


/*============================================================================*/

void homePosition( )

  /*--------------------------------------*/
  /* Move the "eye" to the home position  */
  /* Reset field of view angle            */
  /* Set the projection to orthographic   */
  /*--------------------------------------*/

{
  xeye = XEYE_HOME;
  yeye = YEYE_HOME;
  zeye = ZEYE_HOME;
  theta = THETA_HOME;
  projection = ORTHOGRAPHIC;
  direction = UP;
  axis = Z;
}

/*============================================================================*/

void operate()
{
  switch (operation)
    {
    case POSITION   :
                      switch (axis)
                        {
                          case X :
                                   if (direction == UP) xeye += ESTEP;
                                   else xeye -= ESTEP;
                                   break;
                          case Y :
                                   if (direction == UP) yeye += ESTEP;
                                   else yeye -= ESTEP;
                                   break;
                          case Z :
                                   if (direction == UP) zeye += ESTEP;
                                   else zeye -= ESTEP;
                                   break;
			}
                      break;
    case FIELD      : if (direction == UP) theta += TSTEP;
                      else theta -= TSTEP;
                      break;
    case PROJECTION : if (direction == UP) projection = ORTHOGRAPHIC;
                      else projection = PERSPECTIVE;
                      break;
    case HOME       : homePosition();
    }

  glLoadIdentity( );
  gluLookAt( xeye, yeye, zeye, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  if (projection == ORTHOGRAPHIC)
     glOrtho( -4.0, 4.0, -4.0, 4.0, -8.0, 8.0 );
  else gluPerspective(theta, 1.0, 0.8, 100.0 );
  glMatrixMode( GL_MODELVIEW );

  glutPostRedisplay( );

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

  /*----------------------------------------------*/
  /* Draw a 2 x 2 x 2 cube, centered at (0, 0, 0) */
  /*----------------------------------------------*/

{
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
   if (key=='\033') exit( 0 );
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
 case 1 : operation = FIELD; break;
 case 2 : operation = PROJECTION; break;
 case 3 : operation = HOME; operate(); break;
 case 4 : exit( 0 );
 }
}


void positionSubMenu( int item )
// Callback for processing rotation submenu.
{
  operation = POSITION;
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
  int positionSubMenuCode;

  positionSubMenuCode = glutCreateMenu( positionSubMenu );
  glutAddMenuEntry( "X Dimension",      1 );
  glutAddMenuEntry( "Y Dimension",      2 );
  glutAddMenuEntry( "Z Dimension",      3 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Position Camera ...", positionSubMenuCode );
  glutAddMenuEntry( "Change Field of View",  1 );
  glutAddMenuEntry( "Change Projection",  2 );
  glutAddMenuEntry( "Home Position",  3 );
  glutAddMenuEntry( "Exit",  4 );
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
  glutCreateWindow( "Cube" );
  glutDisplayFunc( drawCube );
  glutReshapeFunc(reshape);
  glutKeyboardFunc( Key );
  glutMouseFunc( mouse );

  setMenus();

  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glColor3f( 0.0, 0.0, 0.0 );

  glEnable( GL_DEPTH_TEST );

  homePosition( );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  glOrtho( -4.0, 4.0, -4.0, 4.0, -8.0, 8.0 );
  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity( );
  gluLookAt( xeye, yeye, zeye, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  glutMainLoop( );
  return 0;
}

/*============================================================================*/
