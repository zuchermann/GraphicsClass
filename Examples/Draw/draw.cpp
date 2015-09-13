/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics         Tom Ellman */
/*--------------------------------------------------------*/
/*  Simple Draw Program                                   */
/*--------------------------------------------------------*/


////////////////////////////////////////////////////////////////////////
// The simple drawing program allows the user to draw lines, rectangles,
// circles and polygons. The user can control the color of the figures to
// be drawn. He/she can also control whether the interiors of rectangles,
// circles and polygons are empty or filled.
//
// The user runs the program by typing "draw" at the Unix shell command
// line.  A window appears on the screen. When the mouse is over the
// window, the middle button brings up a menu. The menu items are self
// explanatory. The use of the other mouse buttons requires explanation:
//
// Drawing a line:
//  - Press the left button down to mark the starting point of the line.
//  - Release the left button to mark the ending point of the line.
//
// Drawing a rectangle:
//  - Press the left button down to mark one corner of the rectangle.
//  - Release the left button to mark the opposite corner of the rectangle.
//
// Drawing a circle:
//  - Press the left button down to mark one endpoint of a diameter.
//  - Release the left button to mark the other endpoint of the diameter.
//
// Drawing a polygon:
//  - Press and release the left button to mark the location of the first
//    vertex.
//  - Then press and release the left button to mark the location of the
//    second vertex.
//  - Continue marking locations of vertices in the same manner.
//  - Press and release the right button to mark the location of the last
//    vertex, and to close the polygon.
////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <cstdlib>

// Defining default values for window size, shape and location.
#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50

using namespace std;

// Enumerated type for keeping track of the type of figure to be drawn.
typedef enum
{
  LINE, RECTANGLE, CIRCLE, POLYGON
}
figureType;

// Enumerated type for keeping track of whether to fill the figure interior.
typedef enum
{
  EMPTY, FILLED
}
interiorType;

// Enumerated type for keeping track of the color of the figure.
typedef enum
{
  BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW
}
colorType;



// Structure type for holding polygon vertices.
struct pointNodeType
{
  float x;
  float y; struct pointNodeType * next;
};



typedef struct pointNodeType pointNode;

// Variables for keeping track of the type and properties of figure to be drawn.
figureType figure, band;
bool bandOn = false;
interiorType interior;
colorType color;

// Keeping track of last two points at which the left button was clicked down.
int xAnchor, yAnchor, xStretch, yStretch;

// Variable for use in rubberbanding.
bool rubberBanding;

// Variables for keeping track of the screen window dimensions.
int windowHeight, windowWidth;

// Variables for storing polygon while it is being drawn.
pointNode * vertices = NULL; // Linked list of polygon vertices.
int xFirst, yFirst; // Cordinates of first vertex.
int size = 0; // Number of vertices so far.


void clearPicture()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//glutSwapBuffers();
	glFlush();
}

colorType intToColor( int item )
// Function for converting integers to colors.
{
  switch ( item )
  {
    case 1:
      return BLUE;
    case 2:
      return GREEN;
    case 3:
      return CYAN;
    case 4:
      return RED;
    case 5:
      return MAGENTA;
    case 6:
      return YELLOW;
    case 7:
      return WHITE;
    default:
      return BLACK;
  }
}

void setColor( colorType color )
// Function for setting the current color.
{
  switch ( color )
  {
    case BLACK:  glColor3f( 0.0, 0.0, 0.0 );   break;
    case BLUE:   glColor3f( 0.0, 0.0, 1.0 );   break;
    case CYAN:   glColor3f( 0.0, 1.0, 1.0 );   break;
    case GREEN:  glColor3f( 0.0, 1.0, 0.0 );   break;
    case MAGENTA:glColor3f( 1.0, 0.0, 1.0 );   break;
    case RED:    glColor3f( 1.0, 0.0, 0.0 );   break;
    case WHITE:  glColor3f( 1.0, 1.0, 1.0 );   break;
    case YELLOW: glColor3f( 1.0, 1.0, 0.0 );   break;
  }
}

