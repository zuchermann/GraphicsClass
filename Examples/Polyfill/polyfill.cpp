/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics      Tom Ellman    */
/*--------------------------------------------------------*/
/*  polyfill.cpp : Polygon Drawing and Filling Program    */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <iostream>

using namespace std;

#define MAX_HEIGHT 1200
#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50
#define SCALE 10
#define DELAY 250

// Definitions of structures used in a linked list that keeps track of
// the figures the user has drawn.
struct pointNodeType
    {
      int x;
      int y;
      struct pointNodeType* next;
    };
typedef struct pointNodeType pointNode;
typedef struct {pointNode* vertices; int size;} polygonData;

typedef enum { BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW } colorType;
colorType color;

// Variables for storing polygon while being drawn.
pointNode* vertices = NULL;
int size = 0;

// Variables keeping track of the start and end points of the rubber band.
int xAnchor, yAnchor, xStretch, yStretch;

// Variables keeping track of the first point on a polygon.
int xStart, yStart;

// Variables for use in rubberbanding.
bool rubberBanding = false , bandOn = false;
bool drawing, filled, closed;


// Variables keeping track of the screen window dimensions.
int windowHeight, windowWidth;

// Scale parameter:
int scale = 1, halfScale = 0;

// Delay parameter.
int delay = 0;


colorType intToColor(int item)
// A function to convert an integer int a color.
{
  switch ( item ) {
    case 1 : return BLUE    ;
    case 2 : return GREEN   ;
    case 3 : return CYAN    ;
    case 4 : return RED     ;
    case 5 : return MAGENTA ;
    case 6 : return YELLOW  ;
    case 7 : return WHITE   ;
   default : return BLACK;
  }
}


void setColor( colorType color )
// A function to set the current drawing color.
{
  switch ( color ) {
    case BLACK   : glColor3f( 0.0, 0.0, 0.0 ); break;
    case BLUE    : glColor3f( 0.0, 0.0, 1.0 ); break;
    case CYAN    : glColor3f( 0.0, 1.0, 1.0 ); break;
    case GREEN   : glColor3f( 0.0, 1.0, 0.0 ); break;
    case MAGENTA : glColor3f( 1.0, 0.0, 1.0 ); break;
    case RED     : glColor3f( 1.0, 0.0, 0.0 ); break;
    case WHITE   : glColor3f( 1.0, 1.0, 1.0 ); break;
    case YELLOW  : glColor3f( 1.0, 1.0, 0.0 ); break; }
}


colorType getPixel( int x, int y )
// A runction for getting the color of a pixel.
{ float RGB[ 3 ];
  int R, G, B;

  glReadPixels( x , y , 1, 1, GL_RGB, GL_FLOAT, RGB );
  R = ( RGB[ 0 ] > 0.9 );
  G = ( RGB[ 1 ] > 0.9 );
  B = ( RGB[ 2 ] > 0.9 );
  return( R ?
            G ?
              B ? WHITE   : YELLOW  :
              B ? MAGENTA : RED     :
            G ?
              B ? CYAN    : GREEN   :
              B ? BLUE    : BLACK     ); }

void line(int oldX, int oldY, int newX, int newY)
{
   glBegin(GL_LINES);
   glVertex2i(oldX*scale+halfScale,oldY*scale+halfScale);
   glVertex2i(newX*scale+halfScale,newY*scale+halfScale);
   glEnd();
}


void drawLine(int oldX, int oldY, int newX, int newY)
{
  line(oldX,oldY,newX,newY);
  glFlush();
}

//  Edge Data-Type Definition.
struct Edge
{
  int xmax;
  int ymax;
  int xmin;
  int ymin;
  float x;
  float invslope;
  struct Edge* next;
};

//  ET & AET Tables.
Edge* ET[MAX_HEIGHT];
Edge* AET;
int aetLength = 0;

void insertEdge(Edge* list, Edge* newEdge)
{
  //  Insert new element into already sorted list.
  Edge* ptr=list;
  // If empty just add.
  if(NULL==ptr->next)	{ ptr->next=newEdge; return; }
  // Else find insertion position.
  while (NULL!=ptr->next)
    {
      // Inserting.
      if(newEdge->x <= ptr->next->x)
	{
	  newEdge->next=ptr->next;
	  ptr->next=newEdge;
	  return;
	}
      //  If at last node then simply append.
      if(NULL==ptr->next->next)
	{
	  ptr->next->next=newEdge;
	  return;
	}
      ptr=ptr->next;
    }
}


