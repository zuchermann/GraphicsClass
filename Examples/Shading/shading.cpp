/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics       Tom Ellman  */
/*--------------------------------------------------------*/
/*           Interpolated Shading Demo                    */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>

using namespace std;

#define STEP 0.1
#define HUE_STEP 6.0
#define UNDEFINED -1.0


//Enumerated type and global variable for talking about
//direction of changes in camera position, and aim
//and the direction of time steps and animation.
typedef enum { DOWN, UP } directionType;
directionType direction;

//Enumerated type and global variable for talking about operations.
typedef enum { RED, GREEN, BLUE, HUE, SATURATION, VALUE } operationType;
operationType operation;

//Enumerated type and global variable for talking about corners.
typedef enum { LEFT, RIGHT, TOP } cornerType;
cornerType corner;

GLdouble redLeft,greenLeft,blueLeft;
GLdouble redRight,greenRight,blueRight;
GLdouble redTop,greenTop,blueTop;

GLdouble hueLeft,saturationLeft,valueLeft;
GLdouble hueRight,saturationRight,valueRight;
GLdouble hueTop,saturationTop,valueTop;

GLdouble maximum(GLdouble a, GLdouble b, GLdouble c)
{
  if (a > b) if (a > c) return a;
                   else return c;
        else if (b > c) return b;
                   else return c;
}

GLdouble minimum(GLdouble a, GLdouble b, GLdouble c)
{
  if (a < b) if (a < c) return a;
                   else return c;
        else if (b < c) return b;
                   else return c;
}


void rgbToHSV(GLdouble r, GLdouble g, GLdouble b, GLdouble *h, GLdouble *s, GLdouble *v)
{
  GLdouble max = maximum(r,g,b);
  GLdouble min = minimum(r,g,b);
  *v = max;
  *s = (max != 0.0) ? (max-min)/max : 0.0;
  if (*s == 0.0) *h = UNDEFINED;
  else {
         GLdouble delta = max-min;
         if      (r == max) *h =       (g - b) / delta;
         else if (g == max) *h = 2.0 + (b - r) / delta;
         else if (b == max) *h = 4.0 + (r - g) / delta;
	 *h *= 60.0;
	 if (*h < 0.0) *h += 360.0;
       }
}

void hsvToRGB(GLdouble h, GLdouble s, GLdouble v, GLdouble *r, GLdouble *g, GLdouble *b)
{
  if (s == 0.0) *r = *g = *b = v;
  else {
         GLdouble f,p,q,t;
         int i;
         if (h == 360.0) h = 0.0;
         h /= 60.0;
	 i = (int) floor(h);
	 f = h-i;
	 p = v * (1.0 - s);
	 q = v * (1.0 - s*f);
	 t = v * (1.0 - s*(1.0 - f));
         switch (i)
	   {
             case 0 : *r = v; *g = t; *b = p; break;
             case 1 : *r = q; *g = v; *b = p; break;
             case 2 : *r = p; *g = v; *b = t; break;
             case 3 : *r = p; *g = q; *b = v; break;
             case 4 : *r = t; *g = p; *b = v; break;
             case 5 : *r = v; *g = p; *b = q; break;
             default : break;
	   }
       }
}

void operate()
//Process the operation that the user has selected.
{
 GLdouble change;
 if (direction == UP)
    {
      if (operation == HUE) change = HUE_STEP;
      else change = STEP;
    }
 else   {
         if (operation == HUE) change = (-HUE_STEP);
         else change = (-STEP);
        }
 switch (operation)
 {
   case  HUE          : if (corner==LEFT) hueLeft += change;
                        else if (corner==RIGHT) hueRight += change;
                             else hueTop += change; break;
   case  SATURATION       :  if (corner==LEFT) saturationLeft += change;
                        else if (corner==RIGHT) saturationRight += change;
                             else saturationTop += change; break;
   case  VALUE        :  if (corner==LEFT) valueLeft += change;
                        else if (corner==RIGHT) valueRight += change;
                             else valueTop += change; break;
   case  RED          : if (corner==LEFT) redLeft += change;
                        else if (corner==RIGHT) redRight += change;
                             else redTop += change; break;
   case  GREEN       :  if (corner==LEFT) greenLeft += change;
                        else if (corner==RIGHT) greenRight += change;
                             else greenTop += change; break;
   case  BLUE        :  if (corner==LEFT) blueLeft += change;
                        else if (corner==RIGHT) blueRight += change;
                             else blueTop += change; break;
 }

  if (redLeft > 1.0) redLeft = 1.0;
  if (greenLeft > 1.0) greenLeft = 1.0;
  if (blueLeft > 1.0) blueLeft = 1.0;
  if (redLeft < 0.0) redLeft = 0.0;
  if (greenLeft < 0.0) greenLeft = 0.0;
  if (blueLeft < 0.0) blueLeft = 0.0;
  if (redRight > 1.0) redRight = 1.0;
  if (greenRight > 1.0) greenRight = 1.0;
  if (blueRight > 1.0) blueRight = 1.0;
  if (redRight < 0.0) redRight = 0.0;
  if (greenRight < 0.0) greenRight = 0.0;
  if (blueRight < 0.0) blueRight = 0.0;
  if (redTop > 1.0) redTop = 1.0;
  if (greenTop > 1.0) greenTop = 1.0;
  if (blueTop > 1.0) blueTop = 1.0;
  if (redTop < 0.0) redTop = 0.0;
  if (greenTop < 0.0) greenTop = 0.0;
  if (blueTop < 0.0) blueTop = 0.0;

  if (hueLeft > 360.0) hueLeft -= 360.0;
  if (saturationLeft > 1.0) saturationLeft = 1.0;
  if (valueLeft > 1.0) valueLeft = 1.0;
  if (hueLeft < 0.0) hueLeft += 360.0;
  if (saturationLeft < 0.0) saturationLeft = 0.0;
  if (valueLeft < 0.0) valueLeft = 0.0;
  if (hueRight > 360.0) hueRight -= 360.0;
  if (saturationRight > 1.0) saturationRight = 1.0;
  if (valueRight > 1.0) valueRight = 1.0;
  if (hueRight < 0.0) hueRight += 360.0;
  if (saturationRight < 0.0) saturationRight = 0.0;
  if (valueRight < 0.0) valueRight = 0.0;
  if (hueTop > 360.0) hueTop -= 360.0;
  if (saturationTop > 1.0) saturationTop = 1.0;
  if (valueTop > 1.0) valueTop = 1.0;
  if (hueTop < 0.0) hueTop += 360.0;
  if (saturationTop < 0.0) saturationTop = 0.0;
  if (valueTop < 0.0) valueTop = 0.0;

  if (operation == RED || operation == GREEN || operation == BLUE)
    {
     if (corner == LEFT)
        rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
     else if (corner == RIGHT)
        rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);
     else if (corner == TOP)
       rgbToHSV(redTop,greenTop,blueTop,&hueTop,&saturationTop,&valueTop);
    }
  else if (operation == HUE || operation == SATURATION || operation == VALUE)
    {
     if (corner == LEFT)
	 hsvToRGB(hueLeft,saturationLeft,valueLeft,&redLeft,&greenLeft,&blueLeft);
     else if (corner == RIGHT)
        hsvToRGB(hueRight,saturationRight,valueRight,&redRight,&greenRight,&blueRight);
     else if (corner == TOP)
       hsvToRGB(hueTop,saturationTop,valueTop,&redTop,&greenTop,&blueTop);
    }
  glutPostRedisplay();
}

