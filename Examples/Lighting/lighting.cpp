/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics        Tom Ellman  */
/*--------------------------------------------------------*/
/*  lighting.cpp : Light Definition Demo                  */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>
#include <iostream>

using namespace std;


#define RESOLUTION_INCREMENT 1
#define FOV_INCREMENT 5.0
#define POSITION_INCREMENT 0.2
#define COLOR_INCREMENT 0.1
#define SHININESS_FACTOR 1.414;

typedef enum {LIGHT_POSITION, CAMERA_POSITION, RESOLUTION, FOV,
	      LIGHT_AMBIENT, LIGHT_DIFFUSE, LIGHT_SPECULAR,
	      SPHERE_AMBIENT, SPHERE_DIFFUSE, SPHERE_SPECULAR,
	      SPHERE_SHININESS, SPHERE_EMISSION} parameterType;
typedef enum {RED=0, GREEN, BLUE, ALL} colorType;
typedef enum { X, Y, Z } axisType;
typedef enum { DOWN, UP } directionType;

parameterType parameter;
colorType color;
directionType direction;
axisType axis;

GLenum   shading;

GLfloat ambientMaterialProperty[4];
GLfloat diffuseMaterialProperty[4];
GLfloat specularMaterialProperty[4];
GLfloat shininessMaterialProperty[1];
GLfloat emissionMaterialProperty[4];

GLfloat ambientLighting[4];
GLfloat diffuseLighting[4];
GLfloat specularLighting[4];
GLfloat lightPosition[3];
GLfloat lightModelAmbient[4];
GLfloat localView[1];

GLfloat cameraPosition[3];

GLfloat fov;

int resolution;

void initLightModel()
{
   glEnable(GL_DEPTH_TEST);

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLighting);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLighting);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specularLighting);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, localView);

   glEnable(GL_LIGHTING);

   glEnable(GL_LIGHT0);
}

void setMaterialProperties()
{
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambientMaterialProperty);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterialProperty);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specularMaterialProperty);
   glMaterialfv(GL_FRONT, GL_SHININESS, shininessMaterialProperty);
   glMaterialfv(GL_FRONT, GL_EMISSION, emissionMaterialProperty);
}


void update(GLfloat* array, int index, GLfloat change)
{
  if (index==(int)ALL)
    for(int i=0; i<3; i++)
      {
	array[i]+= change;
	if (array[i] < 0.0) array[i] = 0.0;
	if (array[i] > 1.0) array[i] = 1.0;
      }
  else
    {
      array[index]+= change;
      if (array[index] < 0.0) array[index] = 0.0;
      if (array[index] > 1.0) array[index] = 1.0;
    }
}

void operate()
{
 GLfloat change;
 GLfloat factor = SHININESS_FACTOR;

 if (parameter == LIGHT_POSITION) change = POSITION_INCREMENT;
  else if (parameter == CAMERA_POSITION) change = POSITION_INCREMENT;
 else if (parameter == RESOLUTION) change = RESOLUTION_INCREMENT;
 else if (parameter == FOV) change = FOV_INCREMENT;
 else change = COLOR_INCREMENT;

 if (direction == DOWN)
    {
      change = (-1) * change;
      factor = 1/factor;
    }


 switch (parameter)
   {
    case LIGHT_POSITION    : switch (axis)
                               {
				  case X : lightPosition[0] += change; break;
				  case Y : lightPosition[1] += change; break;
				  case Z : lightPosition[2] += change; break;
			       }
                               break;
    case CAMERA_POSITION    : switch (axis)
                               {
				  case X : cameraPosition[0] += change; break;
				  case Y : cameraPosition[1] += change; break;
				  case Z : cameraPosition[2] += change; break;
			       }
                               break;
    case RESOLUTION        : resolution += (int) change; break;
    case FOV               : fov += change; break;
    case LIGHT_AMBIENT     : update(ambientLighting,color,change);break;
    case LIGHT_DIFFUSE     : update(diffuseLighting,color,change);break;
    case LIGHT_SPECULAR    : update(specularLighting,color,change);break;
    case SPHERE_AMBIENT    : update(ambientMaterialProperty,color,change); break;
    case SPHERE_DIFFUSE    : update(diffuseMaterialProperty,color,change); break;
    case SPHERE_SPECULAR   : update(specularMaterialProperty,color,change); break;
    case SPHERE_EMISSION   : update(emissionMaterialProperty,color,change); break;
    case SPHERE_SHININESS  : shininessMaterialProperty[0] *= factor; break;
   }
 glutPostRedisplay();
}