void addEdge(Edge* ET[], int x1, int y1, int x2, int y2)
//  Add an edge between (x1,y1) & (x2,y2) to ET.
{
        int ymin;
	if(y1==y2) return;    // Horizontal Edge not Added.
	Edge* newEdge = new Edge;
	if(y1<y2)
	{	newEdge->xmin=x1;
	        newEdge->ymin=y1;
		newEdge->xmax=x2;
		newEdge->ymax=y2;
		newEdge->x = x1;
		newEdge->invslope=(float)(x2-x1)/(float)(y2-y1);
		ymin=y1;
	}
	else
	{	newEdge->xmin=x2;
	        newEdge->ymin=y2;
		newEdge->xmax=x1;
		newEdge->ymax=y1;
		newEdge->x = x2;
		newEdge->invslope=(float)(x1-x2)/(float)(y1-y2);
		ymin=y2;
	}
	newEdge->next=NULL;
	// Inserting in Sorted Order in ET.
	insertEdge(ET[ymin],newEdge);
}


void initializeTables()
//  Add Dummy Headers to AET and ET and then Add Polygon Edges to ET from Input
{
	// Add Dummy Head to AET/
	AET = new Edge;
	AET->next = NULL;
	//  Adding Dummy Headers to ET
	for(int y=0;y<windowHeight;y++)
	{
	  ET[y]= new Edge;
	  ET[y]->next=NULL;
	}
	if (size >= 3) // Must have at least three vertices for a polygon.
	  {
	    pointNode* vertex = vertices;
	    for (int i = 1; i<size; i++)
	      {
		addEdge(ET,vertex->x,vertex->y,vertex->next->x,vertex->next->y);
		vertex=vertex->next;
	      }
	    addEdge(ET,vertex->x,vertex->y,vertices->x,vertices->y);
	  }
}

void wait()
{
  clock_t t=clock();
  do {} while( (float)(clock()-t)/CLOCKS_PER_SEC<(float)delay/1000 );
}

void drawSpan(int x1, int x2, int y)
{
  glBegin(GL_POINTS);
  int yReal = y*scale + halfScale;
  for (int i = x1; i<=x2; i++) glVertex2i(i*scale + halfScale,yReal);
  glEnd();
}


void drawSpans(Edge* AET, int y)
//  Fill in the spans in scan line y with color.
{
 	Edge *p1, *p2;
	int x1,x2;
	// Skip Empty Scan Lines
	if(NULL==AET->next) return;
	p1=AET->next;
	p2=AET->next->next;
	while(p1!=NULL)
	{
		if (p1->x == floor(p1->x))
		  {
		    x1 = (int)(p1->x);
		    if   (p2->x == floor(p2->x))
                         if   ((int)p1->x == (int)p2->x)
                              x2 = (int)(p2->x);
                         else x2 = (int)(p2->x - 1);
		    else x2 = (int) floor(p2->x);
		  }
		else
		  {
		    x1 = (int) floor(p1->x+1);
		    if   (p2->x == floor(p2->x))
                         x2 = (int)(p2->x-1);
		    else x2 = (int) floor(p2->x);
		  }
	  drawSpan(x1,x2,y);
	  if(NULL==p2->next) return;
	  p1=p2->next;
	  p2=p2->next->next;
	}
}

void deleteNextEdge(Edge* ptr)
//  Delete the next edge after ptr.
{
	Edge* temp;

	temp=ptr->next;
	ptr->next=ptr->next->next;
	delete temp;
        aetLength--;
}

void sortAET(Edge* AET)
//  Sort a list of edges.
{
	Edge *temp,*ptr;

	temp=AET->next;
	AET->next=NULL;

	while(NULL!=temp)
	{
		ptr=temp;
		temp=temp->next;
		ptr->next=NULL;
		insertEdge(AET,ptr);
	}
}

