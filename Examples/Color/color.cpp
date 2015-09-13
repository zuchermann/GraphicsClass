/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*                      Color Demo                        */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <iostream>

#define STEP 0.05
#define HUE_STEP 3.0
#define UNDEFINED -1.0

using namespace std;

//Global variables for keeping track of the size and shape
//of the window.
int windowHeight, windowWidth;

//Enumerated type and global variable for keeping track
//of the selected operation.
typedef enum { RED, GREEN, BLUE, SATURATION, VALUE, HUE,
              COMPLEMENT, LEFT_TO_RIGHT, RIGHT_TO_LEFT} operationType;
operationType operation = RED;

//Enumerated type and global variable for talking about
//direction of changes in camera position, and aim
//and the direction of time steps and animation.
typedef enum { DOWN, UP } directionType;
directionType direction = UP;

//Enumerated type and global variable for talking about
//which color box to change.
typedef enum { LEFT, RIGHT } sideType;
sideType side = LEFT;

//Enumerated type for background.
typedef enum { BLACK, WHITE, GRAY } backgroundType;
backgroundType background = WHITE;

//Global variables for keeping track of the state.
GLdouble redLeft;
GLdouble greenLeft;
GLdouble blueLeft;
GLdouble redRight;
GLdouble greenRight;
GLdouble blueRight;
GLdouble hueLeft;
GLdouble saturationLeft;
GLdouble valueLeft;
GLdouble hueRight;
GLdouble saturationRight;
GLdouble valueRight;

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


//Initialize the display window.
void init()
{
   glClearColor (1.0, 1.0, 1.0, 1.0);
   glShadeModel (GL_FLAT);
}

void drawLeft()
{
   glColor3d(redLeft,greenLeft,blueLeft);
   glBegin(GL_POLYGON);
   glVertex2i((int) windowWidth/4,  (int) windowHeight/4);
   glVertex2i((int) windowWidth/4,  (int) 3*windowHeight/4);
   glVertex2i((int) windowWidth/2,(int) 3*windowHeight/4);
   glVertex2i((int) windowWidth/2,(int) windowHeight/4);
   glEnd();
}

void drawRight()
{
   glColor3d(redRight,greenRight,blueRight);
   glBegin(GL_POLYGON);
   glVertex2i((int) windowWidth/2,    (int) windowHeight/4);
   glVertex2i((int) windowWidth/2,    (int) 3*windowHeight/4);
   glVertex2i((int) 3*windowWidth/4,(int) 3*windowHeight/4);
   glVertex2i((int) 3*windowWidth/4,(int) windowHeight/4);
   glEnd();
}

void display()
//Callback for redisplaying the image.
{
  switch (background)
    {
    case BLACK : glClearColor (0.0, 0.0, 0.0, 0.0); break;
    case GRAY  : glClearColor (0.5, 0.5, 0.5, 0.0); break;
    case WHITE : glClearColor (1.0, 1.0, 1.0, 0.0); break;
    }

  glClear( GL_COLOR_BUFFER_BIT  );

  drawLeft();
  drawRight();
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
   gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);

}

