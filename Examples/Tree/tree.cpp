/*--------------------------------------------------------*/
/*  CS-395           Computer Graphics       Tom Ellman   */
/*--------------------------------------------------------*/
/*  tree.cpp : Recursive Tree Program                     */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <cfloat>
#include <ctime>

using namespace std;


// Needed for degrees/radians conversions
#define PI 3.14159265358979

// Tree display parameters.
#define SLICES 10
#define STACKS 10
#define DEPTH_OFFSET -1.0
#define HEIGHT_OFFSET -0.5
#define INITIAL_RADIUS 0.0125
#define INITIAL_HEIGHT 0.25
#define INITIAL_RADIUS_REDUCTION_FACTOR 0.5
#define INITIAL_LENGTH_REDUCTION_FACTOR 0.5
#define INITIAL_AXIAL_SYMMETRY_ORDER 2
#define INITIAL_BRANCH_ANGLE 25.0
#define INITIAL_RECURSION_DEPTH 1
#define INITIAL_ROTATION_FACTOR 1

// Tree change parameters.
#define RADIUS_STEP 0.005
#define HEIGHT_STEP 0.1
#define RADIUS_REDUCTION_FACTOR_STEP 0.05
#define LENGTH_REDUCTION_FACTOR_STEP 0.05
#define AXIAL_SYMMETRY_ORDER_STEP 1
#define BRANCH_ANGLE_STEP 1.0
#define RECURSION_DEPTH_STEP 1
#define ROTATION_FACTOR_STEP 1


//Constants defining the viewing arrangement:
#define NEAR_CLIP 0.1
#define FAR_CLIP 200
#define LEFT_RIGHT_CLIP 2.0
#define BOTTOM_TOP_CLIP 2.0
#define BACK_CLIP 10.0
#define FRONT_CLIP -10.0
#define DEPTH_SHIFT -2.0
#define INITIAL_FOV  60.0

// Constants for defining changes to the viewing setup.
#define FOV_STEP 5.0
#define ROTATION_STEP 3.0
#define TRANSLATION_STEP 0.1
#define SCALE_FACTOR 1.1

// Constants for defining changes to the lighting setup.
#define INITIAL_SPECULAR 0.3
#define SPECULAR_STEP 0.1
#define INITIAL_SHININESS 100.0
#define SHININESS_STEP 5.0
#define DIFFUSE_STEP 0.1
#define INITIAL_DIFFUSE_RED 0.0
#define INITIAL_DIFFUSE_GREEN 1.0
#define INITIAL_DIFFUSE_BLUE 0.0

// Constants defining amount and changes to randomness.
#define INITIAL_RANDOMNESS 0.0
#define RANDOMNESS_STEP 0.05


//Constant for initialization of max and min loops.
#define INFINITY 10000

//Globals defining the tree:
GLdouble radius = INITIAL_RADIUS;
GLdouble height = INITIAL_HEIGHT;
GLdouble radiusReductionFactor = INITIAL_RADIUS_REDUCTION_FACTOR;
GLdouble lengthReductionFactor = INITIAL_LENGTH_REDUCTION_FACTOR;
GLdouble nthrtRadiusReductionFactor;
int axialSymmetryOrder = INITIAL_AXIAL_SYMMETRY_ORDER;
GLdouble branchAngle = INITIAL_BRANCH_ANGLE;
int rotationFactor = INITIAL_ROTATION_FACTOR;
int recursionDepth = INITIAL_RECURSION_DEPTH;
GLdouble randomness = INITIAL_RANDOMNESS;

// Flag to indicate type of projection.
typedef enum { PERSPECTIVE, ORTHOGRAPHIC } projectionType;
projectionType projection = PERSPECTIVE;

// Global variable for storing the model view matrix.
GLdouble modelViewMatrix[ 16 ];

//Global variables for keeping track of the size and shape
//of the window.
int windowHeight, windowWidth;