void copyEdges(Edge* ET[], int y, Edge* AET)
//  Copy edges from ET to AET, inserting in sorted order.
{
	Edge *temp, *ptr;

	ptr=ET[y]->next;
	while(NULL!=ptr)
	{
		// Creating New Edge
	        temp=new Edge;
		// Copying to Temp
		temp->xmax=ptr->xmax;
		temp->ymax=ptr->ymax;
		temp->xmin=ptr->xmin;
		temp->ymin=ptr->ymin;
		temp->x=ptr->x;
		temp->invslope=ptr->invslope;
		temp->next=NULL;

		// Inserting Temp in AET
		insertEdge(AET,temp);
                aetLength++;
		ptr=ptr->next;
	}
}

void refreshAET(Edge* ET[], int y, Edge* AET)
//  Chuck out old edges and add new edges if necessary.
{
	Edge *ptr, *temp;
	// Checking AET for Deletions, Incrementing x
	ptr=AET;
	while(NULL!=ptr->next)
	{
		if(y==ptr->next->ymax)
		{
			temp=ptr;
			deleteNextEdge(ptr);
		}
		else
		{
			// Incrementing x
			ptr->next->x += ptr->next->invslope;
			temp=ptr->next;
		}
		ptr=temp;
	}
	//  Sorting Edges by New Incremented X Values
	if(NULL!=AET->next)
	{
		sortAET(AET);
	}
	// Checking for Additions From ET
	if(NULL!=ET[y]->next)
	{
		copyEdges(ET,y,AET);
	}
}




//void line( pointNode* p1, pointNode* p2 )
//
///* Draw a line from p1 to p2 (Bresenham's Algorithm) */
//
//{ int x1, y1, x2, y2, dx, dy, x, y, g, twodx, twodxdy, twody, twodydx, p;
//
//  glBegin( GL_POINTS );
//
//  if ( p1->x < p2->x ) {
//    x1 = p1->x;  y1 = p1->y;  x2 = p2->x;  y2 = p2->y; }
//  else if ( p1->x > p2->x ) {
//    x1 = p2->x;  y1 = p2->y;  x2 = p1->x;  y2 = p1->y; }
//  else if ( p1->y < p2->y ) {
//    x1 = p1->x;  y1 = p1->y;  x2 = p2->x;  y2 = p2->y; }
//  else {
//    x1 = p2->x;  y1 = p2->y;  x2 = p1->x;  y2 = p1->y; }
//
//  dx = x2 - x1;
//  dy = y2 - y1;
//
//  if ( dx == 0 )   /* m = +infinity */
//    { for ( y = y1; y <= y2; y++ )
//        glVertex2i( x1 * scale + halfScale, y * scale + halfScale );
//      glEnd( );
//      return; }
//
//  if ( dy == 0 )   /* m = 0         */
//    { for ( x = x1; x <= x2; x++ )
//        glVertex2i( x * scale + halfScale, y1 * scale + halfScale );
//      glEnd( );
//      return; }
//
//  if ( dy == dx )  /* m = +1        */
//    { g = y1 - x1; for ( x = x1; x <= x2; x++ )
//        glVertex2i( x * scale + halfScale, ( g + x ) * scale + halfScale );
//      glEnd( );
//      return; }
//
//  if ( dy == -dx ) /* m = -1        */
//    { g = y1 + x1; for ( x = x1; x <= x2; x++ )
//        glVertex2i( x * scale + halfScale, ( g - x ) * scale + halfScale );
//      glEnd( );
//      return; }
//
//  x = x1;
//  y = y1;
//  glVertex2i( x * scale + halfScale, y * scale + halfScale );
//
//  if ( dy > 0 && dy < dx ) {  /* 0 < m < +1 */
//    twody   = 2 * dy;
//    twodydx = 2 * ( dy - dx );
//    p       = twody - dx;
//    for ( x = x1 + 1; x <= x2; x++ ) {
//      if ( p < 0 ) {
//        p = p + twody; }
//      else {
//        y = y + 1;
//        p = p + twodydx; }
//      glVertex2i( x * scale + halfScale, y * scale + halfScale ); }
//    glEnd( );
//    return; }
//
//  if ( dy < 0 && dy > -dx ) { /* -1 < m < 0 */
//    twody   = 2 * dy;
//    twodydx = 2 * ( dy + dx );
//    p       = -twody - dx;
//    for ( x = x1 + 1; x <= x2; x++ ) {
//      if ( p < 0 ) {
//        p = p - twody; }
//      else {
//        y = y - 1;
//        p = p - twodydx; }
//      glVertex2i( x * scale + halfScale, y * scale + halfScale ); }
//    glEnd( );
//    return; }
//
//  if ( dy > dx ) {            /* m > +1     */
//    twodx   = 2 * dx;
//    twodxdy = 2 * ( dx - dy );
//    p       = twodx - dy;
//    for ( y = y1 + 1; y <= y2; y++ ) {
//      if ( p < 0 ) {
//        p = p + twodx; }
//      else {
//        x = x + 1;
//        p = p + twodxdy; }
//      glVertex2i( x * scale + halfScale, y * scale + halfScale ); }
//    glEnd( );
//    return; }
//
//  if ( dy < -dx ) {           /* m < -1     */
//    twodx   = 2 * dx;
//    twodxdy = 2 * ( dx + dy );
//    p       = -twodx - dy;
//    for ( y = y1 - 1; y >= y2; y-- ) {
//      if ( p > 0 ) {
//        p = p - twodx; }
//      else {
//        x = x + 1;
//        p = p - twodxdy; }
//      glVertex2i( x * scale + halfScale, y * scale + halfScale ); }
//    glEnd( );
//    return; } }




