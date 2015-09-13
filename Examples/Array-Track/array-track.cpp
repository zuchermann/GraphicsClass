/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics           Fall 98  */
/*--------------------------------------------------------*/
/*                   Robot and array track animation      */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <ctime>
#include <cfloat>

using namespace std;

#define TRACK_SLICES 36
#define TRACK_RINGS 1
#define TRACK_RADIUS 1.0
#define TRACK_WIDTH_FACTOR 1.2
#define SPHERE_RADIUS 0.015
#define SPHERE_SLICES 12
#define SPHERE_STACKS 6
#define CYLINDER_BASE 0.5
#define CYLINDER_HEIGHT 0.5
#define CYLINDER_SLICES 6
#define CYLINDER_STACKS 1
#define NUM_CIRCLES 6
#define TILE_ROWS_PER_CIRCLE 10
#define CART_BASE_SIZE 0.1
#define CART_ASPECT_X 1.0
#define CART_ASPECT_Y 2.0
#define CART_ASPECT_Z 0.5
#define WHEEL_RADIUS1 0.0375
#define WHEEL_RADIUS2 0.0125
#define WHEEL_SLICES 10
#define WHEEL_DICES  20
#define CART_BASE_HEIGHT (WHEEL_RADIUS1 + WHEEL_RADIUS2)
#define SPOKE_SLICES 3
#define SPOKE_STACKS 1
#define SPOKE_RADIUS 0.00125
#define DELTA 0.005
#define EPSILON 0.005
#define FRAME_FACTOR 1.05
#define PI 3.14159
#define NUM_RABBITS 20


//Constants defining the step increments for changing
//the location, and field of view of the camera.
#define EYE_STEP 0.1
#define CEN_STEP 0.1
#define FOV_STEP 5.0

//System constants determined at initialization;
GLdouble trackInnerRadius;
GLdouble trackOuterRadius;
GLdouble trackWidth;

//Data structures for drawing cylinders.
GLUquadricObj *spoke;
GLUquadricObj *ring;
GLUquadricObj *sphere;
GLUquadricObj *cylinder;

//Global variables for keeping track of the size and shape
//of the window.
int windowHeight, windowWidth;

//Enumerated type and global variable for keeping track
//of the selected operation.
typedef enum { POSITION, AIM, ORIENTATION, ZOOM, HOME, SPEED, TIME, PROJECTION, ANIMATE, NOTHING} operationType;
operationType operation = POSITION;

//Enumerated type and global variable for keeping track
//of the object the user has selected to track with the camera
//and the object the user has selected for the camera to ride.
typedef enum { CART_CENTER, CART_FRONT, CART_MIDDLE, CART_REAR, ORIGIN, NONE } trackingType;
trackingType tracking = NONE;
trackingType riding = NONE;

//Enumerated type and global variable for talking about axies.
typedef enum { X, Y, Z } axisType;
axisType axis = Z;

//Global variables for keeping track of the speed of the animation.
int speed;

//Enumerated type and global variable for talking about
//direction of changes in camera position, and aim
//and the direction of time steps and animation.
typedef enum { DOWN, UP } directionType;
directionType direction = UP;

//Enumerated type and global variable for keeping track of
//the type of projection being used.
typedef enum { ORTHOGRAPHIC, PERSPECTIVE } projectionType;
projectionType  projection = PERSPECTIVE;

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

//Global variable for keepting track of field of view.
GLdouble fov = 45.0;


// Scene Constants:
const GLdouble trackRadius = TRACK_RADIUS;
const GLdouble wheelRadius = CART_BASE_HEIGHT;
GLdouble radiusRatio;

const int numCircles = NUM_CIRCLES;
int numTileRows;
GLdouble tileSize;
GLdouble extente;

GLdouble trackDegreesPerStep;
GLdouble trackLengthPerStep;
GLdouble wheelDegreesPerStep;


// Granularity constants:
// Ratio chosen to be approximately 4/pi,
// ratio of diameter to quarter circle arc.
// Number of steps along a circle diameter.
const int numCartTranslationSteps = 510;
// Number of steps in a quarter circle arc.
const int numCartRevolutionSteps = 400;


// Type definitions:
typedef enum {CIRCLE, LINE} pathType;
typedef enum {CLOCK, COUNTER} circulationType;
typedef enum {EAST=0, NORTH, WEST, SOUTH} headingType;
typedef enum {LEFT=-1, RIGHT, STRAIGHT} steeringType;

struct state
{
  pathType path;

  int xCartCenter;
  int yCartCenter;
  int quadrant;
  circulationType circulation;
  int cartRevolution;

  int xSegmentStart;
  int ySegmentStart;
  headingType heading;
  int cartTranslation;

  GLdouble wheelRevolution;

  bool randomize;

  steeringType steering;

};

state rabbits[NUM_RABBITS];
state chaseCarState;

GLuint theTrack;
GLuint theCartBody;
GLuint aWheel;


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

