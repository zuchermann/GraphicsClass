/*--------------------------------------------------------*/
/*  CS-395           Computer Graphics       Tom Ellman  */
/*--------------------------------------------------------*/
/*  nurbs.cpp : Nurbs Surface Editor                      */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

//Constant defining the file for holding the surface.
const char* fileName = "surface.nrb";

//Constants defining the control point array:
#define INITIAL_ROWS 5
#define INITIAL_COLUMNS 5
#define INITIAL_POINTS 5
#define INITIAL_FOV  60.0
#define MAX_COLUMNS 20
#define MAX_ROWS 20
#define MAX_POINTS 20

//Constants defining the viewing arrangement:
#define NEAR_CLIP 0.1
#define FAR_CLIP 200
#define LEFT_RIGHT_CLIP 2.0
#define BOTTOM_TOP_CLIP 2.0
#define BACK_CLIP 10.0
#define FRONT_CLIP -10.0
#define DEPTH_SHIFT -2.0

// Constants for defining changes to the viewing setup.
#define FOV_STEP 5.0
#define ROTATION_STEP 3.0
#define TRANSLATION_STEP 0.1
#define SCALE_FACTOR 1.1

// Constants for defining changes to the lighting setup.
#define DIFFUSE_STEP 0.1
#define SPECULAR_STEP 0.1
#define SHININESS_STEP 5.0

// Constants for defining changes to the weighting of control points.
#define WEIGHT_FACTOR 1.1

// Needed for degrees/radians conversions
#define PI 3.14159265358979

//Constant for initialization of max and min loops.
#define INFINITY 10000

// Order (degree + 1) of the NURBS.
#define INITIAL_ORDER 4
#define MAX_ORDER 4

//Constant defining the point size of the rendered control points.
#define CONTROL_POINT_SIZE 8

// Tolerance in pixels for selecting control points.
#define PICKING_DIMENSION 10

// Size of the selection buffer.
#define SELECTION_BUFFER_SIZE  10

// Variables to record the orders of the surface.
int rowOrder = INITIAL_ORDER;
int columnOrder = INITIAL_ORDER;
int order = INITIAL_ORDER;

// Indexes of the selected control point.
bool somethingSelected = false;
int rowSelection, columnSelection, selection;
list<int> selectionList;

// Flag to indicate direction of knot insertion.
typedef enum { ROW, COLUMN } orientationType;
orientationType orientation;

// Flag to indicate type of projection.
typedef enum { PERSPECTIVE, ORTHOGRAPHIC } projectionType;
projectionType projection = PERSPECTIVE;

// Global variable for storing the model view matrix.
GLdouble modelViewMatrix[ 16 ];
GLdouble inverseModelViewMatrix[ 16 ];

//Global variables for keeping track of the size and shape
//of the window.
int windowHeight, windowWidth;

//Enumerated type and global variable for keeping track
//of the selected operation.
typedef enum { SCALE, TRANSLATE, ROTATE, ZOOM, SELECT, MOVE, WEIGHT, INSERT, SWING, SWEEP,
               DIFFUSE, SPECULAR, SHININESS, HOME} operationType;
operationType operation = ZOOM;

//Enumerated type and global variable for talking about axies.
typedef enum { X, Y, Z } axisType;
axisType axis = Z;

//Enumerated type and global variable for talking about
//direction of changes.
typedef enum { DOWN, UP } directionType;
directionType direction = UP;

//Enumerated type and global variable for deciding how to
//display surface.
typedef enum { MESH, FILL, HIDE } displayModeType;
displayModeType displayMode = FILL;

//Enumerated type and global variable for indicating whether
//working on curve or surface.
typedef enum { CURVE, SURFACE } objectType;
objectType object = SURFACE;

//Global variables for keeping track of the field of view.
GLdouble fov = INITIAL_FOV, fovRadians = PI*INITIAL_FOV/180.0;
GLdouble nearPlane = NEAR_CLIP, farPlane = FAR_CLIP;
GLdouble aspectRatio;
GLdouble leftClip = -LEFT_RIGHT_CLIP, rightClip = LEFT_RIGHT_CLIP;
GLdouble bottom = -BOTTOM_TOP_CLIP, top = BOTTOM_TOP_CLIP;
GLdouble front = FRONT_CLIP, back = BACK_CLIP;

// Buffer for holding selection hit records.
GLuint selectionBuffer[SELECTION_BUFFER_SIZE];
GLint hits;


// Array for holding veiwport information.
GLint viewport[4];

// Array to hold the NURB surface control points.
int numRows = INITIAL_ROWS;
int numColumns = INITIAL_COLUMNS;
int numRowsCopy = INITIAL_ROWS;
int numColumnsCopy = INITIAL_COLUMNS;
GLfloat controlPoint[MAX_ROWS*MAX_COLUMNS*4];
GLfloat controlPointCopy[MAX_ROWS*MAX_COLUMNS*4];
GLfloat sKnots[MAX_ROWS+MAX_ORDER];
GLfloat sKnotsCopy[MAX_ROWS+MAX_ORDER];
GLfloat tKnots[MAX_COLUMNS+MAX_ORDER];
GLfloat tKnotsCopy[MAX_COLUMNS+MAX_ORDER];
bool showControlPoints = true;
bool showControlNet = true;

// Array to hold the NURB curve control points.
int numPoints = INITIAL_POINTS;
int numPointsCopy = INITIAL_POINTS;
GLfloat ctlPnt[MAX_POINTS*4];
GLfloat ctlPntCopy[MAX_POINTS*4];
GLfloat knots[MAX_POINTS+MAX_ORDER];
GLfloat knotsCopy[MAX_POINTS+MAX_ORDER];

// Values derived from surface control points.
GLfloat avgX, avgY, avgZ, maxX, maxY, maxZ, minX, minY, minZ;

// Values derived from curve control points.
GLfloat aX, aY, aZ, mxX, mxY, mxZ, mnX, mnY, mnZ;

// Pointer to the NURB surface object.
GLUnurbsObj *theNurbSurface;

// Pointer to the NURB curve object.
GLUnurbsObj *theNurbCurve;

// Variables to hold lighting description:
GLfloat shininess = 100.0;
GLfloat specular = 0.3;
GLfloat diffuseRed=0.0, diffuseGreen=0.7, diffuseBlue=0.0;
typedef enum { RED, GREEN, BLUE} colorType;
colorType color;


// Function to get surface control point.
inline GLfloat getSurfaceControlPoint(int row, int column, int dimension)
{
  return controlPoint[row*numColumns*4+column*4+dimension];
}

// Function to get surface control point from copy array.
inline GLfloat getSurfaceControlPointCopy(int row, int column, int dimension)
{
  return controlPointCopy[row*numColumnsCopy*4+column*4+dimension];
}

// Function to get surface homogenized control point.
inline GLfloat getHomogenizedSurfaceControlPoint(int row, int column, int dimension)
{
  return getSurfaceControlPoint(row,column,dimension)/getSurfaceControlPoint(row,column,3);
}