void blank( )
// Callback for clearing the screen window.
{
  glClear( GL_COLOR_BUFFER_BIT );
  glFlush();
}

void drawPolygon()
// Function to draw a polygon given a list of vertices in world coordinates.
{
 if (size<=1) return;
 pointNode* current = vertices;
 while (current->next!=NULL)
   {
     line(current->x, current->y,current->next->x, current->next->y);
     current = current->next;
   }
 if (closed) line(current->x, current->y,vertices->x, vertices->y);
 glFlush();
}

void showAET()
{
  Edge* current = AET->next;
  setColor(RED);
  while (current!=NULL)
    {
      glBegin(GL_LINES);
      glVertex2i(current->xmin*scale+halfScale,current->ymin*scale+halfScale);
      glVertex2i(current->xmax*scale+halfScale,current->ymax*scale+halfScale);
      glEnd();
      current = current->next;
    }
  setColor(color);
  glFlush();
}



void fillPolygon()
//  Callback for filling the polygon.
{
  // Scan Line Loop
  int y;
  for(y=0; y<windowHeight; y++)
    {
      // Remove Dead Edges, Add New Edges, Sort Edges
      refreshAET(ET,y,AET);
      if (aetLength > 0)
      {
        drawPolygon();
        drawSpans(AET,y);
        showAET();
        wait();
      }
    }
  drawPolygon();
  glFlush();
}

void drawRubberBand(int xA, int yA, int xS, int yS)
{
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    drawLine(xA,yA,xS,yS);
    glDisable(GL_COLOR_LOGIC_OP);
    glFlush();
}


void rubberBand( int x, int y )
// Callback for processing mouse motion.
{
  if ( rubberBanding )
  {
    if (bandOn) drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    y = windowHeight - y;
    x = x/scale;
    y = y/scale;
    xStretch = x;
    yStretch = y;
    drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
    bandOn = true;
    glFlush();
  }
}


void escExit( GLubyte key, int, int )
// Callback for processing keyboard events.
{ if ( key == 27 /* ESC */ ) std::exit( 0 ); }


void addPolygonPoint(int x, int y)
// Function to add a point to the polygon the user is currently drawing.
{
  pointNode* newPoint = new pointNode;
  size++;
  newPoint->x = x;
  newPoint->y = y;
  newPoint->next = vertices;
  vertices = newPoint;
  if (size == 1) {xStart = x; yStart = y;}
}


void reshape(int w, int h)
// Callback for processing reshape events.
{
  windowWidth = w;
  windowHeight = h;

  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}

void newPolygon()
{
  blank();
  drawing = true;
  closed = false;
  size = 0;
  filled = false;
  while (vertices!=NULL)
    {
      pointNode* temp = vertices;
      vertices=vertices->next;
      delete temp;
    }
}

void scalePixels()
{
  delay = DELAY - delay;

  scale = SCALE - scale + 1;
  halfScale = scale / 2;

  glPointSize((GLfloat)scale);
  newPolygon();
}