void normalize(GLdouble* angle)
//Normalizing an angle to put it in the range: -180.0 < angle <= 180.0
{
  while (*angle >= 180.0) *angle -= 360.0;
  while (*angle < -180.0) *angle += 360.0;
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


void drawCartBase()
// Draw the cart base, relative to cart coordinates.
{
   glPushMatrix();
   //   glScaled ((GLdouble) CART_ASPECT_X, (GLdouble) CART_ASPECT_Y, (GLdouble) CART_ASPECT_Z);
   glScaled ((GLdouble) CART_ASPECT_X*1.05, (GLdouble) CART_ASPECT_Y*1.25, (GLdouble) CART_ASPECT_Z);
   GLfloat cartBaseDiffuse[] = {1.0, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, cartBaseDiffuse);
   GLfloat cartBaseAmbient[] = {0.65, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, cartBaseAmbient);
   //   glutSolidCube(CART_BASE_SIZE);
   gluSphere(sphere,CART_BASE_SIZE/2.0,SPHERE_SLICES*4,SPHERE_STACKS);
   glPopMatrix();
}

void drawCartAxles()
// Draw the cart axles, relative to cart coordinates.
{
   GLfloat cartAxlesDiffuse[] = {0.35, 0.35, 0.35, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, cartAxlesDiffuse);
   GLfloat cartAxlesAmbient[] = {0.325, 0.325, 0.325, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, cartAxlesAmbient);

   glPushMatrix();

   glRotated(90.0,0.0,1.0,0.0);

   glPushMatrix();
   glTranslated(0.0,CART_BASE_SIZE*CART_ASPECT_Y/2-WHEEL_RADIUS1,-CART_BASE_SIZE*CART_ASPECT_X/2-WHEEL_RADIUS2);
   gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,CART_BASE_SIZE*CART_ASPECT_X+2*WHEEL_RADIUS2,SPOKE_SLICES,SPOKE_STACKS);
   glPopMatrix();

   glPushMatrix();
   glTranslated(0.0,-CART_BASE_SIZE*CART_ASPECT_Y/2+WHEEL_RADIUS1,-CART_BASE_SIZE*CART_ASPECT_X/2-WHEEL_RADIUS2);
   gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,CART_BASE_SIZE*CART_ASPECT_X+2*WHEEL_RADIUS2,SPOKE_SLICES,SPOKE_STACKS);
   glPopMatrix();

   glPopMatrix();
}

void drawCartCanopy()
// Draw the cart canopy.
{
   glPushMatrix();
   glTranslated (0.0,-CART_BASE_SIZE/4,CART_BASE_HEIGHT/2.0);
   glScaled (1.0,2.0,1.0);
   GLfloat cartCanopyDiffuse[] = {1.0, 1.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, cartCanopyDiffuse);
   GLfloat cartCanopyAmbient[] = {.625, .625, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, cartCanopyAmbient);
   //   gluSphere(sphere,CART_BASE_SIZE/3.0,SPHERE_SLICES,SPHERE_STACKS);
   gluSphere(sphere,CART_BASE_SIZE/4.0,SPHERE_SLICES*4,SPHERE_STACKS);
   glPopMatrix();
}

void drawSpokes()
// Draw wheel spokes, relative to wheel coordinates.
{
  GLfloat spokesDiffuse[] = {0.35, 0.35, 0.35, 1.0};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, spokesDiffuse);
  GLfloat spokesAmbient[] = {0.325, 0.325, 0.325, 1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, spokesAmbient);

  glPushMatrix();

  glRotated(90.0,1.0,0.0,0.0);
  glTranslated(0.0,0.0,-WHEEL_RADIUS1);
  gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,2*WHEEL_RADIUS1,SPOKE_SLICES,SPOKE_STACKS);
  glTranslated(0.0,0.0,WHEEL_RADIUS1);
  glRotated(-90.0,1.0,0.0,0.0);
  glRotated(90.0,0.0,1.0,0.0);
  glTranslated(0.0,0.0,-WHEEL_RADIUS1);
  gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,2*WHEEL_RADIUS1,SPOKE_SLICES,SPOKE_STACKS);
  glTranslated(0.0,0.0,WHEEL_RADIUS1);
  glRotated(-90.0,0.0,1.0,0.0);

  glRotated(45.0,0.0,0.0,1.0);

  glRotated(90.0,1.0,0.0,0.0);
  glTranslated(0.0,0.0,-WHEEL_RADIUS1);
  gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,2*WHEEL_RADIUS1,SPOKE_SLICES,SPOKE_STACKS);
  glTranslated(0.0,0.0,WHEEL_RADIUS1);
  glRotated(-90.0,1.0,0.0,0.0);
  glRotated(90.0,0.0,1.0,0.0);
  glTranslated(0.0,0.0,-WHEEL_RADIUS1);
  gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,2*WHEEL_RADIUS1,SPOKE_SLICES,SPOKE_STACKS);
  glTranslated(0.0,0.0,WHEEL_RADIUS1);
  glRotated(-90.0,0.0,1.0,0.0);

  glPopMatrix();

  //  glMaterialf(GL_FRONT, GL_SHININESS, 0.0);

}

void drawCircle(GLdouble radius)
//Draw a circle in the z=0 plane the slow but easy way.
{
  GLdouble theta, delta;
  delta = 2*PI/TRACK_SLICES;
  glBegin(GL_LINE_LOOP);
    for (theta = 0; theta < 2*PI; theta += delta)
        glVertex3f(radius*cos(theta),radius*sin(theta),0.0);
  glEnd();
}

void gotoWheelCoordinates(int lr, int fb, state& carState)
// Move to a coordinate system at one of the wheels, assuming
// already at cart coordinates.
{
   glRotated(90.0, 0.0, 1.0, 0.0);
   glTranslated (0.0,
                 fb*(CART_BASE_SIZE*CART_ASPECT_Y/2-WHEEL_RADIUS1),
                 lr*(CART_BASE_SIZE*CART_ASPECT_X/2+WHEEL_RADIUS2));
   glRotated(carState.wheelRevolution,0.0,0.0,1.0);
}

void  drawCartWheels(state& carState)
// Draw four wheels, relative to cart coordinates.
{
   glPushMatrix();
   gotoWheelCoordinates(1,1,carState);
   glCallList(aWheel);
   glPopMatrix();
   glPushMatrix();
   gotoWheelCoordinates(1,-1,carState);
   glCallList(aWheel);
   glPopMatrix();
   glPushMatrix();
   gotoWheelCoordinates(-1,1,carState);
   glCallList(aWheel);
   glPopMatrix();
   glPushMatrix();
   gotoWheelCoordinates(-1,-1,carState);
   glCallList(aWheel);
   glPopMatrix();
}

void gotoLightCoordinates(int lr, int fb)
// Move to a coordinate system at one of the lights, assuming
// already at cart coordinates.
{
   glTranslated ((1.0/2.0)*lr*(CART_BASE_SIZE*CART_ASPECT_X/2),
                 fb*(CART_BASE_SIZE*CART_ASPECT_Y/2),
                 0.0);
}

