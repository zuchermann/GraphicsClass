/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*                      Blending Demo                     */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>

#define STEP 0.1
#define HUE_STEP 6.0
#define UNDEFINED - 1.0

using namespace std;

//Enumerated type and global variable for talking about
//direction of changes in camera position, and aim
//and the direction of time steps and animation.
typedef enum { DOWN, UP } directionType;
directionType direction;

//Enumerated type and global variable for talking about operations.
typedef enum { RED, GREEN, BLUE, HUE, SATURATION, VALUE, ALPHA } operationType;
operationType operation;

//Enumerated type and global variable for talking about sides.
typedef enum { LEFT, RIGHT } sideType;
sideType side;

GLdouble redLeft,greenLeft,blueLeft,alphaLeft;
GLdouble redRight,greenRight,blueRight,alphaRight;

GLdouble hueLeft,saturationLeft,valueLeft;
GLdouble hueRight,saturationRight,valueRight;

//Flag to control what gets drawn first.
static int leftFirst = GL_TRUE;

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
   case  HUE          : if (side==LEFT) hueLeft += change;
                        else if (side==RIGHT) hueRight += change; break;
   case  SATURATION       :  if (side==LEFT) saturationLeft += change;
                        else if (side==RIGHT) saturationRight += change; break;
   case  VALUE        :  if (side==LEFT) valueLeft += change;
                        else if (side==RIGHT) valueRight += change; break;
   case  RED          : if (side==LEFT) redLeft += change;
                        else if (side==RIGHT) redRight += change; break;
   case  GREEN       :  if (side==LEFT) greenLeft += change;
                        else if (side==RIGHT) greenRight += change; break;
   case  BLUE        :  if (side==LEFT) blueLeft += change;
                        else if (side==RIGHT) blueRight += change; break;
   case  ALPHA        :  if (side==LEFT) alphaLeft += change;
                        else if (side==RIGHT) alphaRight += change; break;
 }

  if (redLeft > 1.0) redLeft = 1.0;
  if (greenLeft > 1.0) greenLeft = 1.0;
  if (blueLeft > 1.0) blueLeft = 1.0;
  if (alphaLeft > 1.0) alphaLeft = 1.0;
  if (redLeft < 0.0) redLeft = 0.0;
  if (greenLeft < 0.0) greenLeft = 0.0;
  if (blueLeft < 0.0) blueLeft = 0.0;
  if (alphaLeft < 0.0) alphaLeft = 0.0;
  if (redRight > 1.0) redRight = 1.0;
  if (greenRight > 1.0) greenRight = 1.0;
  if (blueRight > 1.0) blueRight = 1.0;
  if (alphaRight > 1.0) alphaRight = 1.0;
  if (redRight < 0.0) redRight = 0.0;
  if (greenRight < 0.0) greenRight = 0.0;
  if (blueRight < 0.0) blueRight = 0.0;
  if (alphaRight < 0.0) alphaRight = 0.0;


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

  if (operation == RED || operation == GREEN || operation == BLUE)
    {
     if (side == LEFT)
        rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
     else if (side == RIGHT)
        rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);
    }
  else if (operation == HUE || operation == SATURATION || operation == VALUE)
    {
     if (side == LEFT)
	 hsvToRGB(hueLeft,saturationLeft,valueLeft,&redLeft,&greenLeft,&blueLeft);
     else if (side == RIGHT)
        hsvToRGB(hueRight,saturationRight,valueRight,&redRight,&greenRight,&blueRight);
    }
  glutPostRedisplay();
}