void processLeftDown(int x, int y)
// Function to handle down click of left button during draw operation.
// Adds a point to the current polygon.
{
  if (!drawing) return;
  if (bandOn) drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
  addPolygonPoint(x,y);
  if (size>1) drawLine(vertices->x,vertices->y,vertices->next->x,vertices->next->y);
  xAnchor = x;
  yAnchor = y;
  xStretch = xAnchor;
  yStretch = yAnchor;
  drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
  bandOn = true;
  rubberBanding = true;
}


void processRightDown(int x, int y)
// Function to handle down click of right button during draw operation.
// Adds final point to the current polygon, makes polygon figure node
// and redraws the whole polygon.
{
  if (!drawing) return;
  if (bandOn) drawRubberBand(xAnchor,yAnchor,xStretch,yStretch);
  bandOn = false;
  rubberBanding = false;
  addPolygonPoint(x,y);
  drawLine(xAnchor,yAnchor,x,y);
  drawLine(x,y,xStart,yStart);
  closed = true;
  drawing = false;
  initializeTables();
}


void mouse( int button, int state, int x, int y )
// Routine for processing mouse events.
{
   y = windowHeight - y;
   x = x / scale;
   y = y / scale;
   if      ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
       processLeftDown(x,y);
   else if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
       processRightDown(x,y);
}

void colorSubMenu( int item )
// Callback for processing color submenu.
{
  color = intToColor(item);
  setColor(color);
}


void mainMenu( int item )
// Callback for processing main menu.
{
  switch ( item )
  {
    case 1 : newPolygon(); break;
    case 2 : scalePixels(); break;
    case 3 : filled = true; fillPolygon(); break;
    case 4 : std::exit( 0 );
  }
}

void setMenus( )
// Routine for creating menus.
{
  int colorsubmenu;
  colorsubmenu = glutCreateMenu( colorSubMenu );
  glutAddMenuEntry( "Blue",    1 );
  glutAddMenuEntry( "Green",   2 );
  glutAddMenuEntry( "Cyan",    3 );
  glutAddMenuEntry( "Red",     4 );
  glutAddMenuEntry( "Magenta", 5 );
  glutAddMenuEntry( "Yellow",  6 );
  glutAddMenuEntry( "White",   7 );

  glutCreateMenu( mainMenu );
  glutAddMenuEntry( "New Polygon", 1 );
  glutAddSubMenu( "Set Color ...",    colorsubmenu    );
  glutAddMenuEntry( "Scale Pixels", 2 );
  glutAddMenuEntry( "Fill Polygon", 3 );
  glutAddMenuEntry( "Exit", 4 );
  glutAttachMenu( GLUT_MIDDLE_BUTTON );
}


int main(int argc, char *argv[])
{
  // Mask floating point exceptions.
   _control87(MCW_EM,MCW_EM);

  // Initialize glut with command line parameters.
  glutInit( &argc, argv );

  // Choose RGB display mode for screen window.
  glutInitDisplayMode(GLUT_RGB);

  // Set initial window size, position, and title.
  glutInitWindowSize( INITIAL_WIN_W, INITIAL_WIN_H );
  glutInitWindowPosition( INITIAL_WIN_X, INITIAL_WIN_Y );
  windowWidth = INITIAL_WIN_W;
  windowHeight = INITIAL_WIN_H;
  glutCreateWindow("Polygon Drawing and Filling Program");

  // You don't (yet) want to know what this does.
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluOrtho2D( 0.0, (double) INITIAL_WIN_W, 0.0, (double) INITIAL_WIN_H ),
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  glTranslatef( 0.375, 0.375, 0.0 );

  glPointSize((GLfloat)scale);


  // Set the color for clearing the screen window.
  glClearColor( 0.0, 0.0, 0.0, 0.0 );

  // Set the callbacks for the screen window.
  glutDisplayFunc( blank );
  glutMouseFunc( mouse );
  glutMotionFunc(rubberBand);
  glutPassiveMotionFunc(rubberBand);
  glutReshapeFunc(reshape);
  glutKeyboardFunc( escExit );

  // Set the initial state or the drawing system.
  drawing = false;
  rubberBanding = false;
  color = YELLOW;
  setColor(color);

  // Initialize the ET and AET for no polygon.
  initializeTables();

  // Set up the menus.
  setMenus();

  // Start Glut mail loop.
  glutMainLoop();

}


