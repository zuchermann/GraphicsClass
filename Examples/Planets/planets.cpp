/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*  planets.cpp : Planetary System Simulation Program      */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <cfloat>

using namespace std;

// Constants defining the sizes, orbits and angular
// velocities of the heavenly bodies.
#define ORBIT_SLICES 360
#define ORBIT_RINGS 10
#define SUN_RADIUS 0.5
#define EARTH_RADIUS 0.1
#define MOON1_RADIUS 0.075
#define MOON2_RADIUS 0.075
#define EARTH_ORBIT_RADIUS 2.0
#define MOON1_ORBIT_RADIUS 0.8
#define MOON2_ORBIT_RADIUS 0.4
#define SUN_ROTATION_STEP 90/44
#define EARTH_REVOLUTION_STEP 90/45
#define EARTH_ROTATION_STEP 90/18
#define MOON1_REVOLUTION_STEP 90/18
#define MOON1_ROTATION_STEP 90/9
#define MOON2_REVOLUTION_STEP 90/9
#define MOON2_ROTATION_STEP 90/9
#define PI 3.14159

//Constants defining the step increments for changing
//the location and the aim of the camera.
#define EYE_STEP 0.1
#define CEN_STEP 0.1
#define ZOOM_FACTOR 8.0

//Global variables that represent the state of the planetary
//system. Rotation variables change as the object spins on its
//axis. Revolution variables change as the object moves in its
//circular orbit.
int sunRotation = 0;
int earthRevolution = 0, earthRotation = 0;
int moon1Revolution = 0, moon1Rotation = 0;
int moon2Revolution = 180, moon2Rotation = 0;

//Global variables that represent the position of each body
//in cartesian coordinates.
GLdouble sunX, sunY, sunZ;
GLdouble earthX, earthY, earthZ;
GLdouble moon1X, moon1Y, moon1Z;
GLdouble moon2X, moon2Y, moon2Z;

//Global variables for keeping track of the size and shape
//of the window.
int windowHeight, windowWidth;

//Enumerated type and global variable for keeping track
//of the selected operation.
typedef enum { POSITION, AIM, ORIENTATION, ZOOM, HOME, TIME, PROJECTION,
               ANIMATE, NOTHING} operationType;
operationType operation = POSITION;

//Enumerated type and global variable for keeping track
//of the object the user has selected to track with the camera
//and the object the user has selected for the camera to ride.
typedef enum { SUN, EARTH, MOON1, MOON2, NONE } trackingType;
trackingType tracking = NONE;
trackingType riding = NONE;

//Enumerated type and global variable for talking about axies.
typedef enum { X, Y, Z } axisType;
axisType axis = Z;

//Enumerated type and global variable for talking about
//direction of changes in camera position, and aim
//and the direction of time steps and animation.
typedef enum { DOWN, UP } directionType;
directionType direction = UP;

//Enumerated type and global variable for keeping track of
//the type of projection being used.
typedef enum { ORTHOGRAPHIC, PERSPECTIVE } projectionType;
projectionType  projection = PERSPECTIVE;

//Enumerated type and global variable for deciding how to
//display bodies.
typedef enum { WIRE, SOLID } sphereType;
sphereType sphere = WIRE;

//Global variables for keeping track of the camera position.
GLdouble xEye = 0.0;
GLdouble yEye = 0.0;
GLdouble zEye = 5.0;

//Global variables for keeping track of the camera aim.
GLdouble xCen = 0.0;
GLdouble yCen = 0.0;
GLdouble zCen = 0.0;

//Global variables for keeping track of the camera orientation.
GLdouble xUp = 0.0;
GLdouble yUp = 1.0;
GLdouble zUp = 0.0;

void multiplyMatrixVector(GLdouble* m, GLdouble* v, GLdouble* w, int size)
//Multiplying a vector by a matrix.
{
  int i,j;
  for (i=0; i<size; i++)
     {
       GLdouble temp = 0.0;
       for (j=0; j<size; j++)
	 temp += (*(m + i + j*size)) * (*(v + j));
       *(w + i) = temp;
     }
}

void getCurrentLocation (GLdouble* x, GLdouble* y, GLdouble* z)
//Finding the location in world coordinates to which the current
//modelview matrix maps the origin.
{
  GLdouble modelviewMatrix[ 16 ];
  GLdouble v[4] = {0.0,0.0,0.0,1.0};
  GLdouble w[4];
  glGetDoublev(  GL_MODELVIEW_MATRIX, modelviewMatrix );
  multiplyMatrixVector(modelviewMatrix,v,w,4);
  *x = w[0]/w[3];
  *y = w[1]/w[3];
  *z = w[2]/w[3];
}


