/*--------------------------------------------------------*/
/*  CS-395           Computer Graphics       Spring 2001  */
/*--------------------------------------------------------*/
/*  roller-coaster.cc : Roller Coaster                    */
/*--------------------------------------------------------*/

#include <cstdlib>

  #include <GL/glut.h>
  #include "roller-coaster-step.h"



#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <cfloat>

using namespace std;

#define TRACK_SLICES 36
#define TRACK_RINGS 1
#define SPHERE_RADIUS 0.015
#define SPHERE_SLICES 24
#define SPHERE_STACKS 24
#define CYLINDER_BASE 2.0
#define CYLINDER_HEIGHT 2.0
#define CYLINDER_SLICES 12
#define CYLINDER_STACKS 1
#define NUM_CIRCLES 3
#define STRETCH 1.325
#define CAR_BASE_SIZE 0.1
#define CAR_ASPECT_X 1.0
#define CAR_ASPECT_Y 2.0
#define CAR_ASPECT_Z 0.5
#define WHEEL_RADIUS1 0.0375
#define WHEEL_RADIUS2 0.0125
#define WHEEL_SLICES 10
#define WHEEL_DICES  20
#define CAR_BASE_HEIGHT (WHEEL_RADIUS1 + WHEEL_RADIUS2)
#define SPOKE_SLICES 10
#define SPOKE_STACKS 3
#define SPOKE_RADIUS 0.00125
#define DELTA 0.005
#define EPSILON 0.005
#define TRACK_RADIUS 1.6
#define TRACK_HEIGHT  0.675
#define TRACK_WIDTH_FACTOR 1.0
#define BANK_ANGLE 0.0
#define FREQUENCY 4
#define DT 0.0005
#define NUM_LANDMARKS 6
#define NUM_CARS 4
#define G 9.8
#define INITIAL_VELOCITY 60.0
#define INITIAL_POSITION 360.0
#define INITIAL_SPEED 4
#define CAR_LENGTH CAR_ASPECT_Y*CAR_BASE_SIZE*STRETCH
#define PI 3.14159265359
#define TwoPI 6.28318530718
#define DTR 0.01745329251994  /* Degrees to radians factor. */
#define RTD 57.29577951308    /* Radians to degrees factor. */

//Constants defining the step increments for changing
//the location and the aim of the camera.
#define EYE_STEP 0.1
#define CEN_STEP 0.1
#define ZOOM_FACTOR 1.414
#define INITIAL_FOV 90.0

//System constants determined at initialization;
GLdouble trackInnerRadius;
GLdouble trackOuterRadius;
GLdouble trackWidth;
GLdouble bank;

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
typedef enum { TRACKING, RIDING, POSITION, AIM, ORIENTATION, ZOOM, HOME, SPEED, TIME, PROJECTION, ANIMATE, NOTHING} operationType;
operationType operation = POSITION;

//Enumerated type and global variable for keeping track
//of the object the user has selected to track with the camera
//and the object the user has selected for the camera to ride.
typedef enum { CAR_CENTER, CAR_FRONT, CAR_MIDDLE, CAR_REAR, ORIGIN, NONE } trackingType;
trackingType tracking = NONE;
trackingType riding = NONE;
int ridingCar = 0;
int trackingCar = 1;

//Enumerated type and global variable for talking about axies.
typedef enum { X, Y, Z } axisType;
axisType axis = Z;

//Global variables for keeping track of the speed of the animation.
int speed = INITIAL_SPEED;

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

//Global variable defining the field of view.
GLdouble fov = 90;

//Global variables for keeping track of the camera aim.
GLdouble xCen = 0.0;
GLdouble yCen = 0.0;
GLdouble zCen = 0.0;

//Global variables for keeping track of the camera orientation.
GLdouble xUp = 0.0;
GLdouble yUp = 1.0;
GLdouble zUp = 0.0;


// Scene Constants:
const GLdouble trackRadius = TRACK_RADIUS;
const GLdouble wheelRadius = CAR_BASE_HEIGHT;


struct state
{
  float carRevolution[NUM_CARS];
  float carRevolutionDot[NUM_CARS];
  float wheelRevolution[NUM_CARS];
};

state carState;

GLuint theCarBody;
GLuint aWheel;

// Flag to controll beep in event of collisions.
bool beepOn = false;

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

