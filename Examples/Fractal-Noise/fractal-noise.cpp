/*--------------------------------------------------------*/
/*  CS-395           Computer Graphics       Tom Ellman   */
/*--------------------------------------------------------*/
/*  fractal-noise.cpp                                     */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <iostream>
#include "utilities.h"

//Useful contants.
#define PI 3.14159

//Constants defining the step increments for changing
//the location and the aim of the camera.
#define INITIAL_EYE_X 0.0
#define INITIAL_EYE_Y 0.0
#define INITIAL_EYE_Z 2.5
#define INITIAL_CEN_X 0.0
#define INITIAL_CEN_Y 0.0
#define INITIAL_CEN_Z 0.0
#define INITIAL_UP_X 0.0
#define INITIAL_UP_Y 1.0
#define INITIAL_UP_Z 0.0
#define INITIAL_FOV 60.0

using namespace std;


//Constants defining the step increments for changing
//the location and the aim of the camera.
#define EYE_STEP 0.1
#define CEN_STEP 0.1
#define FOV_STEP 5.0
#define ROTATION_STEP 3.0
#define TRANSLATION_STEP 0.1
#define SCALE_FACTOR 1.1

//Constants defining the viewing arrangement:
#define NEAR_CLIP 0.1
#define FAR_CLIP 200
#define LEFT_RIGHT_CLIP 2.0
#define BOTTOM_TOP_CLIP 2.0
#define FRONT_BACK_CLIP 100.0

// Constants for defining changes to the lighting setup.
#define INITIAL_SPECULAR 0.3
#define SPECULAR_STEP 0.1
#define INITIAL_SHININESS 100.0
#define SHININESS_STEP 5.0

//Constant defining the length of the texmap.
#define	TEXTURE_IMAGE_LENGTH 1024

//Constants defining the terrain array size.
#define MAX_RESOLUTION 512
#define MIN_RESOLUTION 8
#define RESOLUTION_FACTOR 2

//Constants defining the coordinate system.
#define INITIAL_X_ORIGIN -0.5
#define INITIAL_Y_ORIGIN -0.5
//#define INITIAL_VERTICAL_SCALE 0.25
#define INITIAL_VERTICAL_SCALE 0.0

// Constants defining the FBM noise.
//#define INITIAL_MINIMUM_FREQUENCY 1.0
#define INITIAL_MINIMUM_FREQUENCY 10.0
//#define INITIAL_FRACTAL_INCREMENT 1.5
#define INITIAL_FRACTAL_INCREMENT 1.0
#define FRACTAL_INCREMENT_STEP 0.1
//#define INITIAL_OCTAVES 8
#define INITIAL_OCTAVES 24
#define MAX_OCTAVES 32
#define OCTAVES_STEP 1
#define INITIAL_LACUNARITY 2.0
#define LACUNARITY_STEP 1.0

// Constants defining the texture.
#define INITIAL_TEXTURE_BIAS 0.5
#define TEXTURE_BIAS_FACTOR 0.5
#define TEXTURE_BIAS_FLAT_UP 0.75
#define TEXTURE_BIAS_FLAT_DOWN 0.25
#define TEXTURE_BIAS_STEP 0.1
#define INITIAL_TEXTURE_GAIN 0.15
#define TEXTURE_GAIN_FACTOR 0.5
#define TEXTURE_GAIN_FLAT_UP 0.75
#define TEXTURE_GAIN_FLAT_DOWN 0.25
#define TEXTURE_GAIN_STEP 0.1

//Constants to hold color at peaks and valleys.
#define PEAK_STEP 0.1
#define VALLEY_STEP 0.1
#define INITIAL_PEAK_RED 1.0
#define INITIAL_PEAK_GREEN 1.0
#define INITIAL_PEAK_BLUE 1.0
#define INITIAL_VALLEY_RED 0.0
#define INITIAL_VALLEY_GREEN 0.0
#define INITIAL_VALLEY_BLUE 0.0

#define ADD 0
#define MULT 1

//Global variables defining the coordinate system.
GLdouble xOrigin;
GLdouble yOrigin;
GLdouble verticalScale;

// Global variable defining the horizontal grid resolution
// in each direction.
int resolution = MIN_RESOLUTION;

//Global variables defining the terrain properties.
GLdouble fractalIncrement;
GLdouble lacunarity;
int octaves;
int addOrMult = ADD;
typedef enum {VALUE, GRADIENT} noiseGeneratorType;
noiseGeneratorType noiseType;
GLdouble exponentArray[MAX_OCTAVES];
GLdouble minimumFrequency;


//Global variables to hold color at peaks and valleys.
GLfloat peakRed = INITIAL_PEAK_RED;
GLfloat peakGreen = INITIAL_PEAK_GREEN;
GLfloat peakBlue = INITIAL_PEAK_BLUE;
GLfloat valleyRed = INITIAL_VALLEY_RED;
GLfloat valleyGreen = INITIAL_VALLEY_GREEN;
GLfloat valleyBlue = INITIAL_VALLEY_BLUE;
GLfloat textureGain = INITIAL_TEXTURE_GAIN;
GLfloat textureBias = INITIAL_TEXTURE_BIAS;