//Initialize the display window.
void init()
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}


//Draw a circle in the z=0 plane the slow but easy way.
void drawCircle(GLdouble radius)
{
  GLdouble theta, delta;
  delta = 2*PI/ORBIT_SLICES;
  glBegin(GL_LINE_LOOP);
    for (theta = 0; theta < 2*PI; theta += delta)
        glVertex3f(radius*cos(theta),radius*sin(theta),0.0);
  glEnd();
}

void drawMoon2()
{
   glRotatef ((GLdouble) -moon2Rotation, 0.0, 0.0, 1.0);
   if (sphere == WIRE)
      glutWireSphere(MOON2_RADIUS, 10, 8);
   else glutSolidSphere(MOON2_RADIUS, 10, 8);
}

void gotoMoon2Coordinates()
{
   glRotatef ((GLdouble) moon2Revolution, 0.0, 0.0, 1.0);
   glTranslatef (MOON2_ORBIT_RADIUS, 0.0, 0.0);
   glRotatef ((GLdouble) -moon2Revolution, 0.0, 0.0, 1.0);
}

void drawMoon2System()
{
   glPushMatrix();
   gotoMoon2Coordinates();
   drawMoon2();
   glPopMatrix();
}


void drawMoon2Orbit()
{
   glColor3f (1.0, 0.0, 0.0);
   drawCircle(MOON2_ORBIT_RADIUS);
}


void drawMoon1()
{
   glRotatef ((GLdouble) -moon1Rotation, 0.0, 0.0, 1.0);
   if (sphere == WIRE)
      glutWireSphere(MOON1_RADIUS, 10, 8);
   else glutSolidSphere(MOON1_RADIUS, 10, 8);
}



void gotoMoon1Coordinates()
{
   glRotatef ((GLdouble) moon1Revolution, 0.0, 0.0, 1.0);
   glTranslatef (MOON1_ORBIT_RADIUS, 0.0, 0.0);
   glRotatef ((GLdouble) -moon1Revolution, 0.0, 0.0, 1.0);
}


void drawMoon1System()
{
   glPushMatrix();
   gotoMoon1Coordinates();
   drawMoon1();
   glPopMatrix();
}


void drawMoon1Orbit()
{
   glColor3f (0.0, 1.0, 0.0);
   drawCircle(MOON1_ORBIT_RADIUS);
}


void drawEarth()
{
   glPushMatrix();
   glRotatef ((GLdouble) earthRotation, 0.0, 0.0, 1.0);
   if (sphere == WIRE)
      glutWireSphere(EARTH_RADIUS, 10, 8);
   else glutSolidSphere(EARTH_RADIUS, 10, 8);
   glPopMatrix();
}


void gotoEarthCoordinates()
{
   glRotatef ((GLdouble) earthRevolution, 0.0, 0.0, 1.0);
   glTranslatef (EARTH_ORBIT_RADIUS, 0.0, 0.0);
   glRotatef ((GLdouble) -earthRevolution, 0.0, 0.0, 1.0);
}


void drawEarthSystem()
{
   glPushMatrix();
   gotoEarthCoordinates();
   drawEarth();
   drawMoon1Orbit();
   drawMoon1System();
   drawMoon2Orbit();
   drawMoon2System();
   glPopMatrix();
}


void drawEarthOrbit()
{
   glColor3f (0.0, 0.0, 1.0);
   drawCircle(EARTH_ORBIT_RADIUS);
}


void drawSun()
{
   glPushMatrix();
   glColor3f (1.0, 1.0, 0.0);
   glRotatef((GLdouble) sunRotation, 0.0, 0.0, 1.0);
   if (sphere == WIRE)
      glutWireSphere(SUN_RADIUS, 20, 16);
   else glutSolidSphere(SUN_RADIUS, 20, 16);
   glPopMatrix();
}

void getMoon2Position(GLdouble* x, GLdouble* y, GLdouble* z)
//Finding the current position of the moon2.
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gotoEarthCoordinates();
  gotoMoon2Coordinates();
  getCurrentLocation(x,y,z);
  glPopMatrix();
}