void normalize(float* angle)
//Normalizing an angle to put it in the range: -180.0 < angle <= 180.0
{
  while (*angle >= PI) *angle -= TwoPI;
  while (*angle < -PI) *angle += TwoPI;
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


void drawCarBase()
// Draw the car base, relative to car coordinates.
{
   glPushMatrix();
   glScaled ((GLdouble) CAR_ASPECT_X*1.05, (GLdouble) CAR_ASPECT_Y*1.25, (GLdouble) CAR_ASPECT_Z);
   GLfloat carBaseDiffuse[] = {1.0, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, carBaseDiffuse);
   GLfloat carBaseAmbient[] = {1.0, 0.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, carBaseAmbient);
   gluSphere(sphere,CAR_BASE_SIZE/2.0,SPHERE_SLICES*4,SPHERE_STACKS);
   glPopMatrix();
}

void drawCarAxles()
// Draw the car axles, relative to car coordinates.
{
   GLfloat carAxlesDiffuse[] = {0.35, 0.35, 0.35, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, carAxlesDiffuse);
   GLfloat carAxlesAmbient[] = {0.35, 0.35, 0.35, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, carAxlesAmbient);

   glPushMatrix();

   glRotated(90.0,0.0,1.0,0.0);

   glPushMatrix();
   glTranslated(0.0,CAR_BASE_SIZE*CAR_ASPECT_Y/2-WHEEL_RADIUS1,-CAR_BASE_SIZE*CAR_ASPECT_X/2-WHEEL_RADIUS2);
   gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,CAR_BASE_SIZE*CAR_ASPECT_X+2*WHEEL_RADIUS2,SPOKE_SLICES,SPOKE_STACKS);
   glPopMatrix();

   glPushMatrix();
   glTranslated(0.0,-CAR_BASE_SIZE*CAR_ASPECT_Y/2+WHEEL_RADIUS1,-CAR_BASE_SIZE*CAR_ASPECT_X/2-WHEEL_RADIUS2);
   gluCylinder(spoke,SPOKE_RADIUS,SPOKE_RADIUS,CAR_BASE_SIZE*CAR_ASPECT_X+2*WHEEL_RADIUS2,SPOKE_SLICES,SPOKE_STACKS);
   glPopMatrix();

   glPopMatrix();
}



void drawCarCanopy()
// Draw the car canopy.
{
   glPushMatrix();
   glTranslated (0.0,0.0,CAR_BASE_HEIGHT/2.0);
   glScaled (1.0,2.0,1.0);
   GLfloat carCanopyDiffuse[] = {1.0, 1.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, carCanopyDiffuse);
   GLfloat carCanopyAmbient[] = {1.0, 1.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, carCanopyAmbient);
   gluSphere(sphere,CAR_BASE_SIZE/4.0,SPHERE_SLICES*4,SPHERE_STACKS);
   glPopMatrix();
}

void drawSpokes()
// Draw wheel spokes, relative to wheel coordinates.
{
  glColor3f(1.0,1.0,0.0);
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
}


void gotoWheelCoordinates(int lr, int fb, state& carState, int numCar)
// Move to a coordinate system at one of the wheels, assuming
// already at car coordinates.
{
   glRotated(90.0, 0.0, 1.0, 0.0);
   glTranslated (0.0,
                 fb*(CAR_BASE_SIZE*CAR_ASPECT_Y/2-WHEEL_RADIUS1),
                 lr*(CAR_BASE_SIZE*CAR_ASPECT_X/2+WHEEL_RADIUS2));
   glRotated(RTD*carState.wheelRevolution[numCar],0.0,0.0,1.0);
}



void  drawCarWheels(state& carState, int numCar)
// Draw four wheels, relative to car coordinates.
{
  glPushMatrix();
  gotoWheelCoordinates(1,1,carState,numCar);
  glCallList(aWheel);
  glPopMatrix();
  glPushMatrix();
  gotoWheelCoordinates(1,-1,carState,numCar);
  glCallList(aWheel);
  glPopMatrix();
  glPushMatrix();
  gotoWheelCoordinates(-1,1,carState,numCar);
  glCallList(aWheel);
  glPopMatrix();
  glPushMatrix();
  gotoWheelCoordinates(-1,-1,carState,numCar);
  glCallList(aWheel);
  glPopMatrix();
}

void gotoLightCoordinates(int lr, int fb)
// Move to a coordinate system at one of the lights, assuming
// already at car coordinates.
{
   glTranslated ((1.0/2.0)*lr*(CAR_BASE_SIZE*CAR_ASPECT_X/2),
                 fb*(CAR_BASE_SIZE*CAR_ASPECT_Y/2),
                 0.0);
}