void reset()
{
   parameter = SPHERE_DIFFUSE;
   color = ALL;
   direction = UP;
   shading = GL_SMOOTH;

   ambientMaterialProperty[0] = 0.0;
   ambientMaterialProperty[1] = 0.0;
   ambientMaterialProperty[2] = 0.0;
   ambientMaterialProperty[3] = 1.0;

   diffuseMaterialProperty[0] = 1.0;
   diffuseMaterialProperty[1] = 0.0;
   diffuseMaterialProperty[2] = 0.0;
   diffuseMaterialProperty[3] = 1.0;

   specularMaterialProperty[0] = 0.8;
   specularMaterialProperty[1] = 0.8;
   specularMaterialProperty[2] = 0.8;
   specularMaterialProperty[3] = 1.0;

   emissionMaterialProperty[0] = 0.0;
   emissionMaterialProperty[1] = 0.0;
   emissionMaterialProperty[2] = 0.0;
   emissionMaterialProperty[3] = 1.0;

   shininessMaterialProperty[0] = 50.0;

   ambientLighting[0] = 0.0;
   ambientLighting[1] = 0.0;
   ambientLighting[2] = 0.0;
   ambientLighting[3] = 1.0;

   diffuseLighting[0] = 0.5;
   diffuseLighting[1] = 0.5;
   diffuseLighting[2] = 0.5;
   diffuseLighting[3] = 1.0;

   specularLighting[0] = 1.0;
   specularLighting[1] = 1.0;
   specularLighting[2] = 1.0;
   specularLighting[3] = 1.0;

   lightPosition[0] = 0.0;
   lightPosition[1] = 1.0;
   lightPosition[2] = 1.0;

   lightModelAmbient[0] = 0.0;
   lightModelAmbient[1] = 0.0;
   lightModelAmbient[2] = 0.0;
   lightModelAmbient[3] = 1.0;

   localView[0] =  0.0;

   cameraPosition[0] = 0.0;
   cameraPosition[1] = 0.0;
   cameraPosition[2] = 10.0;

   resolution = 32;

   fov = 75.0;
}