//Variables for storing the texture.
static GLubyte textureImage[TEXTURE_IMAGE_LENGTH][4];
static GLuint texName;

// Variables to hold lighting description:
GLfloat shininess = INITIAL_SHININESS;
GLfloat specular = INITIAL_SPECULAR;
GLfloat diffuseRed=1.0, diffuseGreen=1.0, diffuseBlue=1.0;
typedef enum { RED, GREEN, BLUE} colorType;
colorType color;

//Global variables for keeping track of the size and shape
//of the window.
int windowHeight, windowWidth;

//Enumerated type and global variable for keeping track
//of the selected operation.
typedef enum { SCALE, TRANSLATE, ROTATE, POSITION, AIM, ORIENTATION, ZOOM, HOME,
	       PROJECTION, NOTHING, PEAK, VALLEY, SPECULAR, SHININESS,
	       TEXTURE_BIAS, TEXTURE_GAIN, RESOLUTION, MINIMUM_FREQUENCY, VERTICAL_SCALE, FRACTAL_INCREMENT,
	       LACUNARITY, OCTAVES, NOISE_TYPE, ADD_OR_MULT} operationType;
operationType operation = POSITION;

//Enumerated type and global variable for talking about axies.
typedef enum { X, Y, Z } axisType;
axisType axis = Z;

//Enumerated type and global variable for talking about direction of
//changes in camera position, and aim and the direction of time steps
//and animation.
typedef enum { DOWN, UP } directionType;
directionType direction = UP;

//Enumerated type and global variable for keeping track of the type of
//projection being used.
typedef enum { ORTHOGRAPHIC, PERSPECTIVE } projectionType;
projectionType  projection = PERSPECTIVE;

//Global variable for keeping track of the camera field of view.
GLdouble fov = INITIAL_FOV;

//Global variables for keeping track of the camera position.
GLdouble xEye = INITIAL_EYE_X;
GLdouble yEye = INITIAL_EYE_Y;
GLdouble zEye = INITIAL_EYE_Z;

//Global variables for keeping track of the camera aim.
GLdouble xCen = INITIAL_CEN_X;
GLdouble yCen = INITIAL_CEN_Y;
GLdouble zCen = INITIAL_CEN_Z;


//Global variables for keeping track of the camera orientation.
GLdouble xUp = INITIAL_UP_X;
GLdouble yUp = INITIAL_UP_Y;
GLdouble zUp = INITIAL_UP_Z;

//Height map defining the terrain.
GLdouble terrainHeight[MAX_RESOLUTION][MAX_RESOLUTION];

//Normal map for the terrain.
GLdouble terrainNormalX[MAX_RESOLUTION][MAX_RESOLUTION];
GLdouble terrainNormalY[MAX_RESOLUTION][MAX_RESOLUTION];
GLdouble terrainNormalZ[MAX_RESOLUTION][MAX_RESOLUTION];

// Global variable for storing the model view matrix.
GLdouble modelViewMatrix[ 16 ];

// Values derived from terrain height map.
GLdouble average[4];
GLdouble initialAverage[4];

//Initialize the texture.
void initializeTexture()
{
   GLdouble imageLength = GLdouble(TEXTURE_IMAGE_LENGTH);
   for (int i = 0; i < TEXTURE_IMAGE_LENGTH; i++)
	{
	  GLdouble alpha = gain(textureGain,bias(textureBias,GLdouble(i)/imageLength));
	  textureImage[i][0] = GLubyte(255 * (valleyRed + alpha * (peakRed-valleyRed)));
	  textureImage[i][1] = GLubyte(255 * (valleyGreen + alpha * (peakGreen-valleyGreen)));
	  textureImage[i][2] = GLubyte(255 * (valleyBlue + alpha * (peakBlue-valleyBlue)));
	  textureImage[i][3] = (GLubyte) 255;
	}
   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_1D, texName);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

   glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, TEXTURE_IMAGE_LENGTH,
                0, GL_RGBA, GL_UNSIGNED_BYTE, textureImage);
   gluBuild1DMipmaps(GL_TEXTURE_1D, GL_RGBA, TEXTURE_IMAGE_LENGTH,
		     GL_RGBA, GL_UNSIGNED_BYTE, textureImage);
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


void setLighting()
// Initialize or reset the lighting arrangement.
{

   if (specular < 0.0) specular = 0.0; else if (specular > 1.0) specular = 1.0;
   if (shininess < 0.0) shininess = 0.0;

   GLfloat mat_diffuse[] = { diffuseRed, diffuseGreen, diffuseBlue, 1.0 };
   GLfloat mat_specular[] = { specular, specular, specular, 1.0 };
   GLfloat mat_shininess[] = { shininess };

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);
}

void rotate()
// Rotate the scene around the centroid of the terrain, in a direction
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
// Scale the scene from centroid of the terrain uniformly in all directions.
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