// Function to get surface homogenized control point from copy array.
inline GLfloat getHomogenizedSurfaceControlPointCopy(int row, int column, int dimension)
{
  return getSurfaceControlPointCopy(row,column,dimension)/getSurfaceControlPointCopy(row,column,3);
}

// Function to set surface control point.
inline void setSurfaceControlPoint(int row, int column, int dimension, GLfloat value)
{
  controlPoint[row*numColumns*4+column*4+dimension] = value;
  return;
}

// Function to set surface control point in copy array.
inline void setSurfaceControlPointCopy(int row, int column, int dimension, GLfloat value)
{
  controlPointCopy[row*numColumnsCopy*4+column*4+dimension] = value;
  return;
}

// Function to get curve control point.
inline GLfloat getCurveControlPoint(int column, int dimension)
{
  return ctlPnt[column*4+dimension];
}

// Function to get curve control point from copy array.
inline GLfloat getCurveControlPointCopy(int column, int dimension)
{
  return ctlPntCopy[column*4+dimension];
}

// Function to get curve homogenized control point.
inline GLfloat getHomogenizedCurveControlPoint(int column, int dimension)
{
  return getCurveControlPoint(column,dimension)/getCurveControlPoint(column,3);
}

// Function to get curve homogenized control point from copy array.
inline GLfloat getHomogenizedCurveControlPointCopy(int column, int dimension)
{
  return getCurveControlPointCopy(column,dimension)/getCurveControlPointCopy(column,3);
}

// Function to set curve control point.
inline void setCurveControlPoint(int column, int dimension, GLfloat value)
{
  ctlPnt[column*4+dimension] = value;
  return;
}

// Function to set curve control point in copy array.
inline void setCurveControlPointCopy(int column, int dimension, GLfloat value)
{
  ctlPnt[column*4+dimension] = value;
  return;
}



void initSurfaceDerivedValues()
// Compute the centroid of the nurb surface control points to be used as the center
// of rotation and scaling operations.
{
  avgX = avgY = avgZ = 0.0;
  maxX = maxY = maxZ = -INFINITY;
  minX = minY = minZ = INFINITY;
  int numControlPoints = numRows*numColumns;
  for (int u = 0; u < numRows; u++)
    {
      for (int v = 0; v < numColumns; v++)
        {
          GLfloat x = getHomogenizedSurfaceControlPoint(u,v,0);
          GLfloat y = getHomogenizedSurfaceControlPoint(u,v,1);
          GLfloat z = getHomogenizedSurfaceControlPoint(u,v,2);
          avgX += x;
          avgY += y;
          avgZ += z;
          minX = x<minX ? x : minX;
          minY = y<minY ? y : minY;
          minZ = z<minZ ? z : minZ;
          maxX = x>maxX ? x : maxX;
          maxY = y>maxY ? y : maxY;
          maxZ = z>maxZ ? z : maxZ;
        }
    }
  avgX /= numControlPoints;
  avgY /= numControlPoints;
  avgZ /= numControlPoints;
}

void initCurveDerivedValues()
// Compute the centroid of the nurb curve control points to be used as the center
// of rotation and scaling operations.
{
  aX = aY = aZ = 0.0;
  mxX = mxY = mxZ = -INFINITY;
  mnX = mnY = mnZ = INFINITY;
  for (int u = 0; u < numPoints; u++)
    {
          GLfloat x = getHomogenizedCurveControlPoint(u,0);
          GLfloat y = getHomogenizedCurveControlPoint(u,1);
          GLfloat z = getHomogenizedCurveControlPoint(u,2);
          aX += x;
          aY += y;
          aZ += z;
          mnX = x<mnX ? x : mnX;
          mnY = y<mnY ? y : mnY;
          mnZ = z<mnZ ? z : mnZ;
          mxX = x>mxX ? x : mxX;
          mxY = y>mxY ? y : mxY;
          mxZ = z>mxZ ? z : mxZ;
    }
  aX /= numPoints;
  aY /= numPoints;
  aZ /= numPoints;
}

// Initialize surface knot vectors.
void initSurfaceKnots()
{
  GLfloat step, knotValue;

  for (int rowIndex=0; rowIndex<rowOrder; rowIndex++)
    {
      sKnots[rowIndex] = 0.0; sKnots[numRows+rowOrder-rowIndex-1] = 1.0;
    }
  step = 1.0/(1+numRows-rowOrder);
  knotValue=step;
  for (int i=rowOrder; i<numRows; i++, knotValue+=step) sKnots[i] = knotValue;

  for (int columnIndex=0; columnIndex<columnOrder; columnIndex++)
    {
      tKnots[columnIndex] = 0.0; tKnots[numColumns+columnOrder-columnIndex-1] = 1.0;
    }
  step = 1.0/(1+numColumns-columnOrder);
  knotValue=step;
  for (int j=columnOrder; j<numColumns; j++, knotValue+=step) tKnots[j] = knotValue;
}

// Initialize curve knot vectors.
void initCurveKnots()
{
  GLfloat step, knotValue;

  for (int index=0; index<order; index++)
    {
      knots[index] = 0.0; knots[numPoints+order-index-1] = 1.0;
    }
  step = 1.0/(1+numPoints-order);
  knotValue=step;
  for (int i=order; i<numPoints; i++, knotValue+=step) knots[i] = knotValue;
}

// Initialize the surface control point array.
void initSurface()
{
   int u, v;
   GLfloat halfRows = (numRows-1)/2.0;
   GLfloat halfColumns = (numColumns-1)/2.0;
   for (u = 0; u < numRows; u++)
     {
      for (v = 0; v < numColumns; v++)
        {
          GLfloat y = (u - halfRows)/(1.0*halfRows);
          GLfloat x = (v - halfColumns)/(1.0*halfColumns);
          GLfloat z = (-(x*x)-(y*y))/4.0 + DEPTH_SHIFT;
          GLfloat w = 1.0;
          setSurfaceControlPoint(u,v,0,x);
          setSurfaceControlPoint(u,v,1,y);
          setSurfaceControlPoint(u,v,2,z);
          setSurfaceControlPoint(u,v,3,w);
        }
     }
   initSurfaceDerivedValues();
   initSurfaceKnots();
}

// Initialize the curve control point array.
void initCurve()
{
   int u;
   GLfloat halfPoints = (numPoints-1)/2.0;
   for (u = 0; u < numPoints; u++)
     {
          GLfloat x = (u - halfPoints)/(1.0*halfPoints);
          GLfloat y = 0.0;
          GLfloat z = DEPTH_SHIFT / 2.0;
          GLfloat w = 1.0;
          setCurveControlPoint(u,0,x);
          setCurveControlPoint(u,1,y);
          setCurveControlPoint(u,2,z);
          setCurveControlPoint(u,3,w);
     }
   initCurveDerivedValues();
   initCurveKnots();
}

// Callback function for reporting NURBS errors.
void nurbsError(GLenum errorCode)
{
  const GLubyte *estring;
  estring = gluErrorString(errorCode);
  cout << "Nurbs Error: " << estring << endl;
  std::exit (0);
}