void homeState()
{
  side = LEFT;
  operation = RED;
  direction = UP;
  redLeft = greenLeft = blueLeft = alphaLeft = 0.5;
  rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
  redRight = greenRight = blueRight = alphaRight = 0.5;
  rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);
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
    case 0 : operation = ALPHA;  break;
    case 1 : operation = RED;  break;
    case 2 : operation = GREEN; break;
    case 3 : operation = BLUE; break;
    case 4 : operation = HUE;  break;
    case 5 : operation = SATURATION; break;
    case 6 : operation = VALUE; break;
  }
}
void sideSubMenu( int item )
{
  switch ( item )
  {
    case 1 : side = LEFT; break;
    case 2 : side = RIGHT; break;
  }
}

void orderSubMenu( int item )
{
  switch ( item )
  {
    case 1 : leftFirst = GL_TRUE; break;
    case 2 : leftFirst = GL_FALSE; break;
  }
  glutPostRedisplay();
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
  int sideSubMenuCode, operationSubMenuCode, orderSubMenuCode;

  sideSubMenuCode = glutCreateMenu( sideSubMenu );
  glutAddMenuEntry( "Left",      1 );
  glutAddMenuEntry( "Right",      2 );

  operationSubMenuCode = glutCreateMenu( operationSubMenu );
  glutAddMenuEntry( "Alpha",        0 );
  glutAddMenuEntry( "Red",        1 );
  glutAddMenuEntry( "Green",      2 );
  glutAddMenuEntry( "Blue",      3 );
  glutAddMenuEntry( "Hue",        4 );
  glutAddMenuEntry( "Saturation",      5 );
  glutAddMenuEntry( "Value",      6 );

  orderSubMenuCode = glutCreateMenu( orderSubMenu );
  glutAddMenuEntry( "Left First",      1 );
  glutAddMenuEntry( "Right First",      2 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Select Side ...",  sideSubMenuCode );
  glutAddSubMenu( "Select Operation  ...",  operationSubMenuCode );
  glutAddSubMenu( "Select Order  ...",  orderSubMenuCode );
  glutAddMenuEntry( "Home Position",  0 );
  glutAddMenuEntry( "Exit",  1 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}

static void init()
{
   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glShadeModel (GL_FLAT);
   glClearColor (0.0, 0.0, 0.0, 0.0);
}

static void drawLeftTriangle()
{

   glBegin (GL_TRIANGLES);
      glColor4f (redLeft,greenLeft,blueLeft,alphaLeft);
      glVertex3f(0.1, 0.9, 0.0);
      glVertex3f(0.1, 0.1, 0.0);
      glVertex3f(0.7, 0.5, 0.0);
   glEnd();
}

static void drawRightTriangle()
{

   glBegin (GL_TRIANGLES);
      glColor4f (redRight,greenRight,blueRight,alphaRight);
      glVertex3f(0.9, 0.9, 0.0);
      glVertex3f(0.3, 0.5, 0.0);
      glVertex3f(0.9, 0.1, 0.0);
   glEnd();
}

void display()
{
   glClear(GL_COLOR_BUFFER_BIT);

   if (leftFirst) {
      drawLeftTriangle();
      drawRightTriangle();
   }
   else {
      drawRightTriangle();
      drawLeftTriangle();
   }

   glFlush();
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   if (w <= h)
      gluOrtho2D (0.0, 1.0, 0.0, 1.0*(GLfloat)h/(GLfloat)w);
   else
      gluOrtho2D (0.0, 1.0*(GLfloat)w/(GLfloat)h, 0.0, 1.0);
}

void keyboard(unsigned char key, int, int)
{
   switch (key) {
      case 't':
      case 'T':
         leftFirst = !leftFirst;
         glutPostRedisplay();
         break;
      case 27:  /*  Escape key  */
         exit(0);
         break;
      default:
         break;
   }
}

int main(int argc, char** argv)
{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
   glutInitWindowSize (400,400);
   glutCreateWindow ("Blending Demo");
   init();
   glutDisplayFunc(display);
   glutMouseFunc(mouse);
   glutReshapeFunc(reshape);
   glutKeyboardFunc (keyboard);
   setMenus();
   homeState();
   glutMainLoop();
   return 0;
}