// Generate a terrain height at (x,y) using FBM noise based on the passed
// (value or gradient) noise function.
double fBm(GLdouble x, GLdouble y, GLdouble noise(GLdouble,GLdouble,GLdouble))
{
	GLdouble value;
	if (addOrMult==ADD) value = 0.0; else value = 1.0;
	for (int i=0; i<octaves; i++)
	  {
	    switch(addOrMult)
	      {
	      case ADD: value += noise(x,y,0.0)*exponentArray[i]; break;
	      case MULT: value *= 1.0 + noise(x,y,0.0)*exponentArray[i]; break;
	      }
	    x *= lacunarity;
	    y *= lacunarity;
	  }
	return(value);
}

// Initialize the terrain height map using FBM noise.
void initializeTerrainHeightMap()
{
  GLdouble min = 1000.0;
  GLdouble max = -1000.0;
  GLdouble (*noiseFunction)(GLdouble,GLdouble,GLdouble) = NULL;

  for(int i=0; i<octaves; i++) exponentArray[i] = pow(lacunarity,-i*fractalIncrement);

  switch(noiseType)
    {
       case VALUE : noiseFunction = vnoise; break;
       case GRADIENT : noiseFunction = gnoise; break;
    }

  for (int c = 0; c<resolution; c++)
    {
      GLdouble x = minimumFrequency * GLdouble(c)/GLdouble(resolution-1);
      for (int r = 0; r<resolution; r++)
  	{
	  GLdouble y = minimumFrequency * GLdouble(r)/GLdouble(resolution-1);
	  GLdouble value = fBm(x,y,noiseFunction);
	  if (value < min) min = value;
	  if (value > max) max = value;
	  terrainHeight[r][c] = value;
	}
    }

  GLdouble heightTotal = 0.0;
  for (int c = 0; c<resolution; c++)
      for (int r = 0; r<resolution; r++)
	{
   	  terrainHeight[r][c] = (terrainHeight[r][c] - min)/(max - min);
	  heightTotal += terrainHeight[r][c];
	}

  initialAverage[0] =  0.0;
  initialAverage[1] =  0.0;
  initialAverage[2] =  verticalScale * heightTotal / (resolution * resolution);
  initialAverage[3] =  1.0;

  multiplyMatrixVector(modelViewMatrix,initialAverage,average,4);
  average[0] = average[0]/average[3];
  average[1] = average[1]/average[3];
  average[2] = average[2]/average[3];
}

// Average two normal vectors and return the (normalized) result.
inline void normalAverage(GLdouble nx1, GLdouble ny1, GLdouble nz1,
			  GLdouble nx2, GLdouble ny2, GLdouble nz2,
			  GLdouble& nX, GLdouble& nY, GLdouble& nZ)
{

  GLdouble ax = (nx1 + nx2)/2.0;
  GLdouble ay = (ny1 + ny2)/2.0;
  GLdouble az = (nz1 + nz2)/2.0;
  GLdouble ln = sqrt(ax*ax + ay*ay + az*az);
  nX = ax / ln;
  nY = ay / ln;
  nZ = az / ln;
}


// Compute the surface normal of a triangle formed by three points.
// The direction of the normal follows the right hand rule based on
// the cross product of vector (x2-x1,y2-y1,z2-z1) and
// (x3-x1,y3-y1,z3-z1).
inline void triangleNormal(GLdouble x1, GLdouble y1, GLdouble z1,
			   GLdouble x2, GLdouble y2, GLdouble z2,
			   GLdouble x3, GLdouble y3, GLdouble z3,
			   GLdouble& nX, GLdouble& nY, GLdouble& nZ)
{
  GLdouble xa,ya,za;
  GLdouble xb,yb,zb;
  GLdouble x,y,z;
  GLdouble ln;
  xa = x2 - x1;
  ya = y2 - y1;
  za = z2 - z1;
  xb = x3 - x1;
  yb = y3 - y1;
  zb = z3 - z1;
  x = ya*zb - za*yb;
  y = za*xb - xa*zb;
  z = xa*yb - ya*xb;
  ln = sqrt(x*x + y*y + z*z);
  nX = x/ln;
  nY = y/ln;
  nZ = z/ln;
}


