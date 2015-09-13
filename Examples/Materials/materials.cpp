/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics        Tom Ellman  */
/*--------------------------------------------------------*/
/*  materials.cpp : Material Properties Demo              */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>
#include <iostream>

using namespace std;


#define SPHERE_SLICES 128
#define SPHERE_STACKS 128

#define COLOR_INCREMENT 0.1
#define SHININESS_INCREMENT 1.0

typedef enum {LIGHT_AMBIENT, LIGHT_DIFFUSE, LIGHT_SPECULAR,
	      SPHERE_AMBIENT, SPHERE_DIFFUSE, SPHERE_SPECULAR,
	      SPHERE_SHININESS, SPHERE_EMISSION, SPHERE_SHADING} parameterType;
typedef enum {TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT} sphereType;
typedef enum {RED=0, GREEN, BLUE, ALL} colorType;
typedef enum { DOWN, UP } directionType;

parameterType parameter;
sphereType sphere;
colorType color;
directionType direction;

GLenum   shadeTL = GL_SMOOTH;
GLenum   shadeTR = GL_SMOOTH;
GLenum   shadeBL = GL_SMOOTH;
GLenum   shadeBR = GL_SMOOTH;

   GLfloat ambientTL[] = {0.0,0.0,0.0,1.0};
   GLfloat ambientTR[] = {0.0,0.0,0.0,1.0};
   GLfloat ambientBL[] = {0.0,0.0,0.0,1.0};
   GLfloat ambientBR[] = {0.0,0.0,0.0,1.0};
   GLfloat diffuseTL[] = {0.0,0.0,0.0,1.0};
   GLfloat diffuseTR[] = {0.0,0.0,0.0,1.0};
   GLfloat diffuseBL[] = {0.0,0.0,0.0,1.0};
   GLfloat diffuseBR[] = {0.0,0.0,0.0,1.0};
   GLfloat specularTL[] = {0.0,0.0,0.0,1.0};
   GLfloat specularTR[] = {0.0,0.0,0.0,1.0};
   GLfloat specularBL[] = {0.0,0.0,0.0,1.0};
   GLfloat specularBR[] = {0.0,0.0,0.0,1.0};
   GLfloat shininessTL[] = {1.0};
   GLfloat shininessTR[] = {1.0};
   GLfloat shininessBL[] = {1.0};
   GLfloat shininessBR[] = {1.0};
   GLfloat emissionTL[] = {0.0, 0.0, 0.0, 0.0};
   GLfloat emissionTR[] = {0.0, 0.0, 0.0, 0.0};
   GLfloat emissionBL[] = {0.0, 0.0, 0.0, 0.0};
   GLfloat emissionBR[] = {0.0, 0.0, 0.0, 0.0};


   GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat position[] = { 0.0, 2.0, 2.0, 0.0 };
   GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat local_view[] = { 0.0 };


void copy(int size, GLfloat* source, GLfloat* destination)
{
  for (int i=0; i<size; i++) destination[i] = source[i];
}

void shift()
{
  copy(3,ambientBL,ambientBR);
  copy(3,ambientTR,ambientBL);
  copy(3,ambientTL,ambientTR);
  copy(3,diffuseBL,diffuseBR);
  copy(3,diffuseTR,diffuseBL);
  copy(3,diffuseTL,diffuseTR);
  copy(3,specularBL,specularBR);
  copy(3,specularTR,specularBL);
  copy(3,specularTL,specularTR);
  copy(3,emissionBL,emissionBR);
  copy(3,emissionTR,emissionBL);
  copy(3,emissionTL,emissionTR);
  copy(1,shininessBL,shininessBR);
  copy(1,shininessTR,shininessBL);
  copy(1,shininessTL,shininessTR);

  shadeBR = shadeBL;
  shadeBL = shadeTR;
  shadeTR = shadeTL;

  glutPostRedisplay();
}

void init()
{
   glClearColor(0.5, 0.5, 0.5, 0.0);
   glEnable(GL_DEPTH_TEST);

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
   glLightfv(GL_LIGHT0, GL_POSITION, position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);

}