void  drawCarLights()
// Draw four wheels, relative to car coordinates.
{
  //glColor3f (1.0, 1.0, 0.0);

  GLfloat ambient[] = {1, 1, 0, 1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
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







void gotoCarCoordinates(state& carState,int num)
// Move to a car centered coordinate system, assuming presently
// at the origin.
{
  double alphaRadians = FREQUENCY*carState.carRevolution[num];

  double betaRadians = atan(FREQUENCY*TRACK_HEIGHT*cos(alphaRadians)/trackRadius);

  glRotated (carState.carRevolution[num]*RTD, 0.0, 0.0, 1.0);

  glTranslated(trackInnerRadius+trackWidth/2,
               0.0,
	       (TRACK_HEIGHT*(1.0+sin(alphaRadians))) - TRACK_HEIGHT);


  glRotated(betaRadians*RTD,1.0, 0.0, 0.0);

  glTranslated(0.0,0.0,wheelRadius);
}



void drawCar(state& carState)
// Draw the car and robot system, relative to the origin.
{
 int i;
 for(i=0; i<NUM_CARS; i++) {
   glPushMatrix();
   gotoCarCoordinates(carState,i);
   glCallList(theCarBody);
   drawCarWheels(carState,i);
   glPopMatrix();
 }
}



void drawTrack()
// Draw the track, relative to the origin.
{

  double i;
  double r1 = trackInnerRadius;
  double r2 = trackOuterRadius;

  glNormal3f(0.0,0.0,1.0);

	GLfloat diffuse1[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat ambient1[] = {1.0, 1.0, 1.0, 1.0};

	GLfloat diffuse2[] = {.2, .2, .2, 1.0};
	GLfloat ambient2[] = {.2, .2, .2, 1.0};

  glBegin(GL_QUADS);
  double twoPi = 2 * PI;
  for(i=0.0; i<=360.0; i+=2){
	  if ((int)i%4 == 0)
	  {
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse1);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient1);
	  }
	  else
	  {
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse2);
		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient2);
	  }
    double alpha = (i/360.0)*twoPi;
    glVertex3f(r1*cos(alpha), r1*sin(alpha), (TRACK_HEIGHT*(sin(FREQUENCY*alpha))) - bank);
    glVertex3f(r2*cos(alpha), r2*sin(alpha), (TRACK_HEIGHT*(sin(FREQUENCY*alpha))) + bank);
	double alphaNext = ((i+2)/360.0)*twoPi;
    glVertex3f(r2*cos(alphaNext), r2*sin(alphaNext), (TRACK_HEIGHT*(sin(FREQUENCY*alphaNext))) + bank);
	glVertex3f(r1*cos(alphaNext), r1*sin(alphaNext), (TRACK_HEIGHT*(sin(FREQUENCY*alphaNext))) - bank);
  }
  glEnd();

}


void getCarPosition(GLdouble* x, GLdouble* y, GLdouble* z, trackingType where, state& carState, int car)
//Finding the current position of the car.
{
  glPushMatrix();
  glLoadIdentity();
  gotoCarCoordinates(carState,car);
  switch (where)
    {
    case  CAR_FRONT:  glTranslated (0.0, CAR_BASE_SIZE*CAR_ASPECT_Y/2-WHEEL_RADIUS1,3.0*CAR_BASE_HEIGHT); break;
    case  CAR_MIDDLE:   glTranslated (0.0,0.0,3.0*CAR_BASE_HEIGHT); break;
    case  CAR_REAR:   glTranslated (0.0,-CAR_BASE_SIZE*CAR_ASPECT_Y/2-WHEEL_RADIUS1,3.0*CAR_BASE_HEIGHT); break;
    case  CAR_CENTER: break;
     default: break;
    }
  getCurrentLocation(x,y,z);
  glPopMatrix();
}



void drawLandmarks()
{

  GLfloat landmarkDiffuse[] = {0.0, 0.0, 1.0, 1.0};
  glMaterialfv(GL_FRONT, GL_DIFFUSE, landmarkDiffuse);

   GLfloat landmarkAmbient[] = {0.0, 0.0, 1.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, landmarkAmbient);

  for (int i = 0; i < NUM_LANDMARKS; i++)
    {
      glPushMatrix();
      glRotated(i*360.0/NUM_LANDMARKS, 0.0, 0.0, 1.0);
      glTranslated(TRACK_RADIUS*5.0,0.0,-TRACK_HEIGHT);
      gluCylinder(cylinder,CYLINDER_BASE,0.0,CYLINDER_HEIGHT,CYLINDER_SLICES,CYLINDER_STACKS);
      glPopMatrix();
    }
}