// Initialize the normals of the vertices at the corners of the height map array.
void initializeCornerNormals()
{
  int w = resolution;
  int h = resolution;
  GLdouble xScale = 1.0/(GLdouble)(resolution-1);
  GLdouble yScale = 1.0/(GLdouble)(resolution-1);
  triangleNormal(xOrigin+0*xScale,yOrigin+0*yScale,terrainHeight[0][0],
		 xOrigin+1*xScale,yOrigin+0*yScale,terrainHeight[0][1],
		 xOrigin+0*xScale,yOrigin+1*yScale,terrainHeight[1][0],
		 terrainNormalX[0][0],terrainNormalY[0][0],terrainNormalZ[0][0]);

  triangleNormal(xOrigin+(w-1)*xScale,yOrigin+0*yScale,terrainHeight[0][w-1],
		 xOrigin+(w-1)*xScale,yOrigin+1*yScale,terrainHeight[1][w-1],
		 xOrigin+(w-2)*xScale,yOrigin+0*yScale,terrainHeight[0][w-2],
		 terrainNormalX[0][w-1],terrainNormalY[0][w-1],terrainNormalZ[0][w-1]);

  triangleNormal(xOrigin+0*xScale,yOrigin+(h-1)*yScale,terrainHeight[h-1][0],
		 xOrigin+0*xScale,yOrigin+(h-2)*yScale,terrainHeight[h-2][0],
		 xOrigin+1*xScale,yOrigin+(h-1)*yScale,terrainHeight[h-1][1],
		 terrainNormalX[h-1][0],terrainNormalY[h-1][0],terrainNormalZ[h-1][0]);

  triangleNormal(xOrigin+(w-1)*xScale,yOrigin+(h-1)*yScale,terrainHeight[h-1][w-1],
		 xOrigin+(w-2)*xScale,yOrigin+(h-1)*yScale,terrainHeight[h-1][w-2],
		 xOrigin+(w-1)*xScale,yOrigin+(h-2)*yScale,terrainHeight[h-2][w-1],
		 terrainNormalX[h-1][w-1],terrainNormalY[h-1][w-1],terrainNormalZ[h-1][w-1]);
}


// Initialize the normals on the sides of the height map array.
void initializeSideNormals()
{
  int w = resolution;
  int h = resolution;
  GLdouble xScale = 1.0/(GLdouble)(resolution-1);
  GLdouble yScale = 1.0/(GLdouble)(resolution-1);
  GLdouble n1x, n1y, n1z, n2x, n2y, n2z;

  for (int c = 1; c < w-1; c++)
    {
      triangleNormal(xOrigin+c*xScale,    yOrigin+0*yScale,terrainHeight[0][c],
		     xOrigin+(c+1)*xScale,yOrigin+0*yScale,terrainHeight[0][c+1],
		     xOrigin+c*xScale,    yOrigin+1*yScale,terrainHeight[1][c],
		     n1x,n1y,n1z);
      triangleNormal(xOrigin+c*xScale,    yOrigin+0*yScale,terrainHeight[0][c],
		     xOrigin+c*xScale,    yOrigin+1*yScale,terrainHeight[1][c],
		     xOrigin+(c-1)*xScale,yOrigin+0*yScale,terrainHeight[0][c-1],
		     n2x,n2y,n2z);
      normalAverage(n1x,n1y,n1z,n2x,n2y,n2z,
		    terrainNormalX[0][c],terrainNormalY[0][c],terrainNormalZ[0][c]);

      triangleNormal(xOrigin+c*xScale,    yOrigin+(h-1)*yScale,terrainHeight[h-1][c],
		     xOrigin+(c-1)*xScale,yOrigin+(h-1)*yScale,terrainHeight[h-1][c-1],
		     xOrigin+c*xScale,    yOrigin+(h-2)*yScale,terrainHeight[h-2][c],
		     n1x,n1y,n1z);
      triangleNormal(xOrigin+c*xScale,    yOrigin+(h-1)*yScale,terrainHeight[h-1][c],
		     xOrigin+c*xScale,    yOrigin+(h-2)*yScale,terrainHeight[h-2][c],
		     xOrigin+(c+1)*xScale,yOrigin+(h-1)*yScale,terrainHeight[h-1][c+1],
		     n2x,n2y,n2z);
      normalAverage(n1x,n1y,n1z,n2x,n2y,n2z,
		    terrainNormalX[h-1][c],terrainNormalY[h-1][c],terrainNormalZ[h-1][c]);
    }

  for (int r = 1; r < h-1; r++)
    {
      triangleNormal(xOrigin+0*xScale, yOrigin+r*yScale,terrainHeight[r][0],
		     xOrigin+0*xScale, yOrigin+(r-1)*yScale,terrainHeight[r-1][0],
		     xOrigin+1*xScale, yOrigin+r*yScale,terrainHeight[r][1],
		     n1x,n1y,n1z);
      triangleNormal(xOrigin+0*xScale, yOrigin+r*yScale,terrainHeight[r][0],
		     xOrigin+1*xScale, yOrigin+r*yScale,terrainHeight[r][1],
		     xOrigin+0*xScale, yOrigin+(r+1)*yScale,terrainHeight[r+1][0],
		     n2x,n2y,n2z);
      normalAverage(n1x,n1y,n1z,n2x,n2y,n2z,
		    terrainNormalX[r][0],terrainNormalY[r][0],terrainNormalZ[r][0]);

      triangleNormal(xOrigin+(w-1)*xScale, yOrigin+r*yScale,terrainHeight[r][w-1],
		     xOrigin+(w-1)*xScale, yOrigin+(r+1)*yScale,terrainHeight[r+1][w-1],
		     xOrigin+(w-2)*xScale, yOrigin+r*yScale,terrainHeight[r][w-2],
		     n1x,n1y,n1z);
      triangleNormal(xOrigin+(w-1)*xScale, yOrigin+r*yScale,terrainHeight[r][w-1],
		     xOrigin+(w-2)*xScale, yOrigin+r*yScale,terrainHeight[r][w-2],
		     xOrigin+(w-1)*xScale, yOrigin+(r-1)*yScale,terrainHeight[r-1][w-1],
		     n2x,n2y,n2z);
      normalAverage(n1x,n1y,n1z,n2x,n2y,n2z,
		    terrainNormalX[r][w-1],terrainNormalY[r][w-1],terrainNormalZ[r][w-1]);
    }
}