// Initialize the model view matrix.
void initModelViewMatrix()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glGetDoublev(  GL_MODELVIEW_MATRIX, modelViewMatrix );
  glGetDoublev(  GL_MODELVIEW_MATRIX, inverseModelViewMatrix );
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

   glClearColor (0.4, 0.4, 0.4, 0.0);
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


//Initialize the display window.
void init()
{

   setLighting();

   initSurface();

   theNurbSurface = gluNewNurbsRenderer();
   gluNurbsProperty(theNurbSurface, (GLenum) GLU_SAMPLING_TOLERANCE, 25.0);

   initCurve();

   theNurbCurve = gluNewNurbsRenderer();
   gluNurbsProperty(theNurbCurve, (GLenum) GLU_SAMPLING_TOLERANCE, 25.0);

   gluNurbsCallback(theNurbSurface, (GLenum) GLU_ERROR, (void (__stdcall *)()) nurbsError);

   initModelViewMatrix();
}

void displaySurfaceControlPoints()
// Display each surface control point as a dot using a large point size.
{
  glColor3f(0.0, 0.0, 1.0);
  //  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  for (list<int>::iterator iter = selectionList.begin(); iter!=selectionList.end(); iter++)
    {
      int row = (*iter)/numColumns;
      int column = (*iter)%numColumns;
      glVertex4f(getSurfaceControlPoint(row,column,0),
                 getSurfaceControlPoint(row,column,1),
                 getSurfaceControlPoint(row,column,2),
                 getSurfaceControlPoint(row,column,3));
    }
  glEnd();
  //  glDisable(GL_POINT_SMOOTH);

  glPointSize(CONTROL_POINT_SIZE);
  glColor3f(1.0, 0.0, 0.0);
  //  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  for (int i = 0; i < numRows; i++)
    for (int j = 0; j < numColumns; j++)
      glVertex4f(getSurfaceControlPoint(i,j,0),
                 getSurfaceControlPoint(i,j,1),
                 getSurfaceControlPoint(i,j,2),
                 getSurfaceControlPoint(i,j,3));
  glEnd();
  //  glDisable(GL_POINT_SMOOTH);
}

void displaySurfaceControlNet()
// Display one set of lines connecting the surface control points in rows.
// Display another set of lines connecting the surface control points in columns.
// Use one color for rows and another for columms.
{
  cout << "Displaying control net with " << numRows << " rows and " << numColumns << " columns." << endl;
  glColor3f(1.0, 1.0, 0.0);
  for (int i = 0; i < numRows; i++)
    {
      glBegin(GL_LINE_STRIP);
      for (int j = 0; j < numColumns; j++)
          glVertex4f(getSurfaceControlPoint(i,j,0),getSurfaceControlPoint(i,j,1),
                     getSurfaceControlPoint(i,j,2),getSurfaceControlPoint(i,j,3));
      glEnd();
    }
  glColor3f(0.0, 0.0, 1.0);
  for (int j = 0; j < numColumns; j++)
    {
      glBegin(GL_LINE_STRIP);
      for (int i = 0; i < numRows; i++)
          glVertex4f(getSurfaceControlPoint(i,j,0),getSurfaceControlPoint(i,j,1),
                     getSurfaceControlPoint(i,j,2),getSurfaceControlPoint(i,j,3));
      glEnd();
    }
}

void displayCurveControlPoints()
// Display each curve control point as a dot using a large point size.
{
  glColor3f(0.0, 0.0, 1.0);
  //  glEnable(GL_POINT_SMOOTH);
  glBegin(GL_POINTS);
  for (list<int>::iterator iter = selectionList.begin(); iter!=selectionList.end(); iter++)
    {
      int index = *iter;
      glVertex4f(getCurveControlPoint(index,0),
                 getCurveControlPoint(index,1),
                 getCurveControlPoint(index,2),
                 getCurveControlPoint(index,3));
    }
  //  glDisable(GL_POINT_SMOOTH);

  glPointSize(CONTROL_POINT_SIZE);
  //  glEnable(GL_POINT_SMOOTH);
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_POINTS);
  for (int i = 0; i < numPoints; i++)
      glVertex4f(getCurveControlPoint(i,0),
                 getCurveControlPoint(i,1),
                 getCurveControlPoint(i,2),
                 getCurveControlPoint(i,3));
  glEnd();
  //  glDisable(GL_POINT_SMOOTH);
}

void displayCurveControlNet()
// Display set of lines connecting the curve control points.
{
  glColor3f(0.0, 0.0, 1.0);
  glBegin(GL_LINE_STRIP);
  for (int i = 0; i < numPoints; i++)
    glVertex4f(getCurveControlPoint(i,0),getCurveControlPoint(i,1),
               getCurveControlPoint(i,2),getCurveControlPoint(i,3));
  glEnd();
}



void displayNurbSurface()
// Display the nurb surface with filled polygons or as a wire mesh. If requested
// also display the control points and the control net.
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if (displayMode!=HIDE)
     {
       switch (displayMode)
         {
         case MESH : gluNurbsProperty(theNurbSurface, (GLenum) GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON); break;
         case FILL : gluNurbsProperty(theNurbSurface, (GLenum) GLU_DISPLAY_MODE, GLU_FILL); break;
         case HIDE : break;
         }
       gluBeginSurface(theNurbSurface);
       gluNurbsSurface(theNurbSurface,
                       numRows+rowOrder, sKnots, numColumns+columnOrder, tKnots,
                       numColumns * 4, 4, &controlPoint[0],
                       rowOrder, columnOrder, GL_MAP2_VERTEX_4);
       gluEndSurface(theNurbSurface);
     }

   glDisable(GL_LIGHTING);
   if (showControlPoints) displaySurfaceControlPoints();
   if (showControlNet) displaySurfaceControlNet();
   glEnable(GL_LIGHTING);

   glFlush();
}