void getMoon1Position(GLdouble* x, GLdouble* y, GLdouble* z)
//Finding the current position of the moon1.
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gotoEarthCoordinates();
  gotoMoon1Coordinates();
  getCurrentLocation(x,y,z);
  glPopMatrix();
}

void getEarthPosition(GLdouble* x, GLdouble* y, GLdouble* z)
//Finding the current position of the earth.
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gotoEarthCoordinates();
  getCurrentLocation(x,y,z);
  glPopMatrix();
}

void getSunPosition(GLdouble* x, GLdouble* y, GLdouble* z)
//Finding the current position of the sun.
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  getCurrentLocation(x,y,z);
  glPopMatrix();
}

void drawSolarSystem()
{
   drawSun();
   drawEarthOrbit();
   drawEarthSystem();
}


void display()
//Callback for redisplaying the image.
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //Set the camera position, aim and orientation.
   glLoadIdentity();
   gluLookAt (xEye, yEye, zEye, xCen, yCen, zCen, xUp, yUp, zUp);

   //Set the projection type and clipping planes.
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (projection == ORTHOGRAPHIC)
      glOrtho( -3.0, 3.0, -3.0, 3.0, -50.0, 50.0 );
   else  gluPerspective(60.0, (GLdouble) windowWidth / (GLdouble) windowHeight,
                        0.1, 200.0);
   glMatrixMode(GL_MODELVIEW);

   //Draw the solar system.
   drawSolarSystem();

   glutSwapBuffers();

   glFlush();
}