void homeState()
//Putting the state varibles back to their original values.
{
 redLeft = 0.0;  greenLeft = 0.0; blueLeft = 0.0;
 rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
 redRight = 0.0;  greenRight = 0.0; blueRight = 0.0;
 rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);

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
   case  RED          :  if (side==LEFT) redLeft += change; else redRight += change; break;
   case  GREEN        :  if (side==LEFT) greenLeft += change; else greenRight += change; break;
   case  BLUE         :  if (side==LEFT) blueLeft += change; else blueRight += change; break;
   case  HUE          :  if (side==LEFT) hueLeft = (hueLeft+change);
                                    else hueRight = (hueRight+change); break;
   case  SATURATION   :  if (side==LEFT) saturationLeft += change; else saturationRight += change; break;
   case  VALUE        :  if (side==LEFT) valueLeft += change; else valueRight += change; break;
   case  COMPLEMENT   :  if (side==LEFT) {redLeft=1.0-redLeft;greenLeft=1.0-greenLeft;blueLeft=1.0-blueLeft;}
                         else {redRight=1.0-redRight;greenRight=1.0-greenRight;blueRight=1.0-blueRight;} break;
   case LEFT_TO_RIGHT : redRight = redLeft; greenRight = greenLeft; blueRight = blueLeft;
                        rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);
                        break;
   case RIGHT_TO_LEFT : redLeft = redRight; greenLeft = greenRight; blueLeft = blueRight;
                        rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
                        break;
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

  if (operation == RED || operation == GREEN || operation == BLUE || operation == COMPLEMENT)
     if (side == LEFT) rgbToHSV(redLeft,greenLeft,blueLeft,&hueLeft,&saturationLeft,&valueLeft);
                 else  rgbToHSV(redRight,greenRight,blueRight,&hueRight,&saturationRight,&valueRight);
  else if (operation == HUE || operation == SATURATION || operation == VALUE)
          if (side == LEFT) hsvToRGB(hueLeft,saturationLeft,valueLeft,&redLeft,&greenLeft,&blueLeft);
                       else  hsvToRGB(hueRight,saturationRight,valueRight,&redRight,&greenRight,&blueRight);
  cout << "Red: " << redLeft << " Green: " << greenLeft << " Blue: " << blueLeft << endl;
  cout << "Hue: " << hueLeft << " Saturation: " << saturationLeft << " Value: " << valueLeft << endl;

  glutPostRedisplay();
}

void mainMenu( int item )
// Callback for processing main menu.
{
 switch ( item )
 {
 case 1 : operation = RED; break;
 case 2 : operation = GREEN; break;
 case 3 : operation = BLUE; break;
 case 5 : operation = SATURATION; break;
 case 6 : operation = VALUE; break;
 case 7 : operation = HUE; break;
 case 8 : operation = COMPLEMENT; break;
 case 9 : homeState(); glutPostRedisplay();break;
 case 11 : operation = LEFT_TO_RIGHT; operate(); glutPostRedisplay();break;
 case 12 : operation = RIGHT_TO_LEFT; operate(); glutPostRedisplay();break;
 case 10 : std::exit( 0 );
 }
}

void sideSubMenu( int item )
// Callback for processing side selection sub menu.
{
  switch ( item )
  {
    case 1 : side = LEFT; break;
    case 2 : side = RIGHT; break;
  }
}

void backgroundSubMenu( int item )
// Callback for processing background selection sub menu.
{
  switch ( item )
  {
    case 1 : background = BLACK; break;
    case 2 : background = GRAY; break;
    case 3 : background = WHITE; break;
  }
  glutPostRedisplay();
}


void setMenus( )
// Routine for creating menus.
{
  int sideSubMenuCode,  backgroundSubMenuCode;
  sideSubMenuCode = glutCreateMenu( sideSubMenu );
  glutAddMenuEntry( "Left",        1 );
  glutAddMenuEntry( "Right",        2 );

  backgroundSubMenuCode = glutCreateMenu( backgroundSubMenu );
  glutAddMenuEntry( "Black",        1 );
  glutAddMenuEntry( "Gray",         2 );
  glutAddMenuEntry( "White",        3 );

  glutCreateMenu( mainMenu );
  glutAddSubMenu( "Side  ...",  sideSubMenuCode );
  glutAddSubMenu( "Background ...", backgroundSubMenuCode );
  glutAddMenuEntry( "Red", 1 );
  glutAddMenuEntry( "Green", 2);
  glutAddMenuEntry( "Blue", 3 );
  glutAddMenuEntry( "Hue", 7 );
  glutAddMenuEntry( "Saturation", 5 );
  glutAddMenuEntry( "Value", 6 );
  glutAddMenuEntry( "Complement", 8 );
  glutAddMenuEntry( "Left To Right", 11 );
  glutAddMenuEntry( "Right To Left", 12 );
  glutAddMenuEntry( "Home State",  9 );
  glutAddMenuEntry( "Exit",  10 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
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




int main(int argc, char** argv)
{
   // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

   glutInit(&argc, argv);
   glutInitDisplayMode( GLUT_RGB );
   glutInitWindowSize (500, 500);
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Color Demo");
   init ();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMouseFunc( mouse );

   setMenus();
   homeState();

   glutMainLoop();
   return 0;
}