void displayNurbCurve()
// Display the nurb curve. If requested also display the control points and the control net.
{
   glDisable(GL_LIGHTING);

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glColor3f(diffuseRed,diffuseGreen,diffuseBlue);

   gluBeginCurve(theNurbCurve);
   gluNurbsCurve(theNurbCurve, numPoints+order, knots, 4, &ctlPnt[0], order, GL_MAP1_VERTEX_4);
   gluEndCurve(theNurbCurve);

   if (showControlPoints) displayCurveControlPoints();
   if (showControlNet) displayCurveControlNet();

   glEnable(GL_LIGHTING);

   glFlush();
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
       case PERSPECTIVE  : gluPerspective(fov,aspectRatio,nearPlane,farPlane); break;
       case ORTHOGRAPHIC : glOrtho(leftClip,rightClip,bottom,top,front,back); break;
     }
   glMatrixMode(GL_MODELVIEW);


   // Draw the curve or surface.
   switch (object)
     {
       case CURVE   : displayNurbCurve(); break;
       case SURFACE : displayNurbSurface(); break;
     }

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
       case PERSPECTIVE  : gluPerspective(fov,aspectRatio,nearPlane,farPlane); break;
       case ORTHOGRAPHIC : glOrtho(leftClip,rightClip,bottom,top,front,back); break;
     }
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void homePosition()
//Putting the solar system and all viewing varibles back to their
//original states.
{
 axis = Z;
 direction = UP;
 operation = ZOOM;
 fov = INITIAL_FOV;
 fovRadians = PI*fov/180.0;
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
  glTranslated(avgX,avgY,avgZ);
  switch (axis)
    {
       case X : glRotated(angle,1.0,0.0,0.0); break;
       case Y : glRotated(angle,0.0,1.0,0.0); break;
       case Z : glRotated(angle,0.0,0.0,1.0); break;
    }
  glTranslated(-avgX,-avgY,-avgZ);
  glMultMatrixd(modelViewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);

  glLoadIdentity();
  glMultMatrixd(inverseModelViewMatrix);
  glTranslated(avgX,avgY,avgZ);
  switch (axis)
    {
       case X : glRotated(-angle,1.0,0.0,0.0); break;
       case Y : glRotated(-angle,0.0,1.0,0.0); break;
       case Z : glRotated(-angle,0.0,0.0,1.0); break;
    }
  glTranslated(-avgX,-avgY,-avgZ);
  glGetDoublev(GL_MODELVIEW_MATRIX,inverseModelViewMatrix);

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

  glLoadIdentity();
  glMultMatrixd(inverseModelViewMatrix);
  glTranslated(-deltaX,-deltaY,-deltaZ);
  glGetDoublev(GL_MODELVIEW_MATRIX,inverseModelViewMatrix);

}

void scale()
// Scale the scene from centroid of the nurb uniformly in all directions.
{

  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
  glTranslated(avgX,avgY,avgZ);
  switch (direction)
    {
      case UP   : glScaled(SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR); break;
      case DOWN : glScaled(1/SCALE_FACTOR,1/SCALE_FACTOR,1/SCALE_FACTOR); break;
    }
  glTranslated(-avgX,-avgY,-avgZ);
  glMultMatrixd(modelViewMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);

  glLoadIdentity();
  glMultMatrixd(inverseModelViewMatrix);
  glTranslated(avgX,avgY,avgZ);
  switch (direction)
    {
      case UP   : glScaled(1/SCALE_FACTOR,1/SCALE_FACTOR,1/SCALE_FACTOR); break;
      case DOWN : glScaled(SCALE_FACTOR,SCALE_FACTOR,SCALE_FACTOR); break;
    }
  glTranslated(-avgX,-avgY,-avgZ);
  glGetDoublev(GL_MODELVIEW_MATRIX,inverseModelViewMatrix);

}

void generateSurfaceControlPoints()
// Generate one vertex for each surface control point. For picking purposes.
{
  glInitNames();
  glPushName(0);
  for (int i = 0; i < numRows; i++)
    {
      for (int j = 0; j < numColumns; j++)
        {
          glLoadName(i*numColumns+j);
          glBegin(GL_POINTS);
          glVertex4f(getSurfaceControlPoint(i,j,0),getSurfaceControlPoint(i,j,1),
                     getSurfaceControlPoint(i,j,2),getSurfaceControlPoint(i,j,3));
          glEnd();
        }
    }
}

void generateCurveControlPoints()
// Generate one vertex for each curve control point. For picking purposes.
{
  glInitNames();
  glPushName(0);
  for (int i = 0; i < numPoints; i++)
    {
      glLoadName(i);
      glBegin(GL_POINTS);
      glVertex4f(getCurveControlPoint(i,0),getCurveControlPoint(i,1),
                 getCurveControlPoint(i,2),getCurveControlPoint(i,3));
      glEnd();
    }
}