//Enumerated type and global variable for keeping track
//of the selected operation.
typedef enum { SCALE, TRANSLATE, ROTATE, ZOOM, DIFFUSE, SPECULAR, SHININESS, HOME,
	       HEIGHT, RADIUS, BRANCH_ANGLE, AXIAL_SYMMETRY_ORDER, RADIUS_REDUCTION_FACTOR,
	       LENGTH_REDUCTION_FACTOR, ROTATION_FACTOR, RECURSION_DEPTH, RANDOMNESS } operationType;
operationType operation = ZOOM;

//Enumerated type and global variable for talking about axies.
typedef enum { X, Y, Z } axisType;
axisType axis = Z;

//Enumerated type and global variable for talking about
//direction of changes.
typedef enum { DOWN, UP } directionType;
directionType direction = UP;

//Global variables for keeping track of the field of view.
GLdouble fov = INITIAL_FOV, fovRadians = PI*INITIAL_FOV/180.0;
GLdouble near = NEAR_CLIP, far = FAR_CLIP;
GLdouble aspectRatio;
GLdouble left = -LEFT_RIGHT_CLIP, right = LEFT_RIGHT_CLIP;
GLdouble bottom = -BOTTOM_TOP_CLIP, top = BOTTOM_TOP_CLIP;
GLdouble front = FRONT_CLIP, back = BACK_CLIP;

// Values for defining centers of transformation.
GLdouble average[] = {0.0,HEIGHT_OFFSET,DEPTH_OFFSET,1.0};
GLdouble initialAverage[] = {0.0,HEIGHT_OFFSET,DEPTH_OFFSET,1.0};

// Variables to hold lighting description:
GLfloat shininess = INITIAL_SHININESS;
GLfloat specular = INITIAL_SPECULAR;
GLfloat diffuseRed = INITIAL_DIFFUSE_RED;
GLfloat diffuseGreen = INITIAL_DIFFUSE_GREEN;
GLfloat diffuseBlue = INITIAL_DIFFUSE_BLUE;
typedef enum { RED, GREEN, BLUE} colorType;
colorType color;

// Global variable to hold a quadric object for rendering trunk and branches.
GLUquadricObj *cylinder;

// Random number in range: [1-randomness,1+randomness].
GLdouble randomNumber()
{
  GLdouble r = GLdouble(rand()%INFINITY)/GLdouble(INFINITY);
  return (1-randomness)+r*2.0*randomness;
}


//Multiplying a vector by a matrix.
void multiplyMatrixVector(GLdouble* m, GLdouble* v, GLdouble* w, int size)
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



// Initialize the model view matrix.
void initModelViewMatrix()
{
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glGetDoublev(  GL_MODELVIEW_MATRIX, modelViewMatrix );
 multiplyMatrixVector(modelViewMatrix,initialAverage,average,4);
 average[0] = average[0]/average[3];
 average[1] = average[1]/average[3];
 average[2] = average[2]/average[3];
}

void setLighting()
// Initialize or reset the lighting arrangement.
{

   if (diffuseRed < 0.0) diffuseRed = 0.0;      if (diffuseRed > 1.0) diffuseRed = 1.0;
   if (diffuseGreen < 0.0) diffuseGreen = 0.0;  if (diffuseGreen > 1.0) diffuseGreen = 1.0;
   if (diffuseBlue < 0.0) diffuseBlue = 0.0;    if (diffuseBlue > 1.0) diffuseBlue = 1.0;

   if (specular < 0.0) specular = 0.0;          if (specular > 1.0) specular = 1.0;

   if (shininess < 0.0) shininess = 0.0;


   GLfloat mat_diffuse[] = { diffuseRed, diffuseGreen, diffuseBlue, 1.0 };
   GLfloat mat_specular[] = { specular, specular, specular, 1.0 };
   GLfloat mat_shininess[] = { shininess };

   glClearColor (0.5, 0.5, 0.5, 0.0);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_AUTO_NORMAL);
   glEnable(GL_NORMALIZE);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}