void drawGround()
{
   GLfloat groundDiffuse[] = {0.0, 0.8, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_DIFFUSE, groundDiffuse);

   GLfloat groundAmbient[] = {0.0, 1.0, 0.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, groundAmbient);

   glNormal3f(0.0,0.0,1.0);

   glBegin(GL_POLYGON);
   glVertex3f(-10*trackRadius,-10*trackRadius,-TRACK_HEIGHT-0.1);
   glVertex3f( 10*trackRadius,-10*trackRadius,-TRACK_HEIGHT-0.1);
   glVertex3f( 10*trackRadius, 10*trackRadius,-TRACK_HEIGHT-0.1);
   glVertex3f(-10*trackRadius, 10*trackRadius,-TRACK_HEIGHT-0.1);
   glEnd();

}

void drawSky()
{

   GLfloat light_ambient_bright[] = {1.0, 1.0, 1.0, 1.0};
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient_bright);

   GLfloat skyAmbient[] = {0.7, .7, 1.0, 1.0};
   glMaterialfv(GL_FRONT, GL_AMBIENT, skyAmbient);

   gluSphere(sphere,9*trackRadius,SPHERE_SLICES*4,SPHERE_STACKS);

   GLfloat light_ambient[] = {.25, .25, .25, 1.0};
   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

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
   else  gluPerspective(fov, (GLdouble) windowWidth / (GLdouble) windowHeight, 0.01, 200.0);
   glMatrixMode(GL_MODELVIEW);


   drawGround();
   drawSky();
   drawLandmarks();
   drawTrack();

   drawCar(carState);

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
       case ORIGIN         : xCen=yCen=zCen=0.0; break;
       case CAR_CENTER    : getCarPosition(&xCen,&yCen,&zCen,CAR_CENTER,carState,trackingCar); break;
       case CAR_FRONT     : getCarPosition(&xCen,&yCen,&zCen,CAR_FRONT,carState,trackingCar); break;
       case CAR_MIDDLE    : getCarPosition(&xCen,&yCen,&zCen,CAR_MIDDLE,carState,trackingCar); break;
       case CAR_REAR      : getCarPosition(&xCen,&yCen,&zCen,CAR_REAR,carState,trackingCar); break;
       default: break;
     }
}

void ride()
//Setting global variables that define the location of the camera.
{
   switch (riding)
     {
       case ORIGIN  : xEye=yEye=zEye=0.0; break;
       case CAR_CENTER    : getCarPosition(&xEye,&yEye,&zEye,CAR_CENTER,carState,ridingCar); break;
       case CAR_FRONT     : getCarPosition(&xEye,&yEye,&zEye,CAR_FRONT,carState,ridingCar); break;
       case CAR_MIDDLE    : getCarPosition(&xEye,&yEye,&zEye,CAR_MIDDLE,carState,ridingCar); break;
       case CAR_REAR      : getCarPosition(&xEye,&yEye,&zEye,CAR_REAR,carState,ridingCar); break;
       default: break;
     }
}

void processCollision(int c1, int c2)
{
  if (beepOn) cout << "\007" << flush;
  float v1, v2, v1Prime, v2Prime, u1Prime, u2Prime, u1, u2, vCM;
  v1 = carState.carRevolutionDot[c1];
  v2 = carState.carRevolutionDot[c2];
  vCM = (v1+v2)/2.0;
  v1Prime = v1-vCM;
  v2Prime = v2-vCM;
  u1Prime = -v1Prime;
  u2Prime = -v2Prime;
  u1 = u1Prime + vCM;
  u2 = u2Prime + vCM;
  carState.carRevolutionDot[c1] = u1;
  carState.carRevolutionDot[c2] = u2;
}