void  drawCartLights()
// Draw four wheels, relative to cart coordinates.
{
   GLfloat cartLightsDiffuse[] = {1.0, 1.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, cartLightsDiffuse);
   GLfloat cartLightsAmbient[] = {.75, .75, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, cartLightsAmbient);
   glPushMatrix();
   gotoLightCoordinates(1,1);
   gluSphere(sphere,SPHERE_RADIUS,SPHERE_SLICES,SPHERE_STACKS);
   glPopMatrix();
   glPushMatrix();
   gotoLightCoordinates(1,-1);
   gluSphere(sphere,SPHERE_RADIUS,SPHERE_SLICES,SPHERE_STACKS);
   glPopMatrix();
   glPushMatrix();
   gotoLightCoordinates(-1,1);
   gluSphere(sphere,SPHERE_RADIUS,SPHERE_SLICES,SPHERE_STACKS);
   glPopMatrix();
   glPushMatrix();
   gotoLightCoordinates(-1,-1);
   gluSphere(sphere,SPHERE_RADIUS,SPHERE_SLICES,SPHERE_STACKS);
   glPopMatrix();
}


void gotoCartCoordinates(state& carState)
// Move to a cart centered coordinate system, assuming presently
// at the origin.
{
   GLdouble xc, yc, x, y, angle=0.0;
   switch (carState.path)
     {
       case CIRCLE: xc = carState.xCartCenter*trackRadius;
	            yc = carState.yCartCenter*trackRadius;
		    glTranslated (xc, yc, 0.0);
		    switch (carState.circulation)
		      {
		        case COUNTER : angle = carState.quadrant*90 + carState.cartRevolution*trackDegreesPerStep;
			               break;
		        case CLOCK   : angle = (carState.quadrant+1)*90 - carState.cartRevolution*trackDegreesPerStep;
			               break;
		      }
		    glRotated (angle, 0.0, 0.0, 1.0);
		    glTranslated(trackInnerRadius+trackWidth/2,0.0,0.0);
		    if (carState.circulation==CLOCK) glRotated (180.0, 0.0, 0.0, 1.0);
		    break;
       case LINE  : x = carState.xSegmentStart*trackRadius;
	            y = carState.ySegmentStart*trackRadius;
		    switch (carState.heading)
		      {
		         case NORTH : y+=carState.cartTranslation*trackLengthPerStep;
			              glTranslated (x, y, 0.0);
				      glRotated (0.0, 0.0, 0.0, 1.0);
				      break;
		         case SOUTH : y-=carState.cartTranslation*trackLengthPerStep;
			              glTranslated (x, y, 0.0);
				      glRotated (180.0, 0.0, 0.0, 1.0);
				      break;
		         case EAST  : x+=carState.cartTranslation*trackLengthPerStep;
			              glTranslated (x, y, 0.0);
				      glRotated (270.0, 0.0, 0.0, 1.0);
				      break;
		         case WEST  : x-=carState.cartTranslation*trackLengthPerStep;
			              glTranslated (x, y, 0.0);
				      glRotated (90.0, 0.0, 0.0, 1.0);
				      break;
		      }
		    break;
     }
   glTranslated (0.0, 0.0,(GLdouble)CART_BASE_HEIGHT);
}

void drawCart(state& carState)
// Draw the cart and robot system, relative to the origin.
{
   glPushMatrix();
   gotoCartCoordinates(carState);
   glCallList(theCartBody);
   drawCartWheels(carState);
   glPopMatrix();
}

void drawSky()
{
   GLfloat skyEmission[] = {0.85, .85, 1.0, 1.0};
   GLfloat noEmission[] = {0.0, 0.0, 0.0, 1.0};
   GLfloat noAmbient[] = {0.0, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, noAmbient);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, skyEmission);
   gluSphere(sphere,9*numCircles*trackRadius,SPHERE_SLICES*4,SPHERE_STACKS);
   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}


void drawGround()
{
   GLfloat groundDiffuse[] = {0.0, 0.5, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, groundDiffuse);
   GLfloat groundAmbient[] = {0.0, .75, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, groundAmbient);

   glNormal3f(0.0,0.0,1.0);

   glBegin(GL_POLYGON);
   glVertex3f(-10*numCircles*trackRadius,-10*numCircles*trackRadius,-0.1);
   glVertex3f( 10*numCircles*trackRadius,-10*numCircles*trackRadius,-0.1);
   glVertex3f( 10*numCircles*trackRadius, 10*numCircles*trackRadius,-0.1);
   glVertex3f(-10*numCircles*trackRadius, 10*numCircles*trackRadius,-0.1);
   glEnd();

}


