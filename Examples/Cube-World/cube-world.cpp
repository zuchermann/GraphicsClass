/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*  Cube Rotating in World Coordinates                    */
/*--------------------------------------------------------*/


#include <GL/glut.h>
#include <cfloat>

#define XEYE 0.00
#define YEYE 0.00
#define ZEYE 3.00
#define TSTEP     0.1
#define RSTEP     5.00
#define SFACTOR   1.1

#define BAND      0.25

using namespace std;

typedef enum { BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW } gColor;
typedef enum { TRANSLATION, ROTATION, SCALING, HOME, NONE } operationType;
typedef enum { X, Y, Z } axisType;
typedef enum { DOWN, UP } directionType;

axisType axis = Z;
directionType direction = UP;
operationType operation = NONE;

int windowHeight, windowWidth;

GLdouble modelViewMatrix[16];


/*============================================================================*/

void homePosition( )
{
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluPerspective(115.0, 1.0, 0.8, 100.0 );
}

/*============================================================================*/

void rotate(axisType axis)
{
   switch (axis)
   {
      case X : if (direction == UP)
	 glRotatef(RSTEP,1.0,0.0,0.0);
      else glRotatef(-RSTEP,1.0,0.0,0.0);
      break;
      case Y : if (direction == UP)
	 glRotatef(RSTEP,0.0,1.0,0.0);
      else glRotatef(-RSTEP,0.0,1.0,0.0);
      break;
      case Z : if (direction == UP)
	 glRotatef(RSTEP,0.0,0.0,1.0);
      else glRotatef(-RSTEP,0.0,0.0,1.0);
      break;
   }
}


void translate(axisType axis)
{
  switch (axis)
  {
     case X : if (direction == UP)
	glTranslatef(TSTEP,0.0,0.0);
     else glTranslatef(-TSTEP,0.0,0.0);
     break;
     case Y : if (direction == UP)
	glTranslatef(0.0,TSTEP,0.0);
     else glTranslatef(0.0,-TSTEP,0.0);
     break;
     case Z : if (direction == UP)
	glTranslatef(0.0,0.0,TSTEP);
     else glTranslatef(0.0,0.0,-TSTEP);
     break;
  }
}

void scale(axisType axis)
{
   switch (axis)
   {
      case X : if (direction == UP)
	 glScalef(SFACTOR,1.0,1.0);
      else glScalef(1/SFACTOR,1.0,1.0);
      break;
      case Y : if (direction == UP)
	 glScalef(1.0,SFACTOR,1.0);
      else glScalef(1.0,1/SFACTOR,1.0);
      break;
      case Z : if (direction == UP)
	 glScalef(1.0,1.0,SFACTOR);
      else glScalef(1.0,1.0,1/SFACTOR);
      break;
   }
}



void operate()
{
  switch (operation)
    {
    case NONE         : break;
    case TRANSLATION  :
                      glMatrixMode( GL_MODELVIEW );
		      glLoadIdentity();
		      translate(axis);
		      glMultMatrixd(modelViewMatrix);
		      glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);
		      break;
    case ROTATION     :
                      glMatrixMode( GL_MODELVIEW );
		      glLoadIdentity();
		      rotate(axis);
		      glMultMatrixd(modelViewMatrix);
		      glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);
                      break;
    case SCALING     :
                      glMatrixMode( GL_MODELVIEW );
		      glLoadIdentity();
		      scale(axis);
		      glMultMatrixd(modelViewMatrix);
		      glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);
                      break;
    case HOME       : homePosition();
    }

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
{

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  gluLookAt( XEYE,YEYE,ZEYE, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glMultMatrixd(modelViewMatrix);

  /*----------------------------------------------*/
  /* Draw a 2 x 2 x 2 cube, centered at (0, 0, 0) */
  /*----------------------------------------------*/

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
 case 1 : operation = HOME; operate(); break;
 case 2 : exit( 0 );
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

void scalingSubMenu( int item )
// Callback for processing rotation submenu.
{
  operation = SCALING;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void translationSubMenu( int item )
// Callback for processing rotation submenu.
{
  operation = TRANSLATION;
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
  int translationSubMenuCode;
  int rotationSubMenuCode;
  int scalingSubMenuCode;

  rotationSubMenuCode = glutCreateMenu( rotationSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  scalingSubMenuCode = glutCreateMenu( scalingSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  translationSubMenuCode = glutCreateMenu( translationSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Translate Cube ...",  translationSubMenuCode );
  glutAddSubMenu( "Rotate Cube ...",  rotationSubMenuCode );
  glutAddSubMenu( "Scale Cube ...",  scalingSubMenuCode );
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
  glutCreateWindow( "Transforming Cube in World Coordinates" );
  glutDisplayFunc( drawCube );
  glutReshapeFunc(reshape);
  glutKeyboardFunc( Key );
  glutMouseFunc( mouse );

  setMenus();

  glClearColor( 0.0, 0.0, 0.0, 0.0 );
  glColor3f( 0.0, 0.0, 0.0 );

  glEnable( GL_DEPTH_TEST );

  homePosition( );

  glutMainLoop( );

  return 0;
}

/*============================================================================*/