void update(GLfloat* array, int index, GLfloat change)
{
  if (array==shininessTL ||  array==shininessTR ||  array==shininessBL ||  array==shininessBR)
    {
      array[0]+= change;
      if (array[index] < 0.0) array[index] = 0.0;
      return;
    }
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
 if (parameter == SPHERE_SHININESS) change = SHININESS_INCREMENT; else change = COLOR_INCREMENT;
 if (direction == DOWN) change = (-1) * change;
 switch (parameter)
   {
    case LIGHT_AMBIENT : update(ambient,color,change);break;
    case LIGHT_DIFFUSE : update(diffuse,color,change);break;
    case LIGHT_SPECULAR : update(specular,color,change);break;
    case SPHERE_AMBIENT : switch(sphere)
                           {
			      case TOP_LEFT : update(ambientTL,color,change); break;
  			      case TOP_RIGHT : update(ambientTR,color,change); break;
			      case BOTTOM_LEFT : update(ambientBL,color,change); break;
  			      case BOTTOM_RIGHT : update(ambientBR,color,change); break;
			   }
                          break;
    case SPHERE_DIFFUSE : switch(sphere)
                           {
			      case TOP_LEFT : update(diffuseTL,color,change); break;
  			      case TOP_RIGHT : update(diffuseTR,color,change); break;
			      case BOTTOM_LEFT : update(diffuseBL,color,change); break;
  			      case BOTTOM_RIGHT : update(diffuseBR,color,change); break;
			   }
                          break;
    case SPHERE_SPECULAR : switch(sphere)
                           {
			      case TOP_LEFT : update(specularTL,color,change); break;
  			      case TOP_RIGHT : update(specularTR,color,change); break;
			      case BOTTOM_LEFT : update(specularBL,color,change); break;
  			      case BOTTOM_RIGHT : update(specularBR,color,change); break;
			   }
                          break;
    case SPHERE_EMISSION : switch(sphere)
                           {
			      case TOP_LEFT : update(emissionTL,color,change); break;
  			      case TOP_RIGHT : update(emissionTR,color,change); break;
			      case BOTTOM_LEFT : update(emissionBL,color,change); break;
  			      case BOTTOM_RIGHT : update(emissionBR,color,change); break;
			   }
                          break;
    case SPHERE_SHININESS : switch(sphere)
                           {
			      case TOP_LEFT : update(shininessTL,0,change); break;
  			      case TOP_RIGHT : update(shininessTR,0,change); break;
			      case BOTTOM_LEFT : update(shininessBL,0,change); break;
  			      case BOTTOM_RIGHT : update(shininessBR,0,change); break;
			   }
                          break;
    case SPHERE_SHADING : switch(sphere)
                           {
			      case TOP_LEFT : shadeTL = (direction==UP) ? GL_SMOOTH : GL_FLAT; break;
			      case TOP_RIGHT : shadeTR = (direction==UP) ? GL_SMOOTH : GL_FLAT; break;
			      case BOTTOM_LEFT : shadeBL = (direction==UP) ? GL_SMOOTH : GL_FLAT; break;
			      case BOTTOM_RIGHT : shadeBR = (direction==UP) ? GL_SMOOTH : GL_FLAT; break;
			   }
                          break;
   }
 glutPostRedisplay();
}