void drawTrack()
// Draw the track, relative to the origin.
{
   GLfloat trackDiffuse[] = {0.25, 0.25, 0.25, 1.0};
   GLfloat pyramidDiffuse1[] = {1.0, 1.0, 0.125, 1.0};
   GLfloat pyramidDiffuse2[] = {0.125, 0.125, 1.0, 1.0};
   GLfloat pyramidAmbient1[] = {0.7, 0.7, 0.0, 1.0};
   GLfloat pyramidAmbient2[] = {0.0, 0.0, 0.7, 1.0};
   GLfloat trackAmbient[] = {0.25, 0.25, 0.25, 1.0};

   int i, j;
   for (i = 1; i<=numCircles; i++)
       for (j = 1; j<=numCircles; j++)
	 {
	   glPushMatrix();
	   glTranslated((2*i-1)*trackRadius,(2*j-1)*trackRadius,0.0);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, trackDiffuse);
	   glMaterialfv(GL_FRONT, GL_AMBIENT, trackAmbient);
	   gluDisk(ring,trackInnerRadius,trackOuterRadius,TRACK_SLICES,TRACK_RINGS);
	   if ((i+j)%2==0)
	     { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse1);
	       glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient1); }
	   else { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse2);
	          glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient2); }
	   gluCylinder(cylinder,CYLINDER_BASE,0.0,CYLINDER_HEIGHT,CYLINDER_SLICES,CYLINDER_STACKS);
	   glPopMatrix();
	   glPushMatrix();
	   glTranslated((2*i-1)*trackRadius,-(2*j-1)*trackRadius,0.0);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, trackDiffuse);
	   glMaterialfv(GL_FRONT, GL_AMBIENT, trackAmbient);
	   gluDisk(ring,trackInnerRadius,trackOuterRadius,TRACK_SLICES,TRACK_RINGS);
	   if ((i+j)%2==0)
	     { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse1);
	       glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient1); }
	   else { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse2);
	          glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient2); }
	   gluCylinder(cylinder,CYLINDER_BASE,0.0,CYLINDER_HEIGHT,CYLINDER_SLICES,CYLINDER_STACKS);
	   glPopMatrix();
	   glPushMatrix();
	   glTranslated(-(2*i-1)*trackRadius,(2*j-1)*trackRadius,0.0);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, trackDiffuse);
	   glMaterialfv(GL_FRONT, GL_AMBIENT, trackAmbient);
	   gluDisk(ring,trackInnerRadius,trackOuterRadius,TRACK_SLICES,TRACK_RINGS);
	   if ((i+j)%2==0) glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse1);
	   else glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse2);
	   if ((i+j)%2==0)
	     { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse1);
	       glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient1); }
	   else { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse2);
	          glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient2); }
	   gluCylinder(cylinder,CYLINDER_BASE,0.0,CYLINDER_HEIGHT,CYLINDER_SLICES,CYLINDER_STACKS);
	   glPopMatrix();
	   glPushMatrix();
	   glTranslated(-(2*i-1)*trackRadius,-(2*j-1)*trackRadius,0.0);
	   glMaterialfv(GL_FRONT, GL_DIFFUSE, trackDiffuse);
	   glMaterialfv(GL_FRONT, GL_AMBIENT, trackAmbient);
	   gluDisk(ring,trackInnerRadius,trackOuterRadius,TRACK_SLICES,TRACK_RINGS);
	   if ((i+j)%2==0) glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse1);
	   else glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse2);
	   if ((i+j)%2==0)
	     { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse1);
	       glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient1); }
	   else { glMaterialfv(GL_FRONT, GL_DIFFUSE, pyramidDiffuse2);
	          glMaterialfv(GL_FRONT, GL_AMBIENT, pyramidAmbient2); }
	   gluCylinder(cylinder,CYLINDER_BASE,0.0,CYLINDER_HEIGHT,CYLINDER_SLICES,CYLINDER_STACKS);
	   glPopMatrix();
	 }


   glMaterialfv(GL_FRONT, GL_DIFFUSE, trackDiffuse);
   glMaterialfv(GL_FRONT, GL_AMBIENT, trackAmbient);
   glNormal3f(0.0,0.0,1.0);

   for (i = -numCircles; i<=numCircles; i++)
     {
       glBegin(GL_POLYGON);
       glVertex3f(2*i*trackRadius+trackWidth/2,-2*numCircles*trackRadius+trackRadius,0.0);
       glVertex3f(2*i*trackRadius+trackWidth/2,2*numCircles*trackRadius-trackRadius,0.0);
       glVertex3f(2*i*trackRadius-trackWidth/2,2*numCircles*trackRadius-trackRadius,0.0);
       glVertex3f(2*i*trackRadius-trackWidth/2,-2*numCircles*trackRadius+trackRadius,0.0);
       glEnd();
     }
   for (j = -numCircles; j<=numCircles; j++)
     {
       glBegin(GL_POLYGON);
       glVertex3f(-2*numCircles*trackRadius+trackRadius,2*j*trackRadius+trackWidth/2,0.0);
       glVertex3f(-2*numCircles*trackRadius+trackRadius,2*j*trackRadius-trackWidth/2,0.0);
       glVertex3f(2*numCircles*trackRadius-trackRadius,2*j*trackRadius-trackWidth/2,0.0);
       glVertex3f(2*numCircles*trackRadius-trackRadius,2*j*trackRadius+trackWidth/2,0.0);
       glEnd();
     }
}

void getCartPosition(GLdouble* x, GLdouble* y, GLdouble* z, trackingType where, state& carState)
//Finding the current position of the cart.
{
  glPushMatrix();
  glLoadIdentity();
  gotoCartCoordinates(carState);
  switch (where)
    {
    case  CART_FRONT:  glTranslated (0.0, CART_BASE_SIZE*CART_ASPECT_Y/2-WHEEL_RADIUS1,1.5*CART_BASE_HEIGHT); break;
    case  CART_MIDDLE:   glTranslated (0.0,0.0,2.25*CART_BASE_HEIGHT); break;
    case  CART_REAR:   glTranslated (0.0,-CART_BASE_SIZE*CART_ASPECT_Y/2-WHEEL_RADIUS1,3.0*CART_BASE_HEIGHT); break;
    case  CART_CENTER: break;
     default: break;
    }
  getCurrentLocation(x,y,z);
  glPopMatrix();
}




