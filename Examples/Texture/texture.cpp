/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics        Tom Ellman  */
/*--------------------------------------------------------*/
/*                   Texture Mapping Demo                 */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>

using namespace std;

#define BAND      0.25
#define ESTEP     0.25
#define TSTEP     10.0
#define RSTEP     5.00

#define XEYE_HOME 0.00
#define YEYE_HOME 0.00
#define ZEYE_HOME 3.60
#define THETA_HOME 60.0

typedef enum { ROTATION, POSITION, PROJECTION, FIELD, HOME } operationType;
typedef enum { X, Y, Z } axisType;
typedef enum { DOWN, UP } directionType;
typedef enum { ORTHOGRAPHIC, PERSPECTIVE } projectionType;
float xeye, yeye, zeye, theta;
float rx, ry, rz;

axisType axis = Z;
directionType direction = UP;
operationType operation = PROJECTION;
projectionType projection = PERSPECTIVE;

int windowHeight, windowWidth;

#define FALSE 0
#define TRUE  1

int wallNotFence = TRUE;
int nearestNotLinear = TRUE;
int useMipMaps = FALSE;

/*============================================================================*/

void homePosition( )

  /*--------------------------------------*/
  /* Rotate the polygon to the home position */
  /* Move the "eye" to the home position  */
  /* Reset field of view angle            */
  /* Set the projection to projection   */
  /*--------------------------------------*/

{
  xeye = XEYE_HOME;
  yeye = YEYE_HOME;
  zeye = ZEYE_HOME;
  rx = 0.0;
  ry = 0.0;
  rz = 0.0;
  theta = THETA_HOME;
  projection = PERSPECTIVE;
  direction = UP;
  axis = Z;

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
    case HOME       : homePosition();
    }

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  gluLookAt( xeye, yeye, zeye, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
  glRotatef(rz, 0.0, 0.0, 1.0);
  glRotatef(ry, 0.0, 1.0, 0.0);
  glRotatef(rx, 1.0, 0.0, 0.0);


  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  if (projection == ORTHOGRAPHIC)
     glOrtho( -2.5, 2.5, -2.5, 2.5, -2.5, 2.5 );
  else gluPerspective(theta, 1.0, 1.0, 200.0 );

  glutPostRedisplay( );

}




/*============================================================================*/


void keyboard( unsigned char key, int, int)

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



void mouse( int button, int state, int, int)
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

/*	Create checkerboard texture	*/
#define	checkImageWidth 64
#define	checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;


void makeCheckImage()
{
   int i, j, c;

   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = (((((i&0x8)==0))^((j&0x8)==0)))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}



void init()
{

   glColor3f(1.0, 0.0, 0.0);
   glClearColor (0.0, 0.0, 0.0, 0.0);

   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   makeCheckImage();
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   if (nearestNotLinear)
     {
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       if (useMipMaps)
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
       else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

     }
   else
     {
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       if (useMipMaps)
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
       else glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     }
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
                0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
   if (useMipMaps)
     gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, checkImageWidth, checkImageHeight,
		       GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}



void filterSubMenu( int item )
// Callback for processing filter submenu.
{
  switch ( item )
  {
    case 1 : nearestNotLinear = TRUE; break;
    case 2 : nearestNotLinear = FALSE; break;
  }
  init();
  glutPostRedisplay();
}

void mipMapSubMenu( int item )
// Callback for processing filter submenu.
{
  switch ( item )
  {
    case 1 : useMipMaps = FALSE; break;
    case 2 : useMipMaps = TRUE; break;
  }
  init();
  glutPostRedisplay();
}

void polygonSubMenu( int item )
// Callback for processing polygon submenu.
{
  operation = HOME;
  switch ( item )
  {
    case 1 : wallNotFence = TRUE; break;
    case 2 : wallNotFence = FALSE; break;
  }
  operate();
  glutPostRedisplay();
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
  int rotationSubMenuCode, positionSubMenuCode, polygonSubMenuCode, filterSubMenuCode, mipMapSubMenuCode;

  polygonSubMenuCode = glutCreateMenu( polygonSubMenu );
  glutAddMenuEntry( "Wall",      1 );
  glutAddMenuEntry( "Fence",     2 );

  filterSubMenuCode = glutCreateMenu( filterSubMenu );
  glutAddMenuEntry( "Nearest",      1 );
  glutAddMenuEntry( "Linear",     2 );

  mipMapSubMenuCode = glutCreateMenu( mipMapSubMenu );
  glutAddMenuEntry( "No Mipmaps",      1 );
  glutAddMenuEntry( "Yes Mipmaps",     2 );

  rotationSubMenuCode = glutCreateMenu( rotationSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  positionSubMenuCode = glutCreateMenu( positionSubMenu );
  glutAddMenuEntry( "X Dimension",      1 );
  glutAddMenuEntry( "Y Dimension",      2 );
  glutAddMenuEntry( "Z Dimension",      3 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Wall or Fence ...",  polygonSubMenuCode );
  glutAddSubMenu( "Filtering ...",  filterSubMenuCode );
  glutAddSubMenu( "Mipmaps ...",  mipMapSubMenuCode );
  glutAddSubMenu( "Rotate Polygon ...",  rotationSubMenuCode );
  glutAddSubMenu( "Position Camera ...", positionSubMenuCode );
  glutAddMenuEntry( "Change Field of View",  1 );
  glutAddMenuEntry( "Change Projection",  2 );
  glutAddMenuEntry( "Home Position",  3 );
  glutAddMenuEntry( "Exit",  4 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}


void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_TEXTURE_2D);
   //   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   //   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D, texName);

   glBegin(GL_QUADS);

   if (wallNotFence)
     {
       glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
       glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, 0.0);
       glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, 0.0);
       glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, 0.0);
     }
   else {
          glTexCoord2f(0.0, -1.0);  glVertex3f(0.0,  -1.0, 1.0);
	  glTexCoord2f(0.0,  1.0);  glVertex3f(0.0,   1.0, 1.0);
	  glTexCoord2f(25.0, 1.0);  glVertex3f(-25.0, 1.0, 1.0);
	  glTexCoord2f(25.0,-1.0);  glVertex3f(-25.0,-1.0, 1.0);
        }

   glEnd();
   glFlush();
   glDisable(GL_TEXTURE_2D);

   glFlush();
}

/*============================================================================*/

int main( int argc, char** argv )

{
   // Mask floating point exceptions.
   _control87( MCW_EM, MCW_EM );
  glutInit( &argc, argv );
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Texture Mapping Demo");
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc( mouse );

  setMenus();

  homePosition( );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluPerspective(theta, 1.0, 1.0, 200.0 );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  gluLookAt( xeye, yeye, zeye, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

  glutMainLoop( );
}

/*============================================================================*/