void drawLine(int xOld, int yOld, int xNew, int yNew )
// Draw a line from (xOld,yOld) to (xNew,yNew).
{
  glBegin( GL_LINES );
  glVertex2i( xOld, yOld );
  glVertex2i( xNew, yNew );
  glEnd();
  //glutSwapBuffers();
  glFlush();
}

void drawRectangle(int xOld, int yOld, int xNew, int yNew, interiorType interior )
// Draw a rectangle from (xOld,yOld) to (xNew,yNew).
{
  switch ( interior )
  {
    case EMPTY:   glBegin( GL_LINE_LOOP ); break;
    case FILLED:  glBegin( GL_POLYGON ); break;
  }
  glVertex2i( xOld, yOld );
  glVertex2i( xNew, yOld );
  glVertex2i( xNew, yNew );
  glVertex2i( xOld, yNew );
  glEnd();
  //glutSwapBuffers();
  glFlush();
}

void plotPoints( int xc, int yc, int x, int y )
// Auxiliary procedure called by 'Circle'.
{
  glVertex2i( xc + x, yc + y );
  glVertex2i( xc - x, yc + y );
  glVertex2i( xc + x, yc - y );
  glVertex2i( xc - x, yc - y );
  glVertex2i( xc + y, yc + x );
  glVertex2i( xc - y, yc + x );
  glVertex2i( xc + y, yc - x );
  glVertex2i( xc - y, yc - x );
}



void Circle(int xOld, int yOld, int xNew, int yNew )
// Draw a circle with diameter from (xOld,yOld) to (xNew,yNew).
{
  int xc, yc, r, p, x, y;
  float l;
  xc = ( int )floor( ( xOld + xNew ) / 2.0 );
  yc = ( int )floor( ( yOld + yNew ) / 2.0 );
  l = ( xNew - xOld ) * ( xNew - xOld ) + ( yNew - yOld ) * ( yNew - yOld );
  r = ( int )floor( sqrt( l ) / 2.0 );
  x = 0;
  y = r;
  plotPoints( xc, yc, x, y );
  p = 1 - r;
  for ( x = 1; x <= y; x++ )
  {
    if ( p < 0 )
      p += 2 * x + 1;
    else
    {
      y--;
      p += 2 * ( x - y ) + 1;
    }
    plotPoints( xc, yc, x, y );
  }
}

void drawCircle(int xOld, int yOld, int xNew, int yNew, interiorType interior )
// Draw a circle with diameter from (xOld,yOld) to (xNew,yNew).
{
  switch ( interior )
  {
    case EMPTY:  glBegin( GL_POINTS );  break;
    case FILLED: glBegin( GL_LINES );  break;
  }
  Circle(xOld, yOld, xNew, yNew );
  glEnd();
  //glutSwapBuffers();
  glFlush();
}

void drawPolygon( pointNode * vertices, int size)
// Function to draw a polygon given a list of vertices.
{
  glBegin( GL_POLYGON );
  for (int i = 0; i < size; i++ )
  {
    float x = vertices->x;
    float y = vertices->y;
    glVertex2f( x, y );
    vertices = vertices->next;
  }
  glEnd();
  //glutSwapBuffers();
  glFlush();
}

void freePolygon( pointNode * vertices )
// Function to free the space used by a polygon's vertices.
{
  pointNode * current, * temp;
  current = vertices;
  while ( current != NULL )
  {
    temp = current->next;
    delete current;
    current = temp;
  }
}

void drawRubberBand(figureType band, int xA, int yA, int xS, int yS)
{
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    setColor(WHITE);
    if ( band == LINE )
    {
      glBegin( GL_LINES );
      glVertex2i( xA, yA );
      glVertex2i( xS, yS );
      glEnd();
    }
  else if ( band == RECTANGLE )
    {
      glBegin( GL_LINE_LOOP );
      glVertex2i( xA, yA);
      glVertex2i( xS, yA );
      glVertex2i( xS, yS );
      glVertex2i( xA, yS );
      glEnd();
    }
  else if ( band == CIRCLE )
    {
      glBegin( GL_POINTS );
      Circle(xA, yA, xS, yS );
      glEnd();
    }
  setColor(color);
  glDisable(GL_COLOR_LOGIC_OP);
  glFlush();
}