void display(void)
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
   else  gluPerspective(fov, (GLdouble) windowWidth / (GLdouble) windowHeight,
                        0.01, 200.0);
   glMatrixMode(GL_MODELVIEW);

   drawGround();
   drawSky();

   glCallList(theTrack);
   drawCart(chaseCarState);
   for (int r=0; r<NUM_RABBITS; r++) drawCart(rabbits[r]);
   glutSwapBuffers();
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
   else  gluPerspective(fov, (GLdouble) w / (GLdouble) h, 1.0, 200.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void track()
//Setting global variables that define an point at which the
//camera is aimed.
{
   switch (tracking)
     {
       case ORIGIN  : xCen=yCen=zCen=0.0; break;
       case CART_CENTER    : getCartPosition(&xCen,&yCen,&zCen,CART_CENTER,chaseCarState); break;
       case CART_FRONT     : getCartPosition(&xCen,&yCen,&zCen,CART_FRONT,chaseCarState); break;
       case CART_MIDDLE    : getCartPosition(&xCen,&yCen,&zCen,CART_MIDDLE,chaseCarState); break;
       case CART_REAR      : getCartPosition(&xCen,&yCen,&zCen,CART_REAR,chaseCarState); break;
       default: break;
     }
}

void ride()
//Setting global variables that define the location of the camera.
{
   switch (riding)
     {
       case ORIGIN  : xEye=yEye=zEye=0.0; break;
       case CART_CENTER    : getCartPosition(&xEye,&yEye,&zEye,CART_CENTER,chaseCarState); break;
       case CART_FRONT     : getCartPosition(&xEye,&yEye,&zEye,CART_FRONT,chaseCarState); break;
       case CART_MIDDLE    : getCartPosition(&xEye,&yEye,&zEye,CART_MIDDLE,chaseCarState); break;
       case CART_REAR      : getCartPosition(&xEye,&yEye,&zEye,CART_REAR,chaseCarState); break;
       default: break;
     }
}

steeringType circleOnTrack(state& carState)
{
  if ((carState.circulation==CLOCK && carState.steering==RIGHT)||(carState.circulation==COUNTER && carState.steering==LEFT))
     return carState.steering;
  int length = numCircles * 2 - 1;
  if ((carState.xCartCenter==length)&&(carState.yCartCenter==length))
    {
     if (carState.circulation==CLOCK)
        switch (carState.quadrant)
	  {
	    case 0 : if (carState.steering==LEFT) return STRAIGHT; else return carState.steering;
  	    case 1 : return RIGHT;
	    case 2 : if (carState.steering==STRAIGHT) return RIGHT; else return carState.steering;
 	    case 3 : return carState.steering;
	  }
     if (carState.circulation==COUNTER)
        switch (carState.quadrant)
	  {
	    case 0 : if (carState.steering==RIGHT) return STRAIGHT; else return carState.steering;
  	    case 1 : return carState.steering;
	    case 2 : if (carState.steering==STRAIGHT) return LEFT; else return carState.steering;
 	    case 3 : return LEFT;
	  }
     return carState.steering;
    }
  if ((carState.xCartCenter==length)&&(carState.yCartCenter==-length))
    {
     if (carState.circulation==CLOCK)
        switch (carState.quadrant)
	  {
	    case 0 : return RIGHT;
  	    case 1 : if (carState.steering==STRAIGHT) return RIGHT; else return carState.steering;
	    case 2 : return carState.steering;
 	    case 3 : if (carState.steering==LEFT) return STRAIGHT; else return carState.steering;
	  }
     if (carState.circulation==COUNTER)
        switch (carState.quadrant)
	  {
  	    case 0 : return carState.steering;
	    case 1 : if (carState.steering==STRAIGHT) return LEFT; else return carState.steering;
 	    case 2 : return LEFT;
	    case 3 : if (carState.steering==RIGHT) return STRAIGHT; else return carState.steering;
	  }
     return carState.steering;
    }
  if ((carState.xCartCenter==-length)&&(carState.yCartCenter==length))
    {
     if (carState.circulation==CLOCK)
        switch (carState.quadrant)
	  {
	    case 0 : return carState.steering;
 	    case 1 : if (carState.steering==LEFT) return STRAIGHT; else return carState.steering;
	    case 2 : return RIGHT;
  	    case 3 : if (carState.steering==STRAIGHT) return RIGHT; else return carState.steering;
	  }
     if (carState.circulation==COUNTER)
        switch (carState.quadrant)
	  {
 	    case 0 : return LEFT;
	    case 1 : if (carState.steering==RIGHT) return STRAIGHT; else return carState.steering;
  	    case 2 : return carState.steering;
	    case 3 : if (carState.steering==STRAIGHT) return LEFT; else return carState.steering;
	  }
     return carState.steering;
    }
  if ((carState.xCartCenter==-length)&&(carState.yCartCenter==-length))
    {
     if (carState.circulation==CLOCK)
        switch (carState.quadrant)
	  {
  	    case 0 : if (carState.steering==STRAIGHT) return RIGHT; else return carState.steering;
	    case 1 : return carState.steering;
 	    case 2 : if (carState.steering==LEFT) return STRAIGHT; else return carState.steering;
	    case 3 : return RIGHT;
	  }
     if (carState.circulation==COUNTER)
        switch (carState.quadrant)
	  {
	    case 0 : if (carState.steering==STRAIGHT) return LEFT; else return carState.steering;
 	    case 1 : return LEFT;
	    case 2 : if (carState.steering==RIGHT) return STRAIGHT; else return carState.steering;
  	    case 3 : return carState.steering;
	  }
     return carState.steering;
    }
  if (carState.xCartCenter ==  length)
     if ((carState.circulation==CLOCK))
        { if ((carState.quadrant==1)&&(carState.steering==STRAIGHT)||((carState.quadrant==0)&&carState.steering==LEFT)) return RIGHT; }
     else if (carState.circulation==COUNTER)
        { if ((carState.quadrant==2)&&(carState.steering==STRAIGHT)||((carState.quadrant==3)&&carState.steering==RIGHT)) return LEFT; }
  if (carState.xCartCenter ==  -length)
     if ((carState.circulation==COUNTER))
        { if ((carState.quadrant==0)&&(carState.steering==STRAIGHT)||((carState.quadrant==1)&&carState.steering==RIGHT)) return LEFT; }
     else if (carState.circulation==CLOCK)
        { if ((carState.quadrant==3)&&(carState.steering==STRAIGHT)||((carState.quadrant==2)&&carState.steering==LEFT)) return RIGHT; }
  if (carState.yCartCenter ==  length)
     if ((carState.circulation==CLOCK))
        { if ((carState.quadrant==2)&&(carState.steering==STRAIGHT)||((carState.quadrant==1)&&carState.steering==LEFT)) return RIGHT; }
     else if (carState.circulation==COUNTER)
        { if ((carState.quadrant==3)&&(carState.steering==STRAIGHT)||((carState.quadrant==0)&&carState.steering==RIGHT)) return LEFT; }
  if (carState.yCartCenter ==  -length)
     if ((carState.circulation==COUNTER))
        { if ((carState.quadrant==1)&&(carState.steering==STRAIGHT)||((carState.quadrant==2)&&carState.steering==RIGHT)) return LEFT; }
     else if (carState.circulation==CLOCK)
        { if ((carState.quadrant==0)&&(carState.steering==STRAIGHT)||((carState.quadrant==3)&&carState.steering==LEFT)) return RIGHT; }
  return carState.steering;
}


steeringType lineOnTrack(state& carState)
{
  int length = numCircles * 2;
  if (carState.xSegmentStart==length)
     if ((carState.heading==NORTH)&&(carState.steering==RIGHT)) return LEFT;
     else if ((carState.heading==SOUTH)&&(carState.steering==LEFT)) return RIGHT;
  if (carState.xSegmentStart==-length)
     if ((carState.heading==NORTH)&&(carState.steering==LEFT)) return RIGHT;
     else if ((carState.heading==SOUTH)&&(carState.steering==RIGHT)) return LEFT;
  if (carState.ySegmentStart==length)
     if ((carState.heading==WEST)&&(carState.steering==RIGHT)) return LEFT;
     else if ((carState.heading==EAST)&&(carState.steering==LEFT)) return RIGHT;
  if (carState.ySegmentStart==-length)
     if ((carState.heading==WEST)&&(carState.steering==LEFT)) return RIGHT;
     else if ((carState.heading==EAST)&&(carState.steering==RIGHT)) return LEFT;

  length-=3;

  if ((carState.xSegmentStart==length)&&(carState.heading == EAST)&&(carState.steering==STRAIGHT))
     if (carState.ySegmentStart>0) return RIGHT; else return LEFT;
  if ((carState.xSegmentStart==-length)&&(carState.heading == WEST)&&(carState.steering==STRAIGHT))
     if (carState.ySegmentStart<0) return RIGHT; else return LEFT;
  if ((carState.ySegmentStart==length)&&(carState.heading == NORTH)&&(carState.steering==STRAIGHT))
     if (carState.xSegmentStart<0) return RIGHT; else return LEFT;
  if ((carState.ySegmentStart==-length)&&(carState.heading == SOUTH)&&(carState.steering==STRAIGHT))
     if (carState.xSegmentStart>0) return RIGHT; else return LEFT;

  return carState.steering;
}


void circleStep(state& carState)
{
  carState.cartRevolution++;
  carState.wheelRevolution -= wheelDegreesPerStep;
  if (carState.cartRevolution==numCartRevolutionSteps)
    {
     carState.steering = circleOnTrack(carState);
     if (carState.steering==STRAIGHT)
       {
          carState.path = LINE;
	  switch (carState.circulation)
	    {
	     case CLOCK:   switch (carState.quadrant)
	                     {
			        case 0 : carState.xSegmentStart = carState.xCartCenter + 1;
				         carState.ySegmentStart = carState.yCartCenter;
					 carState.heading = SOUTH; break;
			        case 1 : carState.ySegmentStart = carState.yCartCenter + 1;
				         carState.xSegmentStart = carState.xCartCenter;
					 carState.heading = EAST; break;
			        case 2 : carState.xSegmentStart = carState.xCartCenter - 1;
				         carState.ySegmentStart = carState.yCartCenter;
					 carState.heading = NORTH; break;
			        case 3 : carState.ySegmentStart = carState.yCartCenter - 1;
				         carState.xSegmentStart = carState.xCartCenter;
					 carState.heading = WEST; break;
			     }
	                   break;
	     case COUNTER: switch (carState.quadrant)
	                     {
			        case 0 : carState.ySegmentStart = carState.yCartCenter + 1;
				         carState.xSegmentStart = carState.xCartCenter;
					 carState.heading = WEST; break;
			        case 1 : carState.xSegmentStart = carState.xCartCenter - 1;
				         carState.ySegmentStart = carState.yCartCenter;
					 carState.heading = SOUTH; break;
			        case 2 : carState.ySegmentStart = carState.yCartCenter - 1;
				         carState.xSegmentStart = carState.xCartCenter;
					 carState.heading = EAST; break;
			        case 3 : carState.xSegmentStart = carState.xCartCenter + 1;
				         carState.ySegmentStart = carState.yCartCenter;
					 carState.heading = NORTH; break;
			     }
	                   break;
	    }

	  carState.cartTranslation=0;
       }
     else if (carState.circulation==COUNTER && carState.steering == RIGHT)
     {
       switch (carState.quadrant)
	 {
  	   case 0 : carState.yCartCenter+=2; carState.quadrant = 2; break;
  	   case 1 : carState.xCartCenter-=2; carState.quadrant = 3; break;
  	   case 2 : carState.yCartCenter-=2; carState.quadrant = 0; break;
  	   case 3 : carState.xCartCenter+=2; carState.quadrant = 1; break;
	 }
       carState.circulation = CLOCK;
       carState.cartRevolution=0;
     }
     else if  (carState.circulation==CLOCK   && carState.steering == LEFT)
     {
       switch (carState.quadrant)
	 {
  	   case 0 : carState.xCartCenter+=2; carState.quadrant = 2; break;
  	   case 1 : carState.yCartCenter+=2; carState.quadrant = 3; break;
  	   case 2 : carState.xCartCenter-=2; carState.quadrant = 0; break;
  	   case 3 : carState.yCartCenter-=2; carState.quadrant = 1; break;
	 }
       carState.circulation = COUNTER;
       carState.cartRevolution=0;
     }
     else {
            switch (carState.circulation)
	      {
	        case COUNTER : carState.quadrant++;
		               if (carState.quadrant>3) carState.quadrant = 0;
		               break;
	        case CLOCK   : carState.quadrant--;
		               if (carState.quadrant<0) carState.quadrant = 3;
		               break;
	      }
	    carState.cartRevolution=0;
     }
    }
}




void lineStep(state& carState)
{
  carState.cartTranslation++;
  carState.wheelRevolution -= wheelDegreesPerStep;
  if (carState.cartTranslation==numCartTranslationSteps)
    {
     carState.steering = lineOnTrack(carState);
     switch (carState.steering)
       {
         case RIGHT   : carState.path = CIRCLE;
	                carState.cartRevolution = 0;
			carState.circulation = CLOCK;
	                switch (carState.heading)
			  {
			     case NORTH :  carState.xCartCenter=carState.xSegmentStart+1;
			                   carState.yCartCenter=carState.ySegmentStart+2;
					   carState.quadrant = 1;
			                   break;
			     case SOUTH :  carState.xCartCenter=carState.xSegmentStart-1;
			                   carState.yCartCenter=carState.ySegmentStart-2;
					   carState.quadrant = 3;
			                   break;
			     case EAST  :  carState.xCartCenter=carState.xSegmentStart+2;
			                   carState.yCartCenter=carState.ySegmentStart-1;
					   carState.quadrant = 0;
			                   break;
			     case WEST  :  carState.xCartCenter=carState.xSegmentStart-2;
			                   carState.yCartCenter=carState.ySegmentStart+1;
					   carState.quadrant = 2;
			                   break;
			  }
	                break;
         case LEFT    : carState.path = CIRCLE;
	                carState.cartRevolution=0;
			carState.circulation = COUNTER;
	                switch (carState.heading)
			  {
			     case NORTH :  carState.xCartCenter=carState.xSegmentStart-1;
			                   carState.yCartCenter=carState.ySegmentStart+2;
					   carState.quadrant = 0;
			                   break;
			     case SOUTH :  carState.xCartCenter=carState.xSegmentStart+1;
			                   carState.yCartCenter=carState.ySegmentStart-2;
					   carState.quadrant = 2;
			                   break;
			     case EAST  :  carState.xCartCenter=carState.xSegmentStart+2;
			                   carState.yCartCenter=carState.ySegmentStart+1;
					   carState.quadrant = 3;
			                   break;
			     case WEST  :  carState.xCartCenter=carState.xSegmentStart-2;
			                   carState.yCartCenter=carState.ySegmentStart-1;
					   carState.quadrant = 1;
			                   break;
			  }
	                break;
         case STRAIGHT: carState.cartTranslation=0;
	                switch (carState.heading)
			  {
			     case NORTH :  carState.ySegmentStart+=2; break;
			     case SOUTH :  carState.ySegmentStart-=2; break;
			     case EAST  :  carState.xSegmentStart+=2; break;
			     case WEST  :  carState.xSegmentStart-=2; break;
			  }
			break;
       }
    }
}


void step(state& carState)
//Move forward or backward in time and change state variables
//accordingly.
{
  if (carState.randomize)
     switch (rand()%3)
       {
         case 0 : carState.steering = LEFT; break;
         case 1 : carState.steering = STRAIGHT; break;
         case 2 : carState.steering = RIGHT; break;
       }
  if(carState.path==CIRCLE)
    {
    }
  else
    {
    }
  switch (carState.path)
    {
      case CIRCLE: circleStep(carState); break;
      case LINE:   lineStep(carState); break;
    }
  track();
  ride();
}

void initialState(state& carState)
{
  carState.path = LINE;
  carState.heading = NORTH;
  carState.xSegmentStart = 0;
  carState.ySegmentStart = -1;
  carState.cartTranslation = 0;
  carState.wheelRevolution = 0.0;
  carState.steering = STRAIGHT;
  carState.randomize = true;
}

void homePosition()
//Put the robot state and all viewing varibles back to their original values.
{
 initialState(chaseCarState);
 for (int r=0; r<NUM_RABBITS; r++) initialState(rabbits[r]);

 axis = Z;
 direction = UP;
 operation = NOTHING;

 xEye = 0.0;
 yEye = 0.0;
 zEye = 5.0;
 xCen = 0.0;
 yCen = 0.0;
 zCen = 0.0;
 xUp = 0.0;
 yUp = 0.0;
 zUp = 1.0;

 tracking = CART_FRONT;
 riding = CART_REAR;

 speed = 3;

 glLoadIdentity();

 track();
 ride();

 glutPostRedisplay();
}


void operate ()
//Process the operation that the user has selected.
{
 if (operation==TIME)
   {
     step(chaseCarState);
     for (int r=0; r<NUM_RABBITS; r++) step(rabbits[r]);
   }
 else if (operation==SPEED)
         {
           switch (direction)
             {
	       case UP   : speed++; break;
	       case DOWN : speed--; break;
	     }
	 }
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
	      if (direction == UP) sign = -1; else sign = 1;
	      fov += sign * FOV_STEP;
         }
 else if (operation==PROJECTION)
      if (direction == UP) projection = ORTHOGRAPHIC;
         else projection = PERSPECTIVE;
 glutPostRedisplay();
}