void display()
{
   glClearColor(0.5, 0.5, 0.5, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity( );
   gluLookAt( cameraPosition[0], cameraPosition[1], cameraPosition[2],
	      0.0, 0.0, 0.0,
	      0.0, 1.0, 0.0 );

   glMatrixMode( GL_PROJECTION );
   glLoadIdentity( );
   gluPerspective(fov, 1.0, 0.8, 100.0 );

   initLightModel();
   setMaterialProperties();
   glShadeModel(shading);

   glutSolidSphere(5.0, resolution, resolution);

   glFlush();
}

void reshape(int w, int h)
// Callback for processing reshape events.
{
  glViewport(0,0, (GLsizei)w, (GLsizei)h);
}

void parameterSubMenu( int item )
{
  switch ( item )
  {
    case 1 : parameter = LIGHT_AMBIENT; break;
    case 2 : parameter = LIGHT_DIFFUSE; break;
    case 3 : parameter = LIGHT_SPECULAR; break;
    case 4 : parameter = SPHERE_AMBIENT; break;
    case 5 : parameter = SPHERE_DIFFUSE; break;
    case 6 : parameter = SPHERE_SPECULAR; break;
    case 7 : parameter = SPHERE_SHININESS; break;
    case 8 : parameter = SPHERE_EMISSION; break;
  }
}


void colorSubMenu( int item )
{
  switch ( item )
  {
    case 1 : color = RED; break;
    case 2 : color = GREEN; break;
    case 3 : color = BLUE; break;
    case 4 : color = ALL; break;
  }
}

void shadingSubMenu( int item )
{
  switch ( item )
  {
    case 1 : shading = GL_FLAT; break;
    case 2 : shading = GL_SMOOTH; break;
  }
  glutPostRedisplay();
}

void localitySubMenu( int item )
{
  switch ( item )
  {
    case 1 : localView[0] = 0.0; break;
    case 2 : localView[0] = 1.0; break;
  }
  glutPostRedisplay();
}

void lightPositionSubMenu( int item )
// Callback for processing lightPosition submenu.
{
  parameter = LIGHT_POSITION;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void cameraPositionSubMenu( int item )
// Callback for processing lightPosition submenu.
{
  parameter = CAMERA_POSITION;
  switch ( item )
  {
    case 1 : axis = X; break;
    case 2 : axis = Y; break;
    case 3 : axis = Z; break;
  }
}

void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
    case 0 : parameter = FOV; break;
    case 1 : parameter = RESOLUTION; break;
    case 2 : reset(); break;
    case 3 : std::exit( 0 );
 }
}

void setMenus( )
{
  int parameterSubMenuCode;
  int colorSubMenuCode;
  int lightPositionSubMenuCode;
  int cameraPositionSubMenuCode;
  int shadingSubMenuCode;
  int localitySubMenuCode;

  parameterSubMenuCode = glutCreateMenu( parameterSubMenu );
  glutAddMenuEntry( "Light Ambient",        1 );
  glutAddMenuEntry( "Light Diffuse",        2 );
  glutAddMenuEntry( "Light Specular",       3 );
  glutAddMenuEntry( "Sphere Ambient",       4 );
  glutAddMenuEntry( "Sphere Diffuse",       5 );
  glutAddMenuEntry( "Sphere Specular",      6 );
  glutAddMenuEntry( "Sphere Shininess",     7 );
  glutAddMenuEntry( "Sphere Emission",      8 );

  colorSubMenuCode = glutCreateMenu( colorSubMenu );
  glutAddMenuEntry( "Red",        1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",       3 );
  glutAddMenuEntry( "All",        4 );

  lightPositionSubMenuCode = glutCreateMenu( lightPositionSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  cameraPositionSubMenuCode = glutCreateMenu( cameraPositionSubMenu );
  glutAddMenuEntry( "X Axis",      1 );
  glutAddMenuEntry( "Y Axis",      2 );
  glutAddMenuEntry( "Z Axis",      3 );

  shadingSubMenuCode = glutCreateMenu( shadingSubMenu );
  glutAddMenuEntry( "Flat",      1 );
  glutAddMenuEntry( "Smooth",      2 );

  localitySubMenuCode = glutCreateMenu( localitySubMenu );
  glutAddMenuEntry( "Infinity",      1 );
  glutAddMenuEntry( "Viewpoint",      2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Parameter  ...",  parameterSubMenuCode );
  glutAddSubMenu( "Color ...", colorSubMenuCode );
  glutAddSubMenu( "Shading ...", shadingSubMenuCode );
  glutAddSubMenu( "Light Position ...", lightPositionSubMenuCode );
  glutAddSubMenu( "Camera Position ...", cameraPositionSubMenuCode );
  glutAddMenuEntry( "Field of View", 0 );
  glutAddSubMenu( "Viewpoint Locality ...", localitySubMenuCode );
  glutAddMenuEntry( "Resolution", 1 );
  glutAddMenuEntry( "Reset",  2 );
  glutAddMenuEntry( "Exit",  3 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

void mouse( int button, int state, int, int)
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
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (600, 600);
   glutCreateWindow("Lighting Demo");
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutMouseFunc(mouse);

   setMenus();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   reset();

   glutMainLoop();
   return 0;
}

