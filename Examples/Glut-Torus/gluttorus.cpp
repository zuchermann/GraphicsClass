/*-------------------------------------------------*/
/*  CS-378       Computer Graphics     Tom Ellman  */
/*-------------------------------------------------*/
/*  gluttorus.cpp : Wire Frame Torus               */
/*-------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>

using namespace std;


#define BAND      0.25
#define ESTEP     0.25
#define TSTEP     10.0
#define RSTEP     5.00

#define RADIUS_STEP 0.01


#define XEYE_HOME 0.00
#define YEYE_HOME 0.00
#define ZEYE_HOME 1.00
#define THETA_HOME 115.0
#define INITIAL_SLICES 10
#define INITIAL_STACKS 10

#define TORUS_RADIUS1  .25
#define TORUS_RADIUS2  .5

float r1 = TORUS_RADIUS1;
float r2 = TORUS_RADIUS2;

typedef enum { ROTATION, POSITION, PROJECTION,
               FIELD, RADIUS, SLICES, STACKS, HOME } operationType;
typedef enum { X, Y, Z } axisType;
typedef enum { DOWN, UP } directionType;
typedef enum { ORTHOGRAPHIC, PERSPECTIVE } projectionType;
float xeye, yeye, zeye, theta;
float rx, ry, rz;

axisType axis = Z;
directionType direction = UP;
operationType operation = PROJECTION;
projectionType projection = ORTHOGRAPHIC;

int slices = INITIAL_SLICES;
int stacks = INITIAL_STACKS;
int windowHeight, windowWidth;

int radius = 1;


/*============================================================================*/

void homePosition( )

  /*--------------------------------------*/
  /* Rotate the torus to the home position */
  /* Move the "eye" to the home position  */
  /* Reset field of view angle            */
  /* Set the projection to orthographic   */
  /*--------------------------------------*/

{
  xeye = XEYE_HOME;
  yeye = YEYE_HOME;
  zeye = ZEYE_HOME;
  rx = 0.0;
  ry = 0.0;
  rz = 0.0;
  theta = THETA_HOME;
  projection = ORTHOGRAPHIC;
  direction = UP;
  axis = Z;
  r1 = TORUS_RADIUS1;
  r2 = TORUS_RADIUS2;
  slices = INITIAL_SLICES;
  stacks = INITIAL_STACKS;

}

/*============================================================================*/

void operate()
{
  switch (operation)
    {
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
    case RADIUS :     if (radius == 1) r1 += RADIUS_STEP * (direction==UP? 1 : -1);
                      else r2 += RADIUS_STEP * (direction==UP? 1 : -1);
                      break;
    case SLICES :     if (direction == UP) slices++;
                      else slices--;
                      break;
    case STACKS :     if (direction == UP) stacks++;
                      else stacks--;
                      break;
    case HOME       : homePosition();
    }

  glLoadIdentity( );
  gluLookAt( xeye, yeye, zeye, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glRotatef(rz, 0.0, 0.0, 1.0);
  glRotatef(ry, 0.0, 1.0, 0.0);
  glRotatef(rx, 1.0, 0.0, 0.0);


  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  if (projection == ORTHOGRAPHIC)
     glOrtho( -1.5, 1.5, -1.5, 1.5, -1.5, 1.5 );
  else gluPerspective(theta, 1.0, 0.1, 500.0 );
  glMatrixMode( GL_MODELVIEW );

  glutPostRedisplay( );

}



/*============================================================================*/

void drawTorus( )

{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   glColor3f (1.0, 1.0, 0.0);
   glutWireTorus(r1, r2, stacks, slices);


  glutSwapBuffers( );
  glFlush();

}

/*============================================================================*/


void Key( unsigned char key, int, int )

{
  switch ( key ) {

    case 'A'    : operation = ROTATION; direction = UP; axis = X; break;
    case 'a'    : operation = ROTATION; direction = DOWN; axis = X; break;
    case 'B'    : operation = ROTATION; direction = UP; axis = Y; break;
    case 'b'    : operation = ROTATION; direction = DOWN; axis = Y; break;
    case 'C'    : operation = ROTATION; direction = UP; axis = Z; break;
    case 'c'    : operation = ROTATION; direction = DOWN; axis = Z; break;

    case 'X'    : operation = POSITION; direction = UP; axis = X; break;
    case 'x'    : operation = POSITION; direction = DOWN; axis = X; break;
    case 'Y'    : operation = POSITION; direction = UP; axis = Y; break;
    case 'y'    : operation = POSITION; direction = DOWN; axis = Y; break;
    case 'Z'    : operation = POSITION; direction = UP; axis = Z; break;
    case 'z'    : operation = POSITION; direction = DOWN; axis = Z; break;

    case 'T'    : operation = FIELD; direction = UP; break;
    case 't'    : operation = FIELD; direction = DOWN; break;
    case 'O'    : operation = PROJECTION; projection = ORTHOGRAPHIC;
                  direction = UP; break;
    case 'o'    : operation = PROJECTION; projection = ORTHOGRAPHIC;
                  direction = UP; break;
    case 'P'    : operation = PROJECTION; projection = PERSPECTIVE;
                  direction = DOWN; break;
    case 'p'    : operation = PROJECTION; projection = PERSPECTIVE;
                  direction = DOWN; break;
    case 'h'    : operation = HOME; break;
    case '\033' : exit( 0 );
  default     : break;
  }
  operate();
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
 case 4 : operation = SLICES; break;
 case 5 : operation = STACKS; break;
 case 6 : operation = HOME; operate(); break;
 case 7 : exit( 0 );
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

void positionSubMenu( int item )
// Callback for processing position submenu.
{
  operation = POSITION;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void radiusSubMenu( int item )
// Callback for processing radius submenu.
{
  operation = RADIUS;
  switch ( item )
  {
    case 1 : radius = 1; break;
    case 2 : radius = 2; break;
  }
}

void setMenus( )
// Routine for creating menus.
{
  int rotationSubMenuCode, positionSubMenuCode, radiusSubMenuCode;

  rotationSubMenuCode = glutCreateMenu( rotationSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  positionSubMenuCode = glutCreateMenu( positionSubMenu );
  glutAddMenuEntry( "X Dimension",      1 );
  glutAddMenuEntry( "Y Dimension",      2 );
  glutAddMenuEntry( "Z Dimension",      3 );

  radiusSubMenuCode = glutCreateMenu( radiusSubMenu );
  glutAddMenuEntry( "Inner Radius",      1 );
  glutAddMenuEntry( "Outer Radius",      2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Rotate Torus ...",  rotationSubMenuCode );
  glutAddSubMenu( "Position Camera ...", positionSubMenuCode );
  glutAddMenuEntry( "Change Field of View",  1 );
  glutAddMenuEntry( "Change Projection",  2 );
  glutAddSubMenu( "Change Radius",  radiusSubMenuCode );
  glutAddMenuEntry( "Change Slices",  4 );
  glutAddMenuEntry( "Change Stacks",  5 );
  glutAddMenuEntry( "Home Position",  6 );
  glutAddMenuEntry( "Exit",  7 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}






/*============================================================================*/

int main( int argc, char** argv )

{
  // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);
  glutInit( &argc, argv );
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowSize( 400, 400 );
  glutInitWindowPosition( 200, 100 );
  glutCreateWindow( "Torus" );
  glutDisplayFunc( drawTorus );
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
  glOrtho( -1.5, 1.5, -1.5, 1.5, -1.5, 1.5 );
  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity( );
  gluLookAt( xeye, yeye, zeye, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  glutMainLoop( );
}

/*============================================================================*/