void processHits()
{
  GLuint numNames, *ptr;
  ptr = (GLuint *) selectionBuffer;

  for (int i = 0; i < hits; i++)
    {
      numNames = *ptr;
      ptr += 3;
      for (GLuint j = 0; j < numNames; j++)
        {
          GLuint name = *ptr;
          switch (object)
            {
               case CURVE : selection = name; break;
                  case SURFACE : rowSelection = name / numColumns;
                              columnSelection = name % numColumns;
                              selection = name;
                              break;
            }
          selectionList.push_back(name);
          somethingSelected = true;
          cout << "\007" << flush;
          ptr++;
        }
    }
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







void selectControlPoint(int x, int y)
// Select a control point that is close to the mouse coordinates (x,y).
{

   //Set up the selection buffer.
   glSelectBuffer(SELECTION_BUFFER_SIZE, selectionBuffer);

   //Go into selection mode.
   glRenderMode(GL_SELECT);

   //Set the default camera position, aim and orientation.
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glMultMatrixd(modelViewMatrix);

   //Set up the projection matrix for picking.
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glGetIntegerv(GL_VIEWPORT, viewport);

   gluPickMatrix ((GLdouble) x, (GLdouble) y, PICKING_DIMENSION, PICKING_DIMENSION, viewport);
   switch (projection)
     {
       case PERSPECTIVE  : gluPerspective(fov,aspectRatio,nearPlane,farPlane); break;
       case ORTHOGRAPHIC : glOrtho(leftClip,rightClip,bottom,top,front,back); break;
     }

   //Draw the control points in selection mode.
   switch (object)
     {
       case CURVE   : generateCurveControlPoints(); break;
       case SURFACE : generateSurfaceControlPoints(); break;
     }

   //Process the hits and record the selection.
   hits = glRenderMode(GL_RENDER);

   processHits();
   glutPostRedisplay();
}

void translateSelectedSurfaceControlPoints(GLdouble deltaX, GLdouble deltaY, GLdouble deltaZ)
{
  for (list<int>::iterator iter = selectionList.begin(); iter!=selectionList.end(); iter++)
    {
      int row = (*iter)/numColumns;
      int column = (*iter)%numColumns;
      GLdouble x = getSurfaceControlPoint(row,column,0);
      GLdouble y = getSurfaceControlPoint(row,column,1);
      GLdouble z = getSurfaceControlPoint(row,column,2);
      GLdouble w = getSurfaceControlPoint(row,column,3);
      setSurfaceControlPoint(row,column,0,x+w*deltaX);
      setSurfaceControlPoint(row,column,1,y+w*deltaY);
      setSurfaceControlPoint(row,column,2,z+w*deltaZ);
    }
}


void moveSurfaceControlPoint(int x, int y)
// Move the currently selected control point perpendicular to the line if sight
// to a location that will render at mouse coordinates (x,y).
{
  if (somethingSelected == false) return;
  GLdouble last[4], lastView[4], nextView[4], next[4];
  GLdouble lastViewZ, xRel, yRel, xProjected, yProjected, yTangent, xTangent;
  GLdouble lastWeight, nextWeight, weightRatio;

  last[0] = getSurfaceControlPoint(rowSelection,columnSelection,0);
  last[1] = getSurfaceControlPoint(rowSelection,columnSelection,1);
  last[2] = getSurfaceControlPoint(rowSelection,columnSelection,2);
  last[3] = getSurfaceControlPoint(rowSelection,columnSelection,3);

  lastWeight = last[3];

  multiplyMatrixVector(modelViewMatrix,last,lastView,4);
  lastViewZ = lastView[2]/lastView[3];

  switch (projection)
    {
      case PERSPECTIVE   : yTangent = tan(fovRadians/2.0);
                           xTangent = yTangent * aspectRatio;
                           xRel = GLdouble(x) - GLdouble(windowWidth)/2.0;
                           yRel = GLdouble(y) - GLdouble(windowHeight)/2.0;
                           xProjected = xTangent * 2.0 * xRel / GLdouble(windowWidth);
                           yProjected = yTangent * 2.0 * yRel / GLdouble(windowHeight);
                           nextView[0] = - lastViewZ * xProjected;
                           nextView[1] = - lastViewZ * yProjected;
                           nextView[2] = lastViewZ;
                           nextView[3] = 1.0;
                           break;
      case ORTHOGRAPHIC  : xRel = GLdouble(x) - GLdouble(windowWidth)/2.0;
                           yRel = GLdouble(y) - GLdouble(windowHeight)/2.0;
                           xProjected = 2.0 * xRel / GLdouble(windowWidth);
                           yProjected = 2.0 * yRel / GLdouble(windowHeight);
                           nextView[0] = xProjected * LEFT_RIGHT_CLIP;
                           nextView[1] = yProjected * BOTTOM_TOP_CLIP;
                           nextView[2] = lastViewZ;
                           nextView[3] = 1.0;
                           break;
    }

  multiplyMatrixVector(inverseModelViewMatrix,nextView,next,4);

  nextWeight = next[3];
  weightRatio = lastWeight / nextWeight;
  next[0] *= weightRatio;
  next[1] *= weightRatio;
  next[2] *= weightRatio;
  next[3] = lastWeight;

  translateSelectedSurfaceControlPoints( (next[0]-last[0])/lastWeight,
                                         (next[1]-last[1])/lastWeight,
                                         (next[2]-last[2])/lastWeight );

  glutPostRedisplay();
}


void moveCurveControlPoint(int x, int y)
// Move the currently selected control point perpendicular to the line if sight
// to a location that will render at mouse coordinates (x,y).
{
  if (somethingSelected == false) return;
  GLdouble last[4], lastView[4], nextView[4], next[4];
  GLdouble lastViewZ, xRel, yRel, xProjected, yProjected;
  GLdouble lastWeight, nextWeight, weightRatio;

  last[0] = getCurveControlPoint(selection,0);
  last[1] = getCurveControlPoint(selection,1);
  last[2] = getCurveControlPoint(selection,2);
  last[3] = getCurveControlPoint(selection,3);

  lastWeight = last[3];

  multiplyMatrixVector(modelViewMatrix,last,lastView,4);
  lastViewZ = lastView[2]/lastView[3];

  switch (projection)
    {
      case PERSPECTIVE   : break;
      case ORTHOGRAPHIC  : xRel = GLdouble(x) - GLdouble(windowWidth)/2.0;
                           yRel = GLdouble(y) - GLdouble(windowHeight)/2.0;
                           xProjected = 2.0 * xRel / GLdouble(windowWidth);
                           yProjected = 2.0 * yRel / GLdouble(windowHeight);
                           nextView[0] = xProjected * LEFT_RIGHT_CLIP;
                           nextView[1] = yProjected * BOTTOM_TOP_CLIP;
                           nextView[2] = lastViewZ;
                           nextView[3] = 1.0;
                           break;
    }

  multiplyMatrixVector(inverseModelViewMatrix,nextView,next,4);

  nextWeight = next[3];
  weightRatio = lastWeight / nextWeight;
  next[0] *= weightRatio;
  next[1] *= weightRatio;
  next[2] *= weightRatio;
  next[3] = lastWeight;

  setCurveControlPoint(selection,0,next[0]);
  setCurveControlPoint(selection,1,next[1]);
  setCurveControlPoint(selection,2,next[2]);
  setCurveControlPoint(selection,3,next[3]);

  glutPostRedisplay();
}


void moveControlPoint(int x, int y)
// Move the currently selected control point perpendicular to the line if sight
  // to a location that will render at mouse coordinates (x,y).
{
   switch (object)
     {
       case CURVE   : moveCurveControlPoint(x,y); break;
       case SURFACE : moveSurfaceControlPoint(x,y); break;
     }
}



void weightSurfaceControlPoint()
// Increase or decrease the weight of the currently selected surface control point.
{
  GLdouble current[4], currentWeight, newWeight, weightRatio;

  current[0] = getSurfaceControlPoint(rowSelection,columnSelection,0);
  current[1] = getSurfaceControlPoint(rowSelection,columnSelection,1);
  current[2] = getSurfaceControlPoint(rowSelection,columnSelection,2);
  current[3] = getSurfaceControlPoint(rowSelection,columnSelection,3);

  currentWeight = current[3];
  newWeight = currentWeight;
  switch (direction)
    {
      case UP   : newWeight *= WEIGHT_FACTOR; break;
      case DOWN : newWeight /= WEIGHT_FACTOR; break;
    }
  weightRatio = newWeight/currentWeight;

  setSurfaceControlPoint(rowSelection,columnSelection,0,current[0]*weightRatio);
  setSurfaceControlPoint(rowSelection,columnSelection,1,current[1]*weightRatio);
  setSurfaceControlPoint(rowSelection,columnSelection,2,current[2]*weightRatio);
  setSurfaceControlPoint(rowSelection,columnSelection,3,newWeight);

  glutPostRedisplay();
}

void weightCurveControlPoint()
// Increase or decrease the weight of the currently selected curve control point.
{
  GLdouble current[4], currentWeight, newWeight, weightRatio;

  current[0] = getCurveControlPoint(selection,0);
  current[1] = getCurveControlPoint(selection,1);
  current[2] = getCurveControlPoint(selection,2);
  current[3] = getCurveControlPoint(selection,3);

  currentWeight = current[3];
  newWeight = currentWeight;
  switch (direction)
    {
      case UP   : newWeight *= WEIGHT_FACTOR; break;
      case DOWN : newWeight /= WEIGHT_FACTOR; break;
    }
  weightRatio = newWeight/currentWeight;

  setCurveControlPoint(selection,0,current[0]*weightRatio);
  setCurveControlPoint(selection,1,current[1]*weightRatio);
  setCurveControlPoint(selection,2,current[2]*weightRatio);
  setCurveControlPoint(selection,3,newWeight);

  glutPostRedisplay();
}

void weightControlPoint()
{
   switch (object)
     {
       case CURVE   : weightCurveControlPoint(); break;
       case SURFACE : weightSurfaceControlPoint(); break;
     }
}

inline GLfloat insertionTerm(GLfloat alpha, GLfloat pHigh, GLfloat pLow)
{
  if (alpha == 0.0) return pLow;
  else if (alpha == 1.0) return pHigh;
  else return (alpha * pHigh + (1 - alpha) * pLow);
}


void insertSurfaceRowKnot()
// Insert a new sufrace knot in the Row direction at a location determined
// by the currently selected control point.
{
  int r,c, index, insertionIndex, oldNumRows, newNumRows;
  GLfloat newKnot;

  oldNumRows = numRows;
  newNumRows = numRows+1;
  numRows = newNumRows;

  insertionIndex = rowSelection + rowOrder/2;

  cout << "Insertion Index: " << insertionIndex << endl;
  if (!somethingSelected)
    {
      cout << "Nothing Selected." << endl;
      numRows = oldNumRows;
      return;
    }
  if (insertionIndex < rowOrder-1 || insertionIndex > oldNumRows-1)
    {
      cout << "Invalid insertion point: " << insertionIndex << endl;
      numRows = oldNumRows;
      return;
    }

  newKnot = ( sKnots[insertionIndex]+sKnots[insertionIndex+1] ) / 2.0;

  for (index = 0; index < oldNumRows*numColumns*4; index++) controlPointCopy[index] = controlPoint[index];

  for (c = 0; c < numColumns; c++)
      for (r = 0; r < newNumRows; r++)
        {
          GLfloat alpha, value;

          if (r <= insertionIndex-(rowOrder-1)) alpha = 1.0;
          else if (r >= insertionIndex+1) alpha = 0.0;
          else alpha = (newKnot - sKnots[r]) / (sKnots[r+rowOrder-1] - sKnots[r]);

          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,0),getSurfaceControlPointCopy(r-1,c,0));
          setSurfaceControlPoint(r,c,0,value);
          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,1),getSurfaceControlPointCopy(r-1,c,1));
          setSurfaceControlPoint(r,c,1,value);
          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,2),getSurfaceControlPointCopy(r-1,c,2));
          setSurfaceControlPoint(r,c,2,value);
          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,3),getSurfaceControlPointCopy(r-1,c,3));
          setSurfaceControlPoint(r,c,3,value);
        }

  numRowsCopy++;

  for (r = 0; r < oldNumRows+rowOrder; r++) sKnotsCopy[r] = sKnots[r];
  for (r = 0; r <= insertionIndex; r++) sKnots[r] = sKnotsCopy[r];
  sKnots[r] =  newKnot;
  for (r = r+1; r < newNumRows+rowOrder; r++) sKnots[r] = sKnotsCopy[r-1];

}