void reshape (int w, int h)
//Callback for responding to reshaping of the display window.
{
   windowWidth = w;
   windowHeight = h;

   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   if (projection == ORTHOGRAPHIC)
      glOrtho( -3.0, 3.0, -3.0, 3.0, -3.0, 3.0 );
   else  gluPerspective(60.0, (GLdouble) w / (GLdouble) h, 1.0, 200.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void homePosition()
//Putting the solar system and all viewing varibles back to their
//original states.
{
 axis = Z;
 direction = UP;
 operation = POSITION;
 tracking = NONE;
 riding = NONE;
 sunRotation = 0;
 earthRevolution = 0;
 earthRotation = 0;
 moon1Revolution = 0;
 moon1Rotation = 0;
 moon2Revolution = 180;
 moon2Rotation = 0;
 xEye = 0.0;
 yEye = 0.0;
 zEye = 5.0;
 xCen = 0.0;
 yCen = 0.0;
 zCen = 0.0;
 xUp = 0.0;
 yUp = 1.0;
 zUp = 0.0;
 sunX = sunY = 0.0;
 sunZ = earthZ = moon1Z = moon2Z = 0.0;
 earthX = EARTH_ORBIT_RADIUS;
 earthY = 0.0;
 moon1X = EARTH_ORBIT_RADIUS+MOON1_ORBIT_RADIUS;
 moon1Y = 0.0;
 moon2X = EARTH_ORBIT_RADIUS-MOON2_ORBIT_RADIUS;
 moon2Y = 0.0;
 glutPostRedisplay();
}

void track()
//Setting global variables that define an point at which the
//camera is aimed.
{
   switch (tracking)
     {
       case SUN      : getSunPosition(&xCen,&yCen,&zCen); break;
       case EARTH    : getEarthPosition(&xCen,&yCen,&zCen); break;
       case MOON1    : getMoon1Position(&xCen,&yCen,&zCen); break;
       case MOON2    : getMoon2Position(&xCen,&yCen,&zCen); break;
       default: break;
     }
}


void ride()
//Setting global variables that define the location of the camera.
{
   switch (riding)
     {
       case SUN      : getSunPosition(&xEye,&yEye,&zEye); break;
       case EARTH    : getEarthPosition(&xEye,&yEye,&zEye); break;
       case MOON1    : getMoon1Position(&xEye,&yEye,&zEye); break;
       case MOON2    : getMoon2Position(&xEye,&yEye,&zEye); break;
       default: break;
     }
}


void timeStep()
//Move forward or backward in time and change state variables
//accordingly.
{
   if (direction == UP)
     {
 	 sunRotation     += SUN_ROTATION_STEP;
         earthRevolution += EARTH_REVOLUTION_STEP;
         earthRotation   += EARTH_ROTATION_STEP;
         moon1Revolution  += MOON1_REVOLUTION_STEP;
         moon1Rotation    += MOON1_ROTATION_STEP;
         moon2Revolution  += MOON2_REVOLUTION_STEP;
         moon2Rotation    += MOON2_ROTATION_STEP;
     }
   else
     {
         sunRotation     -= SUN_ROTATION_STEP;
         earthRevolution -= EARTH_REVOLUTION_STEP;
         earthRotation   -= EARTH_ROTATION_STEP;
         moon1Revolution  -= MOON1_REVOLUTION_STEP;
         moon1Rotation    -= MOON1_ROTATION_STEP;
         moon2Revolution  -= MOON2_REVOLUTION_STEP;
         moon2Rotation    -= MOON2_ROTATION_STEP;
     }
   track();
   ride();
}





void operate ()
//Process the operation that the user has selected.
{
 if (operation==TIME) timeStep();
 else if (operation==POSITION)
   switch (axis)
      {
    case X :
      if (direction == UP) xEye += EYE_STEP;
      else xEye -= EYE_STEP;
      break;
    case Y :
      if (direction == UP) yEye += EYE_STEP;
      else yEye -= EYE_STEP;
      break;
    case Z :
      if (direction == UP) zEye += EYE_STEP;
      else zEye -= EYE_STEP;
      break;
      }
 else if (operation==AIM)
   switch (axis)
      {
    case X :
      if (direction == UP) xCen += CEN_STEP;
      else xCen -= CEN_STEP;
      break;
    case Y :
      if (direction == UP) yCen += CEN_STEP;
      else yCen -= CEN_STEP;
      break;
    case Z :
      if (direction == UP) zCen += CEN_STEP;
      else zCen -= CEN_STEP;
      break;
      }
 else if (operation==ZOOM)
         {
	   int sign;
           if (direction == UP) sign = 1; else sign = -1;
	   xEye += sign * (xCen - xEye)/ZOOM_FACTOR;
	   yEye += sign * (yCen - yEye)/ZOOM_FACTOR;
	   zEye += sign * (zCen - zEye)/ZOOM_FACTOR;
         }
 else if (operation==PROJECTION)
      if (direction == UP) projection = ORTHOGRAPHIC;
         else projection = PERSPECTIVE;
 glutPostRedisplay();
}

void stepDisplay()
//Glut idle callback for animation.
{
  timeStep();
  glutPostRedisplay();
}


void keyboard (unsigned char key, int, int)
//Function to support keyboard control of some operations.
{
   switch (key) {
      case 't': direction = DOWN; timeStep(); break;
      case 'T': direction = UP; timeStep(); break;
      case 'x':
         xEye -= EYE_STEP;
         glutPostRedisplay();
         break;
      case 'X':
         xEye += EYE_STEP;
         glutPostRedisplay();
         break;
      case 'y':
         yEye -= EYE_STEP;
         glutPostRedisplay();
         break;
      case 'Y':
         yEye += EYE_STEP;
         glutPostRedisplay();
         break;
      case 'z':
         zEye -= EYE_STEP;
         glutPostRedisplay();
         break;
      case 'Z':
  	 zEye += EYE_STEP;
         glutPostRedisplay();
         break;
      case 'a':
         xCen -= CEN_STEP;
         glutPostRedisplay();
         break;
      case 'A':
         xCen += CEN_STEP;
         glutPostRedisplay();
         break;
      case 'b':
         yCen -= CEN_STEP;
         glutPostRedisplay();
         break;
      case 'B':
         yCen += CEN_STEP;
         glutPostRedisplay();
         break;
      case 'c':
         zCen -= CEN_STEP;
         glutPostRedisplay();
         break;
      case 'C':
  	 zCen += CEN_STEP;
         glutPostRedisplay();
         break;
      case 'P':
 	 projection = PERSPECTIVE;
	 glutPostRedisplay();
	 break;
      case 'p':
 	 projection = PERSPECTIVE;
	 glutPostRedisplay();
	 break;
      case 'O':
 	 projection = ORTHOGRAPHIC;
	 glutPostRedisplay();
	 break;
      case 'o':
 	 projection = ORTHOGRAPHIC;
	 glutPostRedisplay();
	 break;
      case 'W':
  	 sphere = WIRE;
         glutPostRedisplay();
         break;
      case 'w':
  	 sphere = WIRE;
         glutPostRedisplay();
         break;
      case 'S':
  	 sphere = SOLID;
         glutPostRedisplay();
         break;
      case 's':
  	 sphere = SOLID;
         glutPostRedisplay();
         break;
      case 27:
         std::exit(0);
         break;
      default:
         break;
   }
}

void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 0 : operation = ZOOM; break;
 case 1 : operation = TIME; break;
 case 2 : operation = PROJECTION; break;
 case 3 : homePosition();glutPostRedisplay(); break;
 case 4 : std::exit( 0 );
 }
}