// Initialize the normals on the interior of the height map array.
void initializeCenterNormals()
{
  int w = resolution;
  int h = resolution;
  GLdouble xScale = 1.0/(GLdouble)(resolution-1);
  GLdouble yScale = 1.0/(GLdouble)(resolution-1);
  GLdouble n1x, n1y, n1z, n2x, n2y, n2z;
  GLdouble n3x, n3y, n3z, n4x, n4y, n4z;
  GLdouble a1x,a1y,a1z,a2x,a2y,a2z;

  for (int c = 1; c < w-1; c++)
      for (int r = 1; r < h-1; r++)
	{

	  triangleNormal(xOrigin+c*xScale,     yOrigin+r*yScale,terrainHeight[r][c],
			 xOrigin+(c+1)*xScale, yOrigin+r*yScale,terrainHeight[r][c+1],
			 xOrigin+c*xScale, yOrigin+(r+1)*yScale,terrainHeight[r+1][c],
			 n1x,n1y,n1z);
	  triangleNormal(xOrigin+c*xScale,     yOrigin+r*yScale,terrainHeight[r][c],
			 xOrigin+c*xScale, yOrigin+(r+1)*yScale,terrainHeight[r+1][c],
			 xOrigin+(c-1)*xScale, yOrigin+r*yScale,terrainHeight[r][c-1],
			 n2x,n2y,n2z);
	  normalAverage(n1x,n1y,n1z,n2x,n2y,n2z,a1x,a1y,a1z);

	  triangleNormal(xOrigin+c*xScale,     yOrigin+r*yScale,terrainHeight[r][c],
			 xOrigin+(c-1)*xScale, yOrigin+r*yScale,terrainHeight[r][c-1],
			 xOrigin+c*xScale, yOrigin+(r-1)*yScale,terrainHeight[r-1][c],
			 n3x,n3y,n3z);
	  triangleNormal(xOrigin+c*xScale,     yOrigin+r*yScale,terrainHeight[r][c],
			 xOrigin+c*xScale, yOrigin+(r-1)*yScale,terrainHeight[r-1][c],
			 xOrigin+(c+1)*xScale, yOrigin+r*yScale,terrainHeight[r][c+1],
			 n4x,n4y,n4z);
	  normalAverage(n2x,n3y,n3z,n4x,n4y,n4z,a2x,a2y,a2z);

	  normalAverage(a1x,a1y,a1z,a2x,a2y,a2z,
			terrainNormalX[r][c],terrainNormalY[r][c],terrainNormalZ[r][c]);
	}
}

// Initialize all the normals of the vertices in the height map array.
void initializeTerrainNormalMaps()
{

  initializeCornerNormals();
  initializeSideNormals();
  initializeCenterNormals();
}


// Initialize the terrain by initializing the height map and the normal map.
void initializeTerrain()
{
  initializeTerrainHeightMap();
  initializeTerrainNormalMaps();
}