void insertSurfaceColumnKnot()
// Insert a new surface knot in the column direction at a location determined
// by the currently selected control point.
{
  int c,r, index, insertionIndex, oldNumColumns, newNumColumns;
  GLfloat newKnot;

  oldNumColumns = numColumns;
  newNumColumns = numColumns+1;
  numColumns = newNumColumns;

  insertionIndex = columnSelection + columnOrder/2;

  cout << "Insertion Index: " << insertionIndex << endl;
  if (!somethingSelected)
    {
      cout << "Nothing Selected. " << endl;
      numColumns = oldNumColumns;
      return;
    }
  if (insertionIndex < columnOrder-1 || insertionIndex > oldNumColumns-1)
    {
      cout << "Invalid insertion point: " << insertionIndex << endl;
      numColumns = oldNumColumns;
      return;
    }

  newKnot = ( tKnots[insertionIndex]+tKnots[insertionIndex+1] ) / 2.0;

  for (index = 0; index < oldNumColumns*numRows*4; index++) controlPointCopy[index] = controlPoint[index];

  for (r = 0; r < numRows; r++)
      for (c = 0; c < newNumColumns; c++)
        {
          GLfloat alpha, value;

          if (c <= insertionIndex-(columnOrder-1)) alpha = 1.0;
          else if (c >= insertionIndex+1) alpha = 0.0;
          else alpha = (newKnot - tKnots[c]) / (tKnots[c+columnOrder-1] - tKnots[c]);

          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,0),getSurfaceControlPointCopy(r,c-1,0));
          setSurfaceControlPoint(r,c,0,value);
          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,1),getSurfaceControlPointCopy(r,c-1,1));
          setSurfaceControlPoint(r,c,1,value);
          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,2),getSurfaceControlPointCopy(r,c-1,2));
          setSurfaceControlPoint(r,c,2,value);
          value = insertionTerm(alpha,getSurfaceControlPointCopy(r,c,3),getSurfaceControlPointCopy(r,c-1,3));
          setSurfaceControlPoint(r,c,3,value);
        }

  numColumnsCopy++;

  for (c = 0; c < oldNumColumns+columnOrder; c++) tKnotsCopy[c] = tKnots[c];
  for (c = 0; c <= insertionIndex; c++) tKnots[c] = tKnotsCopy[c];
  tKnots[c] =  newKnot;
  for (c = c+1; c < newNumColumns+columnOrder; c++) tKnots[c] = tKnotsCopy[c-1];

}

void insertCurveKnot()
// Insert a new curve knot in at a location determined
// by the currently selected control point.
{
  int c, index, insertionIndex, oldNumPoints, newNumPoints;
  GLfloat newKnot;

  oldNumPoints = numPoints;
  newNumPoints = numPoints+1;
  numPoints = newNumPoints;

  insertionIndex = selection + order/2;

  cout << "Insertion Index: " << insertionIndex << endl;
  if (!somethingSelected)
    {
      cout << "Nothing Selected. " << endl;
      numPoints = oldNumPoints;
      return;
    }
  if (insertionIndex < order-1 || insertionIndex > oldNumPoints-1)
    {
      cout << "Invalid insertion point: " << insertionIndex << endl;
      numPoints = oldNumPoints;
      return;
    }

  newKnot = ( knots[insertionIndex]+knots[insertionIndex+1] ) / 2.0;

  for (index = 0; index < oldNumPoints*4; index++) ctlPntCopy[index] = ctlPnt[index];

  for (c = 0; c < newNumPoints; c++)
        {
          GLfloat alpha, value;

          if (c <= insertionIndex-(order-1)) alpha = 1.0;
          else if (c >= insertionIndex+1) alpha = 0.0;
          else alpha = (newKnot - knots[c]) / (knots[c+order-1] - knots[c]);

          value = insertionTerm(alpha,getCurveControlPointCopy(c,0),getCurveControlPointCopy(c-1,0));
          setCurveControlPoint(c,0,value);
          value = insertionTerm(alpha,getCurveControlPointCopy(c,1),getCurveControlPointCopy(c-1,1));
          setCurveControlPoint(c,1,value);
          value = insertionTerm(alpha,getCurveControlPointCopy(c,2),getCurveControlPointCopy(c-1,2));
          setCurveControlPoint(c,2,value);
          value = insertionTerm(alpha,getCurveControlPointCopy(c,3),getCurveControlPointCopy(c-1,3));
          setCurveControlPoint(c,3,value);
        }

  numPointsCopy++;

  for (c = 0; c < oldNumPoints+order; c++) knotsCopy[c] = knots[c];
  for (c = 0; c <= insertionIndex; c++) knots[c] = knotsCopy[c];
  knots[c] =  newKnot;
  for (c = c+1; c < newNumPoints+order; c++) knots[c] = knotsCopy[c-1];

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
      case SELECT    : break;
      case MOVE      : break;
      case SWING      : break;
      case SWEEP      : break;
      case WEIGHT    : weightControlPoint(); break;
      case INSERT    : switch (object)
                        {
                            case CURVE   : insertCurveKnot(); break;
                          case SURFACE : switch (orientation)
                                         {
                                           case ROW     : insertSurfaceRowKnot(); break;
                                           case COLUMN  : insertSurfaceColumnKnot(); break;
                                         }
                                         break;
                        }
                       break;
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
    }
  glutPostRedisplay();
}