void step(state& carState)
//Move forward or backward in time and change state variables accordingly.
{
  for (int i = 0; i<speed; i++)
    {
      coasterInitialize(NUM_CARS, FREQUENCY, TRACK_HEIGHT, trackRadius, wheelRadius, G);
      coasterStep(carState.carRevolution,carState.carRevolutionDot,carState.wheelRevolution,DT);

      for (int i = 0; i<NUM_CARS; i++)
	{
	  normalize(&carState.carRevolution[i]);
	  normalize(&carState.wheelRevolution[i]);
	}
      for (int c1=0; c1<NUM_CARS-1; c1++)
	for (int c2=c1+1; c2<NUM_CARS; c2++)
	  {
	    float theta1 = carState.carRevolution[c1];
	    float theta1Dot = carState.carRevolutionDot[c1];
	    float theta2 = carState.carRevolution[c2];
	    float theta2Dot = carState.carRevolutionDot[c2];
	    float RR = trackRadius*trackRadius;
	    float ff = FREQUENCY*FREQUENCY;
	    float hh = TRACK_HEIGHT*TRACK_HEIGHT;
	    float cosine = cos(FREQUENCY*(theta1+theta2)/2.0);
	    float separation = fabs((theta2-theta1)*sqrt(RR+hh*ff*cosine*cosine));
	    if (separation<float(7.0*CAR_LENGTH/8.0)
               && ( (theta2>theta1 && theta1Dot>theta2Dot) || (theta1>theta2 && theta2Dot>theta1Dot) ) )
	      processCollision(c1,c2);
	  }
    }


//      for (int i = 0; i<NUM_CARS; i++)
//	{
//	  float RR = trackRadius*trackRadius;
//	  float ff = FREQUENCY*FREQUENCY;
//	  float hh = TRACK_HEIGHT*TRACK_HEIGHT;
//	  float cosine = cos(FREQUENCY*carState.carRevolution[i]);
//	  float velocity = carState.carRevolutionDot[i]*sqrt(RR+hh*ff*cosine*cosine);
//	  float height = TRACK_HEIGHT*sin(FREQUENCY*carState.carRevolution[i]);
//	  //  cout << "Height: " << height << endl;
//	  //  cout << "Velocity Squared: " << velocity*velocity << endl << endl;
//	  cout << "Total Energy of Car: " <<  i << "is: " << .5*velocity*velocity + G*height << endl;
//	}

  track();
  ride();
}

void initialState(state& carState)
{
  for (int i = 0; i < NUM_CARS; i++)
    {
      //      carState.carRevolution[i] = DTR*(INITIAL_POSITION+360.0*float(i)/float(NUM_CARS));
      //      carState.carRevolutionDot[i] = (i%2==0) ? DTR*INITIAL_VELOCITY : -DTR*INITIAL_VELOCITY;

      carState.carRevolution[i] = DTR*(INITIAL_POSITION*float(rand()/(double)RAND_MAX));
      carState.carRevolutionDot[i] = DTR*(INITIAL_VELOCITY*((rand()/(double)RAND_MAX)-0.5));
      carState.wheelRevolution[i] = 0.0;

    }

  coasterInitialize(NUM_CARS, FREQUENCY, TRACK_HEIGHT, trackRadius, wheelRadius, G);
}

void homePosition()
//Put the car state and all viewing varibles back to their original values.
{
 initialState(carState);

 axis = Z;
 direction = UP;
 operation = NOTHING;

 xEye = 1.0;
 yEye = 0.0;
 zEye = 0.0;
 xCen = 0.0;
 yCen = 0.0;
 zCen = 0.0;
 xUp = 0.0;
 yUp = 0.0;
 zUp = 1.0;

 fov = INITIAL_FOV;

 tracking = CAR_CENTER;
 riding = ORIGIN;

 speed = INITIAL_SPEED;

 glLoadIdentity();

 track();
 ride();

 glutPostRedisplay();
}


void operate()
//Process the operation that the user has selected.
{
 if (operation==TIME)
   {
     step(carState);
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
	   riding = NONE;
           if (direction == DOWN)
	     {
	       fov *= ZOOM_FACTOR;
	       if (fov>=180.0) fov=180.0;
	     }
	   else fov /= ZOOM_FACTOR;
	 }
 else if (operation==PROJECTION)
      {
	if (direction == UP) projection = ORTHOGRAPHIC;
	else projection = PERSPECTIVE;
      }
 else if (operation == TRACKING)
     {
       if (direction == UP) trackingCar = (trackingCar+1)%NUM_CARS;
       else trackingCar = (trackingCar-1)%NUM_CARS;
       track();
     }
 else if (operation == RIDING)
     {
       if (direction == UP) ridingCar = (ridingCar+1)%NUM_CARS;
       else ridingCar = (ridingCar-1)%NUM_CARS;
       ride();
     }
 glutPostRedisplay();
}

void stepDisplay()
//Glut idle callback for animation.
{
  step(carState);
  glutPostRedisplay();
}