// Render the terain using the normal map, the height map and the texture map.
void displayTerrain()
{
  GLfloat alpha;

   glEnable(GL_TEXTURE_1D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBindTexture(GL_TEXTURE_1D, texName);

  GLdouble xScale = 1.0/(GLdouble)(resolution-1);
  GLdouble yScale = 1.0/(GLdouble)(resolution-1);

  for (int r=0; r< resolution-1; r++)
    {
      glBegin(GL_QUAD_STRIP);
      for (int c=0; c< resolution; c++)
	{

	  GLdouble x1, y1, z1, x2, y2, z2;
	  GLdouble nx1, ny1, nz1, nx2, ny2, nz2;

	  x1 = xOrigin+c*xScale;
          y1 = yOrigin+r*yScale;
	  alpha = terrainHeight[r][c];
	  z1 = verticalScale * alpha;

	  glTexCoord1d(alpha);
	  //	  glNormal3d(nx1,ny1,nz1);
	  glNormal3d(0.0,0.0,1.0);
	  glVertex3d(x1,y1,z1);

	  x2 = xOrigin+c*xScale;
          y2 = yOrigin+(r+1)*yScale;
	  alpha = terrainHeight[r+1][c];
	  z2 = verticalScale * alpha;

	  glTexCoord1d(alpha);
	  //	  glNormal3d(nx2,ny2,nz2);
	  glNormal3d(0.0,0.0,1.0);
	  glVertex3d(x2,y2,z2);

	}
      glEnd();
    }

}


void display()
//Callback for redisplaying the image.
{
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //Set the camera position, aim and orientation.
   glLoadIdentity();
   gluLookAt (xEye, yEye, zEye, xCen, yCen, zCen, xUp, yUp, zUp);

   //Translate, rotate and scale the object.
   glMatrixMode(GL_MODELVIEW);
   glMultMatrixd(modelViewMatrix);


   //Set the projection type and clipping planes.
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (projection == ORTHOGRAPHIC)
      glOrtho( -LEFT_RIGHT_CLIP, LEFT_RIGHT_CLIP,
	       -BOTTOM_TOP_CLIP, BOTTOM_TOP_CLIP,
	       -FRONT_BACK_CLIP, FRONT_BACK_CLIP );
   else  gluPerspective(fov, (GLdouble) windowWidth / (GLdouble) windowHeight,
                        NEAR_CLIP, FAR_CLIP);
   glMatrixMode(GL_MODELVIEW);

   displayTerrain();

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
      glOrtho( -LEFT_RIGHT_CLIP, LEFT_RIGHT_CLIP,
	       -BOTTOM_TOP_CLIP, BOTTOM_TOP_CLIP,
	       -FRONT_BACK_CLIP, FRONT_BACK_CLIP );
   else  gluPerspective(fov, (GLdouble) windowWidth / (GLdouble) windowHeight,
                        NEAR_CLIP, FAR_CLIP);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void homePosition()
//Putting the terrain and all viewing varibles back to their original states.
{
 axis = Z;
 direction = UP;
 operation = POSITION;
 xEye = INITIAL_EYE_X;
 yEye = INITIAL_EYE_Y;
 zEye = INITIAL_EYE_Z;
 xCen = INITIAL_CEN_X;
 yCen = INITIAL_CEN_Y;
 zCen = INITIAL_CEN_Z;
 xUp = INITIAL_UP_X;
 yUp = INITIAL_UP_Y;
 zUp = INITIAL_UP_Z;
 fov = INITIAL_FOV;
 xOrigin = INITIAL_X_ORIGIN;
 yOrigin = INITIAL_Y_ORIGIN;
 verticalScale = INITIAL_VERTICAL_SCALE;
 minimumFrequency = INITIAL_MINIMUM_FREQUENCY;
 fractalIncrement = INITIAL_FRACTAL_INCREMENT;
 lacunarity = INITIAL_LACUNARITY;
 octaves = INITIAL_OCTAVES;
 noiseType = GRADIENT;
 resolution = MIN_RESOLUTION;
 addOrMult = ADD;
 glMatrixMode(GL_MODELVIEW);
 glLoadIdentity();
 glGetDoublev(  GL_MODELVIEW_MATRIX, modelViewMatrix );
 multiplyMatrixVector(modelViewMatrix,initialAverage,average,4);
 average[0] = average[0]/average[3];
 average[1] = average[1]/average[3];
 average[2] = average[2]/average[3];
 glutPostRedisplay();
}


void operate()
//Process the operation that the user has selected.
{
 int sign;
 if (direction == UP) sign = 1; else sign = -1;
 switch (operation)
   {
   case SCALE         : scale(); break;
   case TRANSLATE     : translate(); break;
   case ROTATE        : rotate(); break;
   case POSITION      : switch (axis)
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
                        break;
   case AIM           : switch (axis)
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
                        break;
   case ORIENTATION   : break;
   case ZOOM          : fov += sign * FOV_STEP; break;
   case HOME          : break;
   case PROJECTION    : break;
   case NOTHING       : break;
   case PEAK          : switch (color)
                        {
			  case RED  : peakRed += sign * PEAK_STEP;
			              if (peakRed<0.0) peakRed=0.0;
			              else if (peakRed>1.0) peakRed=1.0;
			              break;
  			  case GREEN  : peakGreen += sign * PEAK_STEP;
			              if (peakGreen<0.0) peakGreen=0.0;
			              else if (peakGreen>1.0) peakGreen=1.0;
			              break;
			  case BLUE  : peakBlue += sign * PEAK_STEP;
			              if (peakBlue<0.0) peakBlue=0.0;
			              else if (peakBlue>1.0) peakBlue=1.0;
			              break;
			}
                        initializeTexture();
			break;
   case VALLEY         : switch (color)
                        {
			  case RED  : valleyRed += sign * VALLEY_STEP;
			              if (valleyRed<0.0) valleyRed=0.0;
			              else if (valleyRed>1.0) valleyRed=1.0;
			              break;
  			  case GREEN  : valleyGreen += sign * VALLEY_STEP;
			              if (valleyGreen<0.0) valleyGreen=0.0;
			              else if (valleyGreen>1.0) valleyGreen=1.0;
			              break;
			  case BLUE  : valleyBlue += sign * VALLEY_STEP;
			              if (valleyBlue<0.0) valleyBlue=0.0;
			              else if (valleyBlue>1.0) valleyBlue=1.0;
			              break;
			}
                        initializeTexture();
			break;
   case SPECULAR      : specular += sign * SPECULAR_STEP;
                        setLighting();
			break;
   case SHININESS     : shininess += sign * SHININESS_STEP;
                        setLighting();
			break;
   case TEXTURE_BIAS    : if (textureBias >= TEXTURE_BIAS_FLAT_UP)
                          switch (sign)
                          {
			   case -1 :  textureBias = 1.0-(1.0-textureBias)/TEXTURE_BIAS_FACTOR; break;
			   case  1 :  textureBias = 1.0-(1.0-textureBias)*TEXTURE_BIAS_FACTOR; break;
			  }
                          else if (textureBias <= TEXTURE_BIAS_FLAT_DOWN)
                          switch (sign)
                          {
			   case -1 :  textureBias = textureBias*TEXTURE_BIAS_FACTOR; break;
			   case  1 :  textureBias = textureBias/TEXTURE_BIAS_FACTOR; break;
			  }
                          else switch (sign)
                          {
			   case -1 :  textureBias -= TEXTURE_BIAS_STEP; break;
			   case  1 :  textureBias += TEXTURE_BIAS_STEP; break;
			  }
			initializeTexture();
			break;
   case TEXTURE_GAIN    : if (textureGain >= TEXTURE_GAIN_FLAT_UP)
                          switch (sign)
                          {
			   case -1 :  textureGain = 1.0-(1.0-textureGain)/TEXTURE_GAIN_FACTOR; break;
			   case  1 :  textureGain = 1.0-(1.0-textureGain)*TEXTURE_GAIN_FACTOR; break;
			  }
                          else if (textureGain <= TEXTURE_GAIN_FLAT_DOWN)
                          switch (sign)
                          {
			   case -1 :  textureGain = textureGain*TEXTURE_GAIN_FACTOR; break;
			   case  1 :  textureGain = textureGain/TEXTURE_GAIN_FACTOR; break;
			  }
                          else switch (sign)
                          {
			   case -1 :  textureGain -= TEXTURE_GAIN_STEP; break;
			   case  1 :  textureGain += TEXTURE_GAIN_STEP; break;
			  }
			initializeTexture();
			break;
   case RESOLUTION    : if (sign==1) resolution = resolution*RESOLUTION_FACTOR;
                        else resolution = resolution/RESOLUTION_FACTOR;
                        if (resolution < MIN_RESOLUTION)
			  {
			    resolution = MIN_RESOLUTION;
			    cout << "\007" << flush;
			  }
                        else if (resolution > MAX_RESOLUTION)
			  {
			    resolution = MAX_RESOLUTION;
			    cout << "\007" << flush;
			  }
			initializeTerrain();
			break;
   case MINIMUM_FREQUENCY : if (sign==1) minimumFrequency *= SCALE_FACTOR;
                         else minimumFrequency /= SCALE_FACTOR;
                         initializeTerrain();
                         break;
   case VERTICAL_SCALE : if (sign==1) verticalScale *= SCALE_FACTOR;
                         else verticalScale /= SCALE_FACTOR;
                         break;
   case FRACTAL_INCREMENT     : fractalIncrement += sign * FRACTAL_INCREMENT_STEP;
                         if (fractalIncrement < 0.0) fractalIncrement = 0.0;
			 initializeTerrain();
			 break;
   case LACUNARITY     : lacunarity += sign * LACUNARITY_STEP;
                         if (lacunarity < 1.0) lacunarity = 1.0;
			 initializeTerrain();
			 break;
   case OCTAVES        : octaves += sign * OCTAVES_STEP;
                         if (octaves < 1) octaves = 1;
			 if (octaves > MAX_OCTAVES) octaves = MAX_OCTAVES;
			 initializeTerrain();
			 break;
   case NOISE_TYPE     : break;
   case ADD_OR_MULT    : break;
   }
 glutPostRedisplay();
}


void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 0 : operation = ZOOM; break;
 case 1 : operation = SCALE; break;
 case 2 : homePosition();
          glutPostRedisplay();
	  break;
 case 5 : operation = SPECULAR; break;
 case 6 : operation = SHININESS; break;
 case 7 : operation = VERTICAL_SCALE; break;
 case 8 : operation = RESOLUTION; break;
 case 9 : operation = NOISE_TYPE; break;
 case 10: operation = ADD_OR_MULT; break;
 case 17 : operation = MINIMUM_FREQUENCY; break;
 case 11: operation = FRACTAL_INCREMENT; break;
 case 12: operation = LACUNARITY; break;
 case 13: operation = OCTAVES; break;
 case 14: operation = TEXTURE_BIAS; break;
 case 15: operation = TEXTURE_GAIN; break;
 case 16: valueTabInit();
          gradientTabInit();
          initializeTerrain();
          glutPostRedisplay();
	  break;
 case 3 : std::exit( 0 );
 }
}