void writeSurface()
{
  ofstream ofs;
  ofs.open(fileName);

  int r, c;

  ofs << rowOrder << endl;
  ofs << numRows << endl;
  for (r = 0; r < numRows + rowOrder; r++) ofs << sKnots[r] << " ";
  ofs << endl;
  ofs << endl;

  ofs << columnOrder << endl;
  ofs << numColumns << endl;
  for (c = 0; c < numColumns + columnOrder; c++) ofs << tKnots[c] << " ";;
  ofs << endl;
  ofs << endl;

  for (c = 0; c < numColumns; c++)
       {
         for (r = 0; r < numRows; r++)
           {
             ofs << getSurfaceControlPoint(r,c,0) << " ";
             ofs << getSurfaceControlPoint(r,c,1) << " ";
             ofs << getSurfaceControlPoint(r,c,2) << " ";
             ofs << getSurfaceControlPoint(r,c,3) << endl;
           }
         ofs << endl;
       }
  ofs << endl;

  ofs.close();
}

void readSurface()
{
  ifstream ifs;
  ifs.open(fileName);

  int r, c;

  ifs >> rowOrder;
  ifs >> numRows;
  numRowsCopy = numRows;
  for (r = 0; r < numRows + rowOrder; r++) ifs >> sKnots[r];

  ifs >> columnOrder;
  ifs >> numColumns;
  numColumnsCopy = numColumns;
  for (c = 0; c < numColumns + columnOrder; c++) ifs >> tKnots[c];

  for (c = 0; c < numColumns; c++)
       {
         for (r = 0; r < numRows; r++)
           {
             GLfloat value;
             ifs >> value; setSurfaceControlPoint(r,c,0,value);
             ifs >> value; setSurfaceControlPoint(r,c,1,value);
             ifs >> value; setSurfaceControlPoint(r,c,2,value);
             ifs >> value; setSurfaceControlPoint(r,c,3,value);
           }
       }

  ifs.close();

  initSurfaceDerivedValues();
  somethingSelected = false;
  selectionList.clear();
}


void setupSwing()
{
  projection = ORTHOGRAPHIC;
  operation = SWING;
  object = CURVE;
}

void setupSweep()
{
  homePosition();
  glRotated(-90.0,1.0,0.0,0.0);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelViewMatrix);
  projection = ORTHOGRAPHIC;
  operation = SWEEP;
  object = CURVE;
}



void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 0 : operation = SCALE; break;
 case 1 : operation = ZOOM; break;
 case 3 : operation = MOVE; break;
 case 4 : operation = WEIGHT; break;
 case 5 : operation = SPECULAR; break;
 case 6 : operation = SHININESS; break;
 case 7 : setupSwing(); break;
 case 8 : setupSweep(); break;
 case 9 : homePosition();glutPostRedisplay(); break;
 case 10 : writeSurface(); break;
 case 11 : readSurface();glutPostRedisplay(); break;
 case 12 : std::exit( 0 );
 }
}