void stepDisplay()
//Glut idle callback for animation.
{
  for (int i=0; i<speed; i++)
    {
      step(chaseCarState);
      for (int r=0; r<NUM_RABBITS; r++) step(rabbits[r]);
    }
  glutPostRedisplay();
}


void trackSubMenu( int item )
// Callback for processing camera set aim submenu.
{
  switch ( item )
  {
    case 0 : tracking = ORIGIN; track(); break;
    case 1 : tracking = CART_CENTER; track(); break;
    case 2 : tracking = CART_FRONT; track(); break;
    case 3 : tracking = CART_MIDDLE; track(); break;
    case 4 : tracking = CART_REAR; track(); break;
    case 5 : tracking = NONE; track(); break;
  }
  operation = NOTHING;
  glutPostRedisplay();
}

void randomSubMenu( int item )
// Callback for processing randomization submenu.
{
  switch ( item )
  {
    case 0 : chaseCarState.randomize = true; break;
    case 1 : chaseCarState.randomize = false; break;
  }
}

void rideSubMenu( int item )
// Callback for processing camera set ride submenu.
{
  switch ( item )
  {
    case 0 : riding = ORIGIN; ride(); break;
    case 1 : riding = CART_CENTER; ride(); break;
    case 2 : riding = CART_FRONT; ride(); break;
    case 3 : riding = CART_MIDDLE; ride(); break;
    case 4 : riding = CART_REAR; ride(); break;
    case 5 : riding = NONE; ride(); break;
  }
  operation = NOTHING;
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
  operation = NOTHING;
  glutPostRedisplay();
}