void trackSubMenu( int item )
// Callback for processing camera set aim submenu.
{
  switch ( item )
  {
    case 1 : tracking = SUN; track(); break;
    case 2 : tracking = EARTH; track(); break;
    case 3 : tracking = MOON1; track(); break;
    case 4 : tracking = MOON2; track(); break;
  }
  glutPostRedisplay();
}

void rideSubMenu( int item )
// Callback for processing camera set ride submenu.
{
  switch ( item )
  {
    case 1 : riding = SUN; ride(); break;
    case 2 : riding = EARTH; ride(); break;
    case 3 : riding = MOON1; ride(); break;
    case 4 : riding = MOON2; ride(); break;
  }
  glutPostRedisplay();
}

void aimSubMenu( int item )
// Callback for processing camera change aim submenu.
{
  operation = AIM;
  tracking = NONE;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void positionSubMenu( int item )
// Callback for processing camera position submenu.
{
  operation = POSITION;
  riding = NONE;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}


void orientationSubMenu( int item )
// Callback for processing camera orientation submenu.
{
  switch ( item )
  {
   case 1 : {xUp = 1.0; yUp = 0.0; zUp = 0.0; break; }
   case 2 : {xUp = 0.0; yUp = 1.0; zUp = 0.0; break; }
   case 3 : {xUp = 0.0; yUp = 0.0; zUp = 1.0; break; }
  }
  glutPostRedisplay();
}


void animateSubMenu( int item )
// Callback for processing animate submenu.
{
  operation = ANIMATE;
  glutIdleFunc(stepDisplay);
  switch ( item )
  {
    case 1 : direction = UP; break;
    case 2 : direction = DOWN; break;
  }
}


void setMenus( )
// Routine for creating menus.
{
  int trackSubMenuCode, rideSubMenuCode;
  int aimSubMenuCode, positionSubMenuCode, orientationSubMenuCode;
  int animateSubMenuCode;

  trackSubMenuCode = glutCreateMenu( trackSubMenu );
  glutAddMenuEntry( "Sun",        1 );
  glutAddMenuEntry( "Earth",      2 );
  glutAddMenuEntry( "Moon1",      3 );
  glutAddMenuEntry( "Moon2",      4 );

  rideSubMenuCode = glutCreateMenu( rideSubMenu );
  glutAddMenuEntry( "Sun",        1 );
  glutAddMenuEntry( "Earth",      2 );
  glutAddMenuEntry( "Moon1",      3 );
  glutAddMenuEntry( "Moon2",      4 );

  aimSubMenuCode = glutCreateMenu( aimSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  positionSubMenuCode = glutCreateMenu( positionSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  orientationSubMenuCode = glutCreateMenu( orientationSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  animateSubMenuCode = glutCreateMenu( animateSubMenu );
  glutAddMenuEntry( "Forward",     1 );
  glutAddMenuEntry( "Backward",    2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Set Body Tracking  ...",  trackSubMenuCode );
  glutAddSubMenu( "Set Body Riding  ...",  rideSubMenuCode );
  glutAddSubMenu( "Change Camera Aim ...",  aimSubMenuCode );
  glutAddSubMenu( "Change Camera Position ...", positionSubMenuCode );
  glutAddSubMenu( "Change Camera Orientation ...", orientationSubMenuCode );
  glutAddSubMenu( "Animate ...", animateSubMenuCode );
  glutAddMenuEntry( "Zoom",  0 );
  glutAddMenuEntry( "Step",  1 );
  glutAddMenuEntry( "Change Projection",  2 );
  glutAddMenuEntry( "Home Position",  3 );
  glutAddMenuEntry( "Exit",  4 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

void mouse( int button, int state, int, int )
// Routine for processing mouse events.
{
   if (operation == ANIMATE)
      { glutIdleFunc(NULL); operation = NOTHING; return; }
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




int main(int argc, char** argv)
{
    // Mask floating point exceptions.
    _control87( MCW_EM, MCW_EM );

   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize (500, 500);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Solar System Simulation");
   init ();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMouseFunc( mouse );
   glEnable( GL_DEPTH_TEST );

   setMenus();
   homePosition();

   glutMainLoop();
   return 0;
}