void objectSubMenu( int item )
// Callback for processing object submenu.
{
  switch ( item )
  {
    case 1 : object = CURVE; projection = ORTHOGRAPHIC; break;
    case 2 : object = SURFACE; break;
  }
  glutPostRedisplay();
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


void displayModeSubMenu( int item )
// Callback for processing display mode submenu.
{
  switch ( item )
  {
   case 1 : {displayMode = FILL; break; }
   case 2 : {displayMode = MESH; break; }
   case 3 : {displayMode = HIDE; break; }
  }
  glutPostRedisplay();
}

void controlPointsSubMenu( int item )
// Callback for processing control points submenu.
{
  switch ( item )
  {
   case 1 : {showControlPoints = true; break; }
   case 2 : {showControlPoints = false; break; }
  }
  glutPostRedisplay();
}

void controlNetSubMenu( int item )
// Callback for processing control net submenu.
{
  switch ( item )
  {
   case 1 : {showControlNet = true; break; }
   case 2 : {showControlNet = false; break; }
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


void insertSubMenu( int item )
// Callback for processing knot insertion submenu.
{
  switch ( item )
  {
   case 1 : {orientation = ROW; break; }
   case 2 : {orientation = COLUMN; break; }
  }
  operation = INSERT;
  operate();
  glutPostRedisplay();
}


void selectSubMenu( int item )
// Callback for processing knot insertion submenu.
{
  switch ( item )
  {
   case 1 : operation = SELECT; break;
   case 2 : selectionList.clear(); somethingSelected = false; break;
  }
   glutPostRedisplay();
}


void setMenus( )
// Routine for creating menus.
{
  int objectSubMenuCode, rotateSubMenuCode, translateSubMenuCode;
  int displayModeSubMenuCode, controlPointsSubMenuCode, controlNetSubMenuCode;
  int diffuseSubMenuCode, insertSubMenuCode;
  int projectionSubMenuCode;
  int selectSubMenuCode;

  objectSubMenuCode = glutCreateMenu( objectSubMenu );
  glutAddMenuEntry( "Curve",      1 );
  glutAddMenuEntry( "Surface",      2 );

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

  displayModeSubMenuCode = glutCreateMenu( displayModeSubMenu );
  glutAddMenuEntry( "Show Fill",      1 );
  glutAddMenuEntry( "Show Mesh",      2 );
  glutAddMenuEntry( "Hide",      3 );

  controlPointsSubMenuCode = glutCreateMenu( controlPointsSubMenu );
  glutAddMenuEntry( "Show",      1 );
  glutAddMenuEntry( "Hide",      2 );

  controlNetSubMenuCode = glutCreateMenu( controlNetSubMenu );
  glutAddMenuEntry( "Show",      1 );
  glutAddMenuEntry( "Hide",      2 );

  diffuseSubMenuCode = glutCreateMenu( diffuseSubMenu );
  glutAddMenuEntry( "Red",      1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",      3 );

  insertSubMenuCode = glutCreateMenu( insertSubMenu );
  glutAddMenuEntry( "Row",      1 );
  glutAddMenuEntry( "Column",      2 );

  selectSubMenuCode = glutCreateMenu( selectSubMenu );
  glutAddMenuEntry( "Select",      1 );
  glutAddMenuEntry( "DeSelect",      2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Object Type ...", objectSubMenuCode );
  glutAddSubMenu( "Rotate ...",  rotateSubMenuCode );
  glutAddSubMenu( "Translate ...", translateSubMenuCode );
  glutAddMenuEntry( "Scale", 0 );
  glutAddSubMenu( "Projection ...", projectionSubMenuCode );
  glutAddMenuEntry( "Zoom",  1 );
  glutAddSubMenu( "Display Surface ...", displayModeSubMenuCode );
  glutAddSubMenu( "Display Control Points ...", controlPointsSubMenuCode );
  glutAddSubMenu( "Display Control Net ...", controlNetSubMenuCode );
  glutAddSubMenu( "Select Control Point",  selectSubMenuCode );
  glutAddMenuEntry( "Move Control Point",  3 );
  glutAddMenuEntry( "Weight Control Point",  4 );
  glutAddSubMenu( "Insert Knot ...", insertSubMenuCode );
  glutAddSubMenu( "Surface Diffuse Properties ...",  diffuseSubMenuCode );
  glutAddMenuEntry( "Surface Specular Property",  5 );
  glutAddMenuEntry( "Surface Shininess Property",  6 );
  glutAddMenuEntry( "Swing Curve",  7 );
  glutAddMenuEntry( "Sweep Curve",  8 );
  glutAddMenuEntry( "Home Position",  9 );
  glutAddMenuEntry( "Write to File",  10 );
  glutAddMenuEntry( "Read from File",  11 );
  glutAddMenuEntry( "Exit",  12 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

// Create a swing surface.
// Special case only: Surface of revolution.
void swing(int x, int)
{
  GLdouble xCenter;
  GLdouble vector[4], newVector[4];

  xCenter =  rightClip * ( x - GLdouble(windowWidth)/2.0 ) / (GLdouble(windowWidth) / 2.0);
  vector[0] = xCenter;
  vector[1] = 0.0;
  vector[2] = 0.0;
  vector[3] = 1.0;
  multiplyMatrixVector(inverseModelViewMatrix,vector,newVector,4);
  xCenter = newVector[0]/newVector[3];;

  numRows = numPoints;
  numRowsCopy = numPoints;
  numColumns = 9;
  numColumnsCopy = 9;

  rowOrder = order;
  columnOrder = 3;

  GLdouble value = sqrt(2.0)/2.0;
  GLdouble maxRadius, m1, m2, zShift;
  m1 = fabs(mxX-xCenter);
  m2 = fabs(mnX-xCenter);
  if (m1>=m2) maxRadius = m1; else maxRadius = m2;
  zShift = -2.0*maxRadius+DEPTH_SHIFT;

  for (int r = 0; r < numPoints; r++)
    for (int c = 0; c < 9; c++)
      {
        GLdouble radius = getCurveControlPoint(r,0)-xCenter;

        if (c==0 || c==1 || c==7 || c==8) setSurfaceControlPoint(r,c,0,radius);
        else if (c==2 || c==6) setSurfaceControlPoint(r,c,0,0.0);
        else setSurfaceControlPoint(r,c,0,-radius);

        setSurfaceControlPoint(r,c,1,getCurveControlPoint(r,1));

        if (c==1 || c==2 || c==3) setSurfaceControlPoint(r,c,2,radius+zShift);
        else if (c==0 || c==4 || c==8) setSurfaceControlPoint(r,c,2,0.0+zShift);
        else setSurfaceControlPoint(r,c,2,-radius+zShift);

        if (c%2==0) setSurfaceControlPoint(r,c,3,getCurveControlPoint(r,3));
        else {
                setSurfaceControlPoint(r,c,0,value*getSurfaceControlPoint(r,c,0));
                setSurfaceControlPoint(r,c,1,value*getSurfaceControlPoint(r,c,1));
                setSurfaceControlPoint(r,c,2,value*getSurfaceControlPoint(r,c,2));
                setSurfaceControlPoint(r,c,3,value*getCurveControlPoint(r,3));
             }

      }

  for (int i=0; i<numPoints+order; i++) { sKnots[i]=knots[i]; }
  tKnots[0] = 0; tKnots[1] = 0; tKnots[2] = 0;
  tKnots[3] = 0.25; tKnots[4] = 0.25;
  tKnots[5] = 0.5; tKnots[6] = 0.5;
  tKnots[7] = 0.75; tKnots[8] = 0.75;
  tKnots[9] = 1.0; tKnots[10] = 1.0; tKnots[11] = 1.0;

  initSurfaceDerivedValues();

  object = SURFACE;

  homePosition();

  glutPostRedisplay();
}

// Create a sweep surface.
// Special case only: Generalized cylinder.
void sweep(int, int y)
{
  GLdouble height, quarterHeight;
  GLdouble vector[4], newVector[4];

  height =  top * ( y - GLdouble(windowHeight)/2.0 ) / (GLdouble(windowHeight) / 2.0);
  vector[0] = 0.0;
  vector[1] = 0.0;
  vector[2] = height;
  vector[3] = 1.0;
  multiplyMatrixVector(inverseModelViewMatrix,vector,newVector,4);
  height = newVector[2]/newVector[3];

  quarterHeight = height/4.0;

  numRows = 4;
  numRowsCopy = 4;
  numColumns = numPoints;
  numColumnsCopy = numPoints;

  rowOrder = 4;
  columnOrder = order;

  for (int r = 0; r < 4; r++)
    for (int c = 0; c < numPoints; c++)
      {
        setSurfaceControlPoint(r,c,0,getCurveControlPoint(c,0));
        setSurfaceControlPoint(r,c,1,getCurveControlPoint(c,1));
        setSurfaceControlPoint(r,c,2,-r*quarterHeight+DEPTH_SHIFT);
        setSurfaceControlPoint(r,c,3,getCurveControlPoint(c,3));
      }

  for (int i=0; i<4; i++) { sKnots[i]=0.0; sKnots[8-i-1]=1.0; }
  for (int j=0; j < numPoints+order; j++) tKnots[j] = knots[j];

  initSurfaceDerivedValues();

  object = SURFACE;

  homePosition();

  glutPostRedisplay();
}


void mouse(int button, int state, int x, int y)
// Routine for processing mouse events.
{
   y = windowHeight - y;
   if ( state != GLUT_DOWN ) return;
   switch (button)
     {
       case GLUT_LEFT_BUTTON : direction = DOWN; break;
       case GLUT_RIGHT_BUTTON : direction = UP; break;
     }
   if (operation == SELECT) selectControlPoint(x,y);
   else if (operation == MOVE) moveControlPoint(x,y);
   else if (operation == SWING) swing(x,y);
   else if (operation == SWEEP) sweep(x,y);
   else operate();
}



int main(int argc, char** argv)
{
  // Mask floating point exceptions.
  _control87(MCW_EM,MCW_EM);

   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize (600, 600);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("NURB Surface Editor");
   init();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc( mouse );
   glEnable( GL_DEPTH_TEST );

   setMenus();
   homePosition();

   glutMainLoop();
   return 0;
}