void keyboardNonAsciiFunction( int key, int, int)
{
  switch (key)
    {
    case GLUT_KEY_LEFT  : chaseCarState.steering=LEFT;     break;
    case GLUT_KEY_UP    : chaseCarState.steering=STRAIGHT; break;
    case GLUT_KEY_RIGHT : chaseCarState.steering=RIGHT;    break;
    }
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



void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
  case 0 : operation = ANIMATE; glutIdleFunc(stepDisplay); break;
  case 1 : operation = ZOOM; break;
  case 2 : operation = SPEED; break;
  case 3 : operation = TIME; break;
  case 4 : operation = PROJECTION; break;
  case 5 : homePosition();glutPostRedisplay(); break;
  case 6 : std::exit( 0 );
 }
}

void setMenus( )
// Routine for creating menus.
{
  int trackSubMenuCode, rideSubMenuCode, randomSubMenuCode;
  int aimSubMenuCode, positionSubMenuCode, orientationSubMenuCode;

  trackSubMenuCode = glutCreateMenu( trackSubMenu );
  glutAddMenuEntry( "Origin",      0 );
  glutAddMenuEntry( "Cart Center", 1 );
  glutAddMenuEntry( "Cart Front", 2 );
  glutAddMenuEntry( "Cart Middle", 3 );
  glutAddMenuEntry( "Cart Rear", 4 );
  glutAddMenuEntry( "None",       5 );

  rideSubMenuCode = glutCreateMenu( rideSubMenu );
  glutAddMenuEntry( "Origin",      0 );
  glutAddMenuEntry( "Cart Center", 1 );
  glutAddMenuEntry( "Cart Front", 2 );
  glutAddMenuEntry( "Cart Middle", 3 );
  glutAddMenuEntry( "Cart Rear", 4 );
  glutAddMenuEntry( "None",       5 );

  randomSubMenuCode = glutCreateMenu( randomSubMenu );
  glutAddMenuEntry( "On",  0 );
  glutAddMenuEntry( "Off", 1 );


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

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Set Body Tracking  ...",  trackSubMenuCode );
  glutAddSubMenu( "Set Body Riding  ...",  rideSubMenuCode );
  glutAddSubMenu( "Change Camera Aim ...",  aimSubMenuCode );
  glutAddSubMenu( "Change Camera Position ...", positionSubMenuCode );
  glutAddSubMenu( "Change Camera Orientation ...", orientationSubMenuCode );
  glutAddSubMenu( "Randomization  ...",  randomSubMenuCode );
  glutAddMenuEntry( "Animate ", 0 );
  glutAddMenuEntry( "Zoom",  1 );
  glutAddMenuEntry( "Speed",  2 );
  glutAddMenuEntry( "Step",  3 );
  glutAddMenuEntry( "Change Projection",  4);
  glutAddMenuEntry( "Home Position",  5);
  glutAddMenuEntry( "Exit",  6 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}


void init()
{


  GLfloat matSpecular[] = {0.0, 0.0, 0.0, 0.0};
  GLfloat shininess[] = {1.0};
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialfv(GL_FRONT, GL_SHININESS, shininess);


  glEnable(GL_LIGHTING);

  GLfloat light_ambient[] = {.5, .5, .5, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  GLfloat light_diffuse[] = {.5, .5, .5, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

  GLfloat light_position[] = {0.0, 0.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_LIGHT0);

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel (GL_SMOOTH);



  trackWidth = TRACK_WIDTH_FACTOR * CART_BASE_SIZE+2.0*WHEEL_RADIUS2;
  trackOuterRadius = trackRadius + trackWidth/2.0;
  trackInnerRadius = trackRadius - trackWidth/2.0;

  radiusRatio = trackRadius/wheelRadius;
  trackDegreesPerStep = 360.0/(4*numCartRevolutionSteps);
  wheelDegreesPerStep = trackDegreesPerStep*radiusRatio;

  trackLengthPerStep = 2*trackRadius/numCartTranslationSteps;

  numTileRows = TILE_ROWS_PER_CIRCLE*NUM_CIRCLES+1;
  tileSize = 2*trackRadius/TILE_ROWS_PER_CIRCLE;
  extente = numTileRows*tileSize;

  theTrack = glGenLists(1);
  glNewList(theTrack, GL_COMPILE);
  drawTrack();
  glEndList();

  theCartBody = glGenLists(2);
  glNewList(theCartBody, GL_COMPILE);
  drawCartBase();
  drawCartAxles();
  drawCartLights();
  drawCartCanopy();
  glEndList();




  aWheel = glGenLists(3);
  glNewList(aWheel, GL_COMPILE);
  GLfloat wheelDiffuse[] = {0.25, 0.25, 0.25, 1.0};
  GLfloat wheelAmbient[] = {0.35, 0.35, 0.35, 1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, wheelAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, wheelDiffuse);
  glutSolidTorus(WHEEL_RADIUS2,WHEEL_RADIUS1,WHEEL_SLICES,WHEEL_DICES);
  drawSpokes();
  glEndList();

}


int main(int argc, char** argv)
{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize (600, 600);
   glutInitWindowPosition (50, 50);
   glutCreateWindow ("Array Track Simulation");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc( mouse );
   glutSpecialFunc( keyboardNonAsciiFunction );
   setMenus();

   glEnable( GL_DEPTH_TEST );


   spoke = gluNewQuadric();
   gluQuadricDrawStyle(spoke,(GLenum)GLU_FILL);

   ring = gluNewQuadric();
   gluQuadricDrawStyle(ring,(GLenum)GLU_FILL);

   sphere = gluNewQuadric();
   gluQuadricDrawStyle(sphere,(GLenum)GLU_FILL);

   cylinder = gluNewQuadric();
   gluQuadricDrawStyle(cylinder,(GLenum)GLU_FILL);

   glMatrixMode(GL_MODELVIEW);

   homePosition();

   init();

   srand(clock());

   glutMainLoop();
   return 0;
}