void display()
{
   init();

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   glTranslatef (-3.0, 3.0, 0.0);
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambientTL);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseTL);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specularTL);
   glMaterialfv(GL_FRONT, GL_SHININESS, shininessTL);
   glMaterialfv(GL_FRONT, GL_EMISSION, emissionTL);
   glShadeModel(shadeTL);
   glutSolidSphere(2.5, SPHERE_SLICES, SPHERE_STACKS);
   glPopMatrix();

   glPushMatrix();
   glTranslatef (3.0, 3.0, 0.0);
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambientTR);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseTR);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specularTR);
   glMaterialfv(GL_FRONT, GL_SHININESS, shininessTR);
   glMaterialfv(GL_FRONT, GL_EMISSION, emissionTR);
   glShadeModel(shadeTR);
   glutSolidSphere(2.5, SPHERE_SLICES, SPHERE_STACKS);
   glPopMatrix();

   glPushMatrix();
   glTranslatef (-3.0, -3.0, 0.0);
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambientBL);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseBL);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specularBL);
   glMaterialfv(GL_FRONT, GL_SHININESS, shininessBL);
   glMaterialfv(GL_FRONT, GL_EMISSION, emissionBL);
   glShadeModel(shadeBL);
   glutSolidSphere(2.5, SPHERE_SLICES, SPHERE_STACKS);
   glPopMatrix();

   glPushMatrix();
   glTranslatef (3.0, -3.0, 0.0);
   glMaterialfv(GL_FRONT, GL_AMBIENT, ambientBR);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseBR);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specularBR);
   glMaterialfv(GL_FRONT, GL_SHININESS, shininessBR);
   glMaterialfv(GL_FRONT, GL_EMISSION, emissionBR);
   glShadeModel(shadeBR);
   glutSolidSphere(2.5, SPHERE_SLICES, SPHERE_STACKS);
   glPopMatrix();

   glutSwapBuffers();
   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, w, h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= (h * 2))
      glOrtho (-6.0, 6.0, -3.0*((GLfloat)h*2)/(GLfloat)w,
         3.0*((GLfloat)h*2)/(GLfloat)w, -10.0, 10.0);
   else
      glOrtho (-6.0*(GLfloat)w/((GLfloat)h*2),
         6.0*(GLfloat)w/((GLfloat)h*2), -3.0, 3.0, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
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
    case 9 : parameter = SPHERE_SHADING; break;
  }
}


void sphereSubMenu( int item )
{
  switch ( item )
  {
    case 1 : sphere = TOP_LEFT; break;
    case 2 : sphere = TOP_RIGHT; break;
    case 3 : sphere = BOTTOM_LEFT; break;
    case 4 : sphere = BOTTOM_RIGHT; break;
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


void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 1 : shift(); break;
 case 2 : std::exit( 0 );
 }
}

void setMenus( )
{
  int parameterSubMenuCode;
  int sphereSubMenuCode;
  int colorSubMenuCode;

  parameterSubMenuCode = glutCreateMenu( parameterSubMenu );
  glutAddMenuEntry( "Light Ambient",        1 );
  glutAddMenuEntry( "Light Diffuse",        2 );
  glutAddMenuEntry( "Light Specular",       3 );
  glutAddMenuEntry( "Sphere Ambient",       4 );
  glutAddMenuEntry( "Sphere Diffuse",       5 );
  glutAddMenuEntry( "Sphere Specular",      6 );
  glutAddMenuEntry( "Sphere Shininess",     7 );
  glutAddMenuEntry( "Sphere Emission",      8 );
  glutAddMenuEntry( "Sphere Shading",      9 );

  sphereSubMenuCode = glutCreateMenu( sphereSubMenu );
  glutAddMenuEntry( "Top Left",        1 );
  glutAddMenuEntry( "Top Right",       2 );
  glutAddMenuEntry( "Bottom Left",     3 );
  glutAddMenuEntry( "Bottom Right",    4 );

  colorSubMenuCode = glutCreateMenu( colorSubMenu );
  glutAddMenuEntry( "Red",        1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",       3 );
  glutAddMenuEntry( "All",        4 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Parameter  ...",  parameterSubMenuCode );
  glutAddSubMenu( "Sphere  ...",  sphereSubMenuCode );
  glutAddSubMenu( "Color ...", colorSubMenuCode );
  glutAddMenuEntry( "Shift",  1 );
  glutAddMenuEntry( "Exit",  2 );
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
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (600, 600);
   glutCreateWindow("Material Properties Demo");
   glutReshapeFunc(reshape);
   glutDisplayFunc(display);
   glutMouseFunc(mouse);

   setMenus();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glutMainLoop();
   return 0;
}