void rubberBand( int x, int y )
// Callback for processing mouse motion.
{

  if ( rubberBanding )
  {
    if (bandOn) drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    y = windowHeight - y;
    xStretch = x;
    yStretch = y;
    drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    bandOn = true;
//    glutSwapBuffers();
    glFlush();
  }
}

  void reshape( int w, int h )
  // Callback for processing reshape events.
  {
    windowWidth = w;
    windowHeight = h;
    glViewport( 0, 0, ( GLsizei )w, ( GLsizei )h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, ( GLdouble )w, 0.0, ( GLdouble )h );
  }


  void escExit( GLubyte key, int, int )
  // Callback for processing keyboard events.
  {
    if ( key == 27 /* ESC */ ) std::exit( 0 );
  }


  void mainMenu( int item )
  // Callback for processing main menu.
  {
    switch ( item )
    {
      case 1: clearPicture(); break;
      case 2: std::exit( 0 );
    }
  }


  void figureSubMenu( int item )
  // Callback for processing figure submenu.
  {
    switch ( item )
    {
      case 1:
        figure = LINE; band = LINE;
      break;
      case 2:
        figure = RECTANGLE; band = RECTANGLE;
      break;
      case 3:
        figure = CIRCLE; band = CIRCLE;
      break;
      case 4:
        figure = POLYGON; band = LINE;
      break;
    }
  }


  void interiorSubMenu( int item )
  // Callback for processing interior submenu.
  {
    switch ( item )
    {
      case 1:  interior = EMPTY; break;
      case 2:  interior = FILLED; break;
    }
  }


  void colorSubMenu( int item )
  // Callback for processing color submenu.
  {
    color = intToColor( item );
    setColor( color );
  }


  void setMenus()
  // Function for creating menus.
  {
    int figuresubmenu, interiorsubmenu, colorsubmenu;

    figuresubmenu = glutCreateMenu( figureSubMenu );
    glutAddMenuEntry( "Line", 1 );
    glutAddMenuEntry( "Rectangle", 2 );
    glutAddMenuEntry( "Circle", 3 );
    glutAddMenuEntry( "Polygon", 4 );

    interiorsubmenu = glutCreateMenu( interiorSubMenu );
    glutAddMenuEntry( "Empty", 1 );
    glutAddMenuEntry( "Filled", 2 );

    colorsubmenu = glutCreateMenu( colorSubMenu );
    glutAddMenuEntry( "Blue", 1 );
    glutAddMenuEntry( "Green", 2 );
    glutAddMenuEntry( "Cyan", 3 );
    glutAddMenuEntry( "Red", 4 );
    glutAddMenuEntry( "Magenta", 5 );
    glutAddMenuEntry( "Yellow", 6 );
    glutAddMenuEntry( "White", 7 );

    glutCreateMenu( mainMenu );
    glutAddSubMenu( "Figure ...", figuresubmenu );
    glutAddSubMenu( "Interior ...", interiorsubmenu );
    glutAddSubMenu( "Color ...", colorsubmenu );
    glutAddMenuEntry( "Clear", 1 );
    glutAddMenuEntry( "Exit", 2 );
    glutAttachMenu( GLUT_MIDDLE_BUTTON );
  }