void tree(int n)
{
  GLdouble newR, oldR;
  oldR = radius;
  newR = radius*nthrtRadiusReductionFactor;
  if (n==0)
       {
	 GLdouble randomHeight = height*randomNumber();
         gluCylinder(cylinder,radius,radius*radiusReductionFactor,randomHeight,SLICES,STACKS);
         return;
       }
  else {
	 GLdouble randomHeight = height*randomNumber();
         glPushMatrix();
	 gluCylinder(cylinder,oldR,newR,randomHeight,SLICES,STACKS);
	 oldR=newR;
	 newR*=nthrtRadiusReductionFactor;
	 glTranslated(0.0,0.0,randomHeight);
	 for (int i = 0; i< axialSymmetryOrder; i++)
	   {
	     glRotated(randomNumber()*rotationFactor*180.0/axialSymmetryOrder,0.0,0.0,1.0);

	     for(int j=0; j<2; j++)
	       {
		 glRotated(randomNumber()*180.0,0.0,0.0,1.0);
		 {
		   GLdouble randomBranchAngle = branchAngle*randomNumber();
		   glPushMatrix();
		   glScaled(radiusReductionFactor,radiusReductionFactor,lengthReductionFactor);
		   glRotated(randomBranchAngle,1.0,0.0,0.0);
		   tree(n-1);
		   glPopMatrix();
		 }
		 gluCylinder(cylinder,oldR,newR,randomHeight/axialSymmetryOrder,SLICES,STACKS);
		 oldR=newR;
		 newR*=nthrtRadiusReductionFactor;
		 glTranslated(0.0,0.0,randomHeight/axialSymmetryOrder);
	       }
	   }
	 glScaled(radiusReductionFactor,radiusReductionFactor,lengthReductionFactor);
	 tree(n-1);
	 glPopMatrix();
       }
}



void displayTree()
{
   glMatrixMode(GL_MODELVIEW);
   glTranslated(0.0,0.0,DEPTH_OFFSET);
   glTranslated(0.0,HEIGHT_OFFSET,0.0);
   glRotated(-90.0,1.0,0.0,0.0);
   tree(recursionDepth);
}


void display()
//Callback for redisplaying the image.
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //Set the default camera position, aim and orientation.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glMultMatrixd(modelViewMatrix);

   //Set the projection type and clipping planes.
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   switch (projection)
     {
       case PERSPECTIVE  : gluPerspective(fov,aspectRatio,near,far); break;
       case ORTHOGRAPHIC : glOrtho(::left,::right,bottom,top,front,back); break;
     }

   // Draw the tree.
   displayTree();

   // Put the image on the screen.
   glutSwapBuffers();
}