void trackSubMenu( int item )
// Callback for processing camera set aim submenu.
{
  switch ( item )
  {
    case 0 : tracking = ORIGIN; track(); break;
    case 1 : tracking = CAR_CENTER; track(); break;
    case 2 : tracking = CAR_FRONT; track(); break;
    case 3 : tracking = CAR_MIDDLE; track(); break;
    case 4 : tracking = CAR_REAR; track(); break;
    case 5 : tracking = NONE; track(); break;
  }
  operation = TRACKING;
  glutPostRedisplay();
}

void rideSubMenu( int item )
// Callback for processing camera set ride submenu.
{
  switch ( item )
  {
    case 0 : riding = ORIGIN; ride(); break;
    case 1 : riding = CAR_CENTER; ride(); break;
    case 2 : riding = CAR_FRONT; ride(); break;
    case 3 : riding = CAR_MIDDLE; ride(); break;
    case 4 : riding = CAR_REAR; ride(); break;
    case 5 : riding = NONE; ride(); break;
  }
  operation = RIDING;
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

void beepSubMenu( int item )
// Callback for processing camera position submenu.
{
  switch ( item )
  {
    case 1 : beepOn = false; break;
    case 2 : beepOn = true; break;
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
  int trackSubMenuCode, rideSubMenuCode;
  int aimSubMenuCode, positionSubMenuCode, orientationSubMenuCode;
  int beepSubMenuCode;

  trackSubMenuCode = glutCreateMenu( trackSubMenu );
  glutAddMenuEntry( "Origin",      0 );
  glutAddMenuEntry( "Car Center", 1 );
  glutAddMenuEntry( "Car Front", 2 );
  glutAddMenuEntry( "Car Middle", 3 );
  glutAddMenuEntry( "Car Rear", 4 );
  glutAddMenuEntry( "None",       5 );

  rideSubMenuCode = glutCreateMenu( rideSubMenu );
  glutAddMenuEntry( "Origin",      0 );
  glutAddMenuEntry( "Car Center", 1 );
  glutAddMenuEntry( "Car Front", 2 );
  glutAddMenuEntry( "Car Middle", 3 );
  glutAddMenuEntry( "Car Rear", 4 );
  glutAddMenuEntry( "None",       5 );

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

  beepSubMenuCode = glutCreateMenu( beepSubMenu );
  glutAddMenuEntry( "Off",      1 );
  glutAddMenuEntry( "On",      2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Set Body Tracking  ...",  trackSubMenuCode );
  glutAddSubMenu( "Set Body Riding  ...",  rideSubMenuCode );
  glutAddSubMenu( "Change Camera Aim ...",  aimSubMenuCode );
  glutAddSubMenu( "Change Camera Position ...", positionSubMenuCode );
  glutAddSubMenu( "Change Camera Orientation ...", orientationSubMenuCode );
  glutAddMenuEntry( "Animate ", 0 );
  glutAddSubMenu( "Beep ...", beepSubMenuCode);
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
  glEnable(GL_LIGHTING);

  GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 0.0 };
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

  GLfloat light_ambient[] = {.25, .25, .25, 1.0};
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  GLfloat light_diffuse[] = {.5, .5, .5, 1.0};
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  GLfloat light_specular[] = {.25, .25, .25, 1.0};
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

  GLfloat light_position[] = {0.0, 1.0, 1.0, 0.0};
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_LIGHT0);

  glClearColor (0.0, 0.0, 0.0, 0.0);
  glShadeModel (GL_SMOOTH);

  trackWidth = TRACK_WIDTH_FACTOR*CAR_BASE_SIZE+4.0*WHEEL_RADIUS2;
  trackOuterRadius = trackRadius + trackWidth/2.0;
  trackInnerRadius = trackRadius - trackWidth/2.0;
  bank = tan(BANK_ANGLE)*trackWidth/2.0;

  theCarBody = glGenLists(2);
  glNewList(theCarBody, GL_COMPILE);
  drawCarBase();
  drawCarAxles();
  drawCarLights();
  drawCarCanopy();
  glEndList();

  aWheel = glGenLists(3);
  glNewList(aWheel, GL_COMPILE);
  GLfloat wheelAmbient[] = {0.15, 0.15, 0.15, 1.0};
  glMaterialfv(GL_FRONT, GL_AMBIENT, wheelAmbient);
  GLfloat wheelDiffuse[] = {0.75, 0.75, 0.75, 1.0};
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
   glutCreateWindow ("Roller Coaster");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc( mouse );
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

   init();

   glMatrixMode(GL_MODELVIEW);

   // Initialize the random number generator.
   srand( (unsigned) time(NULL));

   homePosition();

   glutMainLoop();
   return 0;
}