void aimSubMenu( int item )
// Callback for processing camera change aim submenu.
{
  operation = AIM;
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

void valleySubMenu( int item )
// Callback for processing valley lighting submenu.
{
  switch ( item )
  {
   case 1 : {color = RED; break; }
   case 2 : {color = GREEN; break; }
   case 3 : {color = BLUE; break; }
  }
  operation = VALLEY;
  glutPostRedisplay();
}


void peakSubMenu( int item )
// Callback for processing peak lighting submenu.
{
  switch ( item )
  {
   case 1 : {color = RED; break; }
   case 2 : {color = GREEN; break; }
   case 3 : {color = BLUE; break; }
  }
  operation = PEAK;
  glutPostRedisplay();
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


void addMultSubMenu ( int item )
// Callback for processing add/mult submenu.
{
  operation = ADD_OR_MULT;
  switch ( item )
  {
    case 0 : addOrMult = 0; initializeTerrain(); break;
    case 1 : addOrMult = 1; initializeTerrain(); break;
  }
  glutPostRedisplay();
}


void valueGradientSubMenu ( int item )
// Callback for processing value / gradient noise submenu.
{
  operation = NOISE_TYPE;
  switch ( item )
  {
    case 1 : noiseType = VALUE ; initializeTerrain(); break;
    case 2 : noiseType = GRADIENT; initializeTerrain(); break;
  }
  glutPostRedisplay();
}



void setMenus( )
// Routine for creating menus.
{

  int rotateSubMenuCode, translateSubMenuCode;
  int aimSubMenuCode, positionSubMenuCode, orientationSubMenuCode;
  int peakSubMenuCode, valleySubMenuCode, projectionSubMenuCode;
  int addMultSubMenuCode, valueGradientSubMenuCode;

  peakSubMenuCode = glutCreateMenu( peakSubMenu );
  glutAddMenuEntry( "Red",      1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",      3 );

  valleySubMenuCode = glutCreateMenu( valleySubMenu );
  glutAddMenuEntry( "Red",      1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",      3 );

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

  projectionSubMenuCode = glutCreateMenu( projectionSubMenu );
  glutAddMenuEntry( "Perspective",      1 );
  glutAddMenuEntry( "Orthographic",      2 );

  rotateSubMenuCode = glutCreateMenu( rotateSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  translateSubMenuCode = glutCreateMenu( translateSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  addMultSubMenuCode = glutCreateMenu( addMultSubMenu );
  glutAddMenuEntry( "Additive Combination",            0 );
  glutAddMenuEntry( "Multiplicative Combination",      1 );

  valueGradientSubMenuCode =  glutCreateMenu( valueGradientSubMenu );
  glutAddMenuEntry( "Value Noise",         1 );
  glutAddMenuEntry( "Gradient Noise",      2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Rotate ...",  rotateSubMenuCode );
  glutAddSubMenu( "Translate ...", translateSubMenuCode );
  glutAddMenuEntry( "Scale", 1 );
  glutAddMenuEntry( "Zoom",  0 );
  glutAddMenuEntry( "Terrain Specular Property",  5 );
  glutAddMenuEntry( "Terrain Shininess Property",  6 );
  glutAddSubMenu( "Camera Aim ...",  aimSubMenuCode );
  glutAddSubMenu( "Camera Position ...", positionSubMenuCode );
  glutAddSubMenu( "Camera Orientation ...", orientationSubMenuCode );
  glutAddSubMenu( "Camera Projection ...", projectionSubMenuCode );

  glutAddMenuEntry( "Vertical Scale ...", 7 );
  glutAddMenuEntry( "Resolution ...", 8 );
  glutAddSubMenu( "Value/Gradient...", valueGradientSubMenuCode );
  glutAddSubMenu( "Add/Multiply ...", addMultSubMenuCode );
  glutAddMenuEntry( "Minimum Frequency ...", 17 );
  glutAddMenuEntry( "Fractal Increment ...", 11 );
  glutAddMenuEntry( "Lacunarity ...", 12 );
  glutAddMenuEntry( "Number of Octaves ...", 13 );
  glutAddSubMenu( "Peak Shading ...",  peakSubMenuCode );
  glutAddSubMenu( "Valley Shading ...",  valleySubMenuCode );
  glutAddMenuEntry( "Altitude Bias  ...", 14 );
  glutAddMenuEntry( "Altitude Gain ...",  15);
  glutAddMenuEntry( "Randomize ...",  16);
  glutAddMenuEntry( "Reset",  2 );
  glutAddMenuEntry( "Exit",  3 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

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



int main(int argc, char** argv)
{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
   glutInitWindowSize (800,800);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Fractal Noise Program");
   glShadeModel (GL_SMOOTH);
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc( mouse );
   glEnable( GL_DEPTH_TEST );

   setMenus();
   homePosition();
   setLighting();

   // Initialize random number table for value noise.
   valueTabInit();
   // Initialize random number table for gradient noise.
   gradientTabInit();

   initializeTexture();
   initializeTerrain();

   glutMainLoop();
   return 0;
}