void reshape (int w, int h)
//Callback for responding to reshaping of the display window.
{
   windowWidth = w;
   windowHeight = h;

   aspectRatio = GLdouble(w)/GLdouble(h);

   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   switch (projection)
     {
       case PERSPECTIVE  : gluPerspective(fov,aspectRatio,near,far); break;
       case ORTHOGRAPHIC : glOrtho(::left,::right,bottom,top,front,back); break;
     }
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void computeRoots()
{
 nthrtRadiusReductionFactor = pow(radiusReductionFactor,1.0/GLdouble(1+2*axialSymmetryOrder));
}


void homePosition()
//Putting the system and all viewing varibles back to their
//original states.
{
 axis = Z;
 direction = UP;
 operation = ZOOM;
 fov = INITIAL_FOV;
 fovRadians = PI*fov/180.0;

 computeRoots();

 glutPostRedisplay();
 initModelViewMatrix();
}

void rotate()
// Rotate the scene around the centroid of the nurb, in a direction
// parallel to the selected axis.
{

  GLdouble angle = ROTATION_STEP;
  if (direction == DOWN) angle *= -1.0;

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glTranslated(average[0],average[1],average[2]);
  switch (axis)
    {
       case X : glRotated(angle,1.0,0.0,0.0); break;
       case Y : glRotated(angle,0.0,1.0,0.0); break;
       case Z : glRotated(angle,0.0,0.0,1.0); break;
    }
  glTranslated(-average[0],-average[1],-average[2]);
  glMultMatrixd(modelViewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);

  multiplyMatrixVector(modelViewMatrix,initialAverage,average,4);
  average[0] = average[0]/average[3];
  average[1] = average[1]/average[3];
  average[2] = average[2]/average[3];
}

void translate()
// Translate the scene in a direction parallel to the selected axis.
{
  GLdouble deltaX, deltaY, deltaZ;
  deltaX = deltaY = deltaZ = 0.0;
  int sign;
  if (direction == UP) sign = 1; else sign = -1;
  switch (axis)
    {
       case X : deltaX = sign*TRANSLATION_STEP; break;
       case Y : deltaY = sign*TRANSLATION_STEP; break;
       case Z : deltaZ = sign*TRANSLATION_STEP; break;
    }

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glTranslated(deltaX,deltaY,deltaZ);
  glMultMatrixd(modelViewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);

  multiplyMatrixVector(modelViewMatrix,initialAverage,average,4);
  average[0] = average[0]/average[3];
  average[1] = average[1]/average[3];
  average[2] = average[2]/average[3];
}

void scale()
// Scale the scene from centroid of the nurb uniformly in all directions.
{

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glTranslated(average[0],average[1],average[2]);
  switch (direction)
    {
      case UP   : glScaled(SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR); break;
      case DOWN : glScaled(1/SCALE_FACTOR,1/SCALE_FACTOR,1/SCALE_FACTOR); break;
    }
  glTranslated(-average[0],-average[1],-average[2]);
  glMultMatrixd(modelViewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);

  multiplyMatrixVector(modelViewMatrix,initialAverage,average,4);
  average[0] = average[0]/average[3];
  average[1] = average[1]/average[3];
  average[2] = average[2]/average[3];
}

void operate()
//Process the operation that the user has selected.
{
  int sign;
  if (direction == UP) sign = 1; else sign = -1;
  switch (operation)
    {
      case SCALE     : scale(); break;
      case TRANSLATE : translate(); break;
      case ROTATE    : rotate(); break;
      case DIFFUSE   : switch (color)
	                 {
			    case RED  : diffuseRed += sign * DIFFUSE_STEP; break;
			    case GREEN  : diffuseGreen += sign * DIFFUSE_STEP; break;
			    case BLUE  : diffuseBlue += sign * DIFFUSE_STEP; break;
			 }
                        setLighting();
			break;
      case SPECULAR  :  specular += sign * SPECULAR_STEP;
	                setLighting();
			break;
      case SHININESS :  shininess += sign * SHININESS_STEP;
			setLighting();
			break;
      case ZOOM      :  fov += - sign * FOV_STEP;
	                fovRadians = PI*fov/180.0;
			break;
      case HOME      :  break;
      case HEIGHT               : height += sign * HEIGHT_STEP; break;
      case RADIUS               : radius += sign * RADIUS_STEP; break;
      case BRANCH_ANGLE         : branchAngle += sign * BRANCH_ANGLE_STEP; break;
      case AXIAL_SYMMETRY_ORDER : axialSymmetryOrder += sign * AXIAL_SYMMETRY_ORDER_STEP;
	                          computeRoots();
	                          break;
      case RADIUS_REDUCTION_FACTOR     : radiusReductionFactor += sign * RADIUS_REDUCTION_FACTOR_STEP;
	                                 computeRoots();
	                                 break;
      case LENGTH_REDUCTION_FACTOR     : lengthReductionFactor += sign * LENGTH_REDUCTION_FACTOR_STEP;
	                                 computeRoots();
	                                 break;
      case ROTATION_FACTOR      : rotationFactor += sign * ROTATION_FACTOR_STEP; break;
      case RECURSION_DEPTH      : recursionDepth += sign * RECURSION_DEPTH_STEP; break;
      case RANDOMNESS           : randomness += sign * RANDOMNESS_STEP;
	                          if (randomness<0.0) randomness=0.0;
				  else if (randomness>1.0) randomness=1.0;
				  break;
    }
  glutPostRedisplay();
}

void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 0 : operation = SCALE; break;
 case 1 : operation = ZOOM; break;
 case 5 : operation = SPECULAR; break;
 case 6 : operation = SHININESS; break;
 case 9 : homePosition();glutPostRedisplay(); break;
 case 21 : operation = HEIGHT; break;
 case 22 : operation = RADIUS; break;
 case 23 : operation = BRANCH_ANGLE; break;
 case 24 : operation = AXIAL_SYMMETRY_ORDER; break;
 case 25 : operation = RADIUS_REDUCTION_FACTOR; break;
 case 27 : operation = LENGTH_REDUCTION_FACTOR; break;
 case 28 : operation = ROTATION_FACTOR; break;
 case 26 : operation = RECURSION_DEPTH; break;
 case 31 : operation = RANDOMNESS; break;
 case 30 : glutPostRedisplay(); break;
 case 12 : std::exit( 0 );
 }
}