void init()
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_SMOOTH);
}

void triangle()
{
   glBegin (GL_TRIANGLES);
   glColor3f (redLeft,greenLeft,blueLeft);
   glVertex2f (5.0, 5.0);
   glColor3f (redRight,greenRight,blueRight);
   glVertex2f (25.0, 5.0);
   glColor3f (redTop,greenTop,blueTop);
   glVertex2f (5.0, 25.0);
   glEnd();
}

void display()
{
   glClear (GL_COLOR_BUFFER_BIT);
   triangle ();
   glFlush ();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   if (w <= h)
      gluOrtho2D (0.0, 30.0, 0.0, 30.0 * (GLfloat) h/(GLfloat) w);
   else
      gluOrtho2D (0.0, 30.0 * (GLfloat) w/(GLfloat) h, 0.0, 30.0);
   glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int)
{
   switch (key) {
      case 27:
         exit(0);
         break;
   }
}
void homeState()
{
  corner = LEFT;
  operation = RED;
  direction = UP;
  redLeft = greenLeft = blueLeft = 0.5;
  rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
  redRight = greenRight = blueRight = 0.5;
  rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);
  redTop = greenTop = blueTop = 0.5;
  rgbToHSV(redTop,greenTop,blueTop,&hueTop,&saturationTop,&valueTop);
  glutPostRedisplay();
}

void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 0 : homeState(); glutPostRedisplay();break;
 case 1 : exit( 0 );
 }
}



void operationSubMenu( int item )
{
  switch ( item )
  {
    case 1 : operation = RED;  break;
    case 2 : operation = GREEN; break;
    case 3 : operation = BLUE; break;
    case 4 : operation = HUE;  break;
    case 5 : operation = SATURATION; break;
    case 6 : operation = VALUE; break;
  }
}
void cornerSubMenu( int item )
{
  switch ( item )
  {
    case 1 : corner = TOP; break;
    case 2 : corner = LEFT; break;
    case 3 : corner = RIGHT; break;
  }
}

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

void setMenus( )
// Routine for creating menus.
{
  int cornerSubMenuCode, operationSubMenuCode;

  cornerSubMenuCode = glutCreateMenu( cornerSubMenu );
  glutAddMenuEntry( "Top",        1 );
  glutAddMenuEntry( "Left",      2 );
  glutAddMenuEntry( "Right",      3 );

  operationSubMenuCode = glutCreateMenu( operationSubMenu );
  glutAddMenuEntry( "Red",        1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",      3 );
  glutAddMenuEntry( "Hue",        4 );
  glutAddMenuEntry( "Saturation",      5 );
  glutAddMenuEntry( "Value",      6 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Select Corner ...",  cornerSubMenuCode );
  glutAddSubMenu( "Select Operation  ...",  operationSubMenuCode );
  glutAddMenuEntry( "Home Position",  0 );
  glutAddMenuEntry( "Exit",  1 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

int main(int argc, char** argv)
{
   // Mask floating point exceptions.
   _control87( MCW_EM, MCW_EM );
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (500, 500);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Shading Demo");
   init ();
   glutDisplayFunc(display);
   glutMouseFunc(mouse);
   glutReshapeFunc(reshape);
   glutKeyboardFunc (keyboard);
   setMenus();
   homeState();
   glutMainLoop();
   return 0;
}