void addPolygonPoint( int xNew, int yNew )
// Function to add a point to the polygon the user is currently drawing.
{
  if ( size > 0 ) drawLine(xAnchor,yAnchor,xNew,yNew);
  else
    {
      xFirst = xNew; yFirst = yNew;
    }
    pointNode * newPoint = new pointNode;
    newPoint->x = xNew;
    newPoint->y = yNew;
    newPoint->next = vertices;
    vertices = newPoint;
    size++;
}

  void processLeftDown( int x, int y )
  // Function for processing mouse left botton down events.
  {
    int xNew = x;
    int yNew = windowHeight - y;
    if (bandOn) drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    if ( figure == POLYGON )
        {
          addPolygonPoint( xNew, yNew );
          if (size > 1) drawLine(xAnchor,yAnchor,xNew,yNew);
        }
    xAnchor = xNew;
    yAnchor = yNew;
    xStretch = xNew;
    yStretch = yNew;
    drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    bandOn = true;
    rubberBanding = true;
  }



  void processLeftUp( int x, int y )
  // Function for processing mouse left botton up events.
  {
    int xNew, yNew;
    if ( figure != POLYGON )
    {
      if (bandOn)
         {
           drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
           bandOn = false;
         }
      xNew = x;
      yNew = windowHeight - y;
      if (rubberBanding)
      {
         switch ( figure )
            {
              case LINE: drawLine(xAnchor, yAnchor, xNew, yNew); break;
              case RECTANGLE: drawRectangle(xAnchor, yAnchor, xNew, yNew, interior); break;
              case CIRCLE: drawCircle(xAnchor, yAnchor, xNew, yNew, interior); break;
              case POLYGON: break;
            }
         glFlush();
      }
      rubberBanding = false;
    }
  }


  void processRightDown( int x, int y )
  // Function for processing mouse right botton down events.
  {
    int xNew, yNew;
    if ( figure == POLYGON )
    {
      xNew = x;
      yNew = windowHeight - y;
      addPolygonPoint( xNew, yNew );
      xAnchor = xNew;
      yAnchor = yNew;
      drawLine(xAnchor,yAnchor,xFirst,yFirst );
      if (interior==FILLED) drawPolygon( vertices, size );
      freePolygon( vertices );
      size = 0;
      vertices = NULL;
      if (bandOn)
         {
           drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
           bandOn = false;
         }
      rubberBanding = false;
    }
  }

  void mouse( int button, int state, int x, int y )
  // Function for processing mouse events.
  {
    if ( button == GLUT_LEFT_BUTTON )
      switch ( state )
      {
        case GLUT_DOWN:
          processLeftDown( x, y );
        break;
        case GLUT_UP:
          processLeftUp( x, y );
        break;
      }
    else if ( button == GLUT_RIGHT_BUTTON )
      switch ( state )
      {
        case GLUT_DOWN:
          processRightDown( x, y );
        break;
      }
  }


  int main( int argc, char * argv[] )
  {
    // Mask floating point exceptions.
    _control87( MCW_EM, MCW_EM );

    // Initialize glut with command line parameters.
    glutInit( & argc, argv );

    // Choose RGB display mode for normal screen window.
    glutInitDisplayMode( GLUT_RGB );

    // Set initial window size, position, and title.
    glutInitWindowSize( INITIAL_WIN_W, INITIAL_WIN_H );
    glutInitWindowPosition( INITIAL_WIN_X, INITIAL_WIN_Y );
    windowWidth = INITIAL_WIN_W;
    windowHeight = INITIAL_WIN_H;
    glutCreateWindow( "Simple Drawing Program" );

    // You don't (yet) want to know what this does.
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, ( double )INITIAL_WIN_W, 0.0, ( double )INITIAL_WIN_H ), glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.375, 0.375, 0.0 );

    // Set the color for clearing the normal screen window.
    glClearColor( 0.0, 0.0, 0.0, 0.0 );

    // Set the callbacks for the normal screen window.
    glutDisplayFunc( clearPicture );
    glutMouseFunc( mouse );
    glutReshapeFunc( reshape );
    glutKeyboardFunc( escExit );
    glutMotionFunc( rubberBand );
    glutPassiveMotionFunc( rubberBand );

    // Set the initial state of the drawing system.
    figure = LINE;
    interior = EMPTY;
    color = YELLOW;
    setColor( YELLOW );
    rubberBanding = false;

    // Set up the menus.
    setMenus();

    glutMainLoop();
    return 0;
  }