void rotateSubMenu( int item )
// Callback for processing rotation submenu.
{
  operation = ROTATE;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void translateSubMenu( int item )
// Callback for processing translation submenu.
{
  operation = TRANSLATE;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}


void projectionSubMenu( int item )
// Callback for processing projection submenu.
{
  switch ( item )
  {
    case 1 : projection = PERSPECTIVE; break;
    case 2 : projection = ORTHOGRAPHIC; break;
  }
  glutPostRedisplay();
}


void diffuseSubMenu( int item )
// Callback for processing diffuse lighting submenu.
{
  switch ( item )
  {
   case 1 : {color = RED; break; }
   case 2 : {color = GREEN; break; }
   case 3 : {color = BLUE; break; }
  }
  operation = DIFFUSE;
  glutPostRedisplay();
}


void setMenus( )
// Routine for creating menus.
{
  int rotateSubMenuCode, translateSubMenuCode;
  int diffuseSubMenuCode;
  int projectionSubMenuCode;

  rotateSubMenuCode = glutCreateMenu( rotateSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  translateSubMenuCode = glutCreateMenu( translateSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  projectionSubMenuCode = glutCreateMenu( projectionSubMenu );
  glutAddMenuEntry( "Perspective",      1 );
  glutAddMenuEntry( "Orthographic",      2 );

  diffuseSubMenuCode = glutCreateMenu( diffuseSubMenu );
  glutAddMenuEntry( "Red",      1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",      3 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Rotate ...",  rotateSubMenuCode );
  glutAddSubMenu( "Translate ...", translateSubMenuCode );
  glutAddMenuEntry( "Scale", 0 );
  glutAddMenuEntry( "Zoom", 1 );
  glutAddSubMenu( "Projection ...", projectionSubMenuCode );
  glutAddSubMenu( "Diffuse Properties ...",  diffuseSubMenuCode );
  glutAddMenuEntry( "Specular Property",  5 );
  glutAddMenuEntry( "Shininess Property",  6 );
  glutAddMenuEntry( "Height", 21);
  glutAddMenuEntry( "Radius", 22);
  glutAddMenuEntry( "Branch Angle", 23);
  glutAddMenuEntry( "Axial Symmetry Order", 24);
  glutAddMenuEntry( "Radius Reduction Factor", 25);
  glutAddMenuEntry( "Length Reduction Factor", 27);
  glutAddMenuEntry( "Rotation Factor", 28);
  glutAddMenuEntry( "Recursion Depth", 26);
  glutAddMenuEntry( "Randomness", 31);
  glutAddMenuEntry( "Reset",  9 );
  glutAddMenuEntry( "Redraw",  30 );
  glutAddMenuEntry( "Exit",  12 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

void mouse(int button, int state, int, int y)
// Routine for processing mouse events.
{
   y = windowHeight - y;
   if ( state != GLUT_DOWN ) return;
   switch (button)
     {
       case GLUT_LEFT_BUTTON : direction = DOWN; break;
       case GLUT_RIGHT_BUTTON : direction = UP; break;
     }
   operate();
}



int main(int argc, char** argv)
{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);
   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize (800, 800);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Recursively Generated Tree");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc( mouse );
   glEnable( GL_DEPTH_TEST );

   setMenus();
   setLighting();
   homePosition();

   srand(clock());
   cylinder = gluNewQuadric();
   gluQuadricDrawStyle(cylinder,(GLenum)GLU_FILL);

   glutMainLoop();
   return 0;
}
