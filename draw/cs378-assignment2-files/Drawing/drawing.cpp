/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics       Tom Ellman   */
/*--------------------------------------------------------*/
/*  drawing.cpp : DRAWING PROGRAM                         */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cfloat>
#include <cmath>
#include <list>
#include <set>
#include <iostream>
#include "drawing.h"
#include "matrix.h"
#include "scene.h"
#include "graphics.h"

using namespace std;

// Initial size and location of window.
#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50

// Mouse click resolution when in snap mode. 
#define SNAP_SCALE 10

// Offset in world coordinates of a new copy of an object.
#define COPY_OFF_X 10
#define COPY_OFF_Y 10
#define EPSILON 0.1

// Size of buffer for recording selection hits. 
#define SELECT_BUFFER_SIZE 512
// Mouse click resolution for selecting objects. 
#define PICK_RANGE 10

// Variable to keep track of current drawing color.
colorType color = YELLOW;

// Variable to keep track of all of the figures drawn so far.
TransformNode* sceneRoot = NULL;

// Variable to record the user's selections of figures.
set<TransformNode*> selections;
TransformNode* lastSelected = NULL;

// Variable for storing polygon while being drawn.
list<Vector*> vertices;

// Variable keeping track of the kind of operation to be performed.
operationType operation;

// Variables keeping track of the kind of figure and rubberbands to be drawn.
figureType figure = LINE;
figureType band = LINE;

// Variables keeping track of the start and end points of the rubber band.
double xAnchor, yAnchor, xStretch, yStretch;

// Variables for use in rubberbanding.
bool rubberBanding = false, bandOn = false;

// Variables keeping track of the screen window dimensions.
int windowHeight, windowWidth;
int xCenter, yCenter, minCenter;

// Flag to indicate whether guides should be drawn.
bool guides = false;

// Flat to indicate whether to display helper nodes.
bool displayHelpers = false;

// Flag to indicate whether mouse coodinates should be
// rounded off to the nearest SNAP_SCALE grid point.
bool snap = false;

// Changing the operation in progress.
void setOperation(operationType newOp )
{
  if (newOp == TRANSLATE || newOp == ROTATE || newOp == SHEAR || newOp == SCALE)
    guides = true;
  else if (newOp == DRAW)
    { guides = false; deSelect(); }
  else if (newOp == NONE)
    { guides = false; }
  operation = newOp;
}


// Function for drawing a rubber band in xor mode based
// on kind of figure being drawn. 
void drawRubberBand(figureType band,
                    double xAnchor, double yAnchor,
                    double xStretch, double yStretch)
{
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_XOR);
    setColor(WHITE);
    switch (band)
    {
      case LINE : drawLine(xAnchor,yAnchor,xStretch,yStretch); break;
      case RECTANGLE: drawRectangle(xAnchor,yAnchor,xStretch,yStretch); break;
      case CIRCLE: drawCircle(xAnchor,yAnchor,xStretch,yStretch); break;
      case POLYGON: {
                      addPolygonPoint(xStretch,yStretch);
                      drawPolygon(vertices,false);
                      removePolygonPoint();
                      break;
                    }
    }
    setColor(color);
    glDisable(GL_COLOR_LOGIC_OP);
    glFlush();
}

// Callback for processing mouse motion.
void rubberBand( int x, int y )
{
  if ( rubberBanding )
  {
    if (bandOn) drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    y = windowHeight - y;
    y = y - yCenter;
    x = x - xCenter;
    if (snap)
       {
        x = ((x+(SNAP_SCALE/2))/SNAP_SCALE)*SNAP_SCALE;
        y = ((y+(SNAP_SCALE/2))/SNAP_SCALE)*SNAP_SCALE;
       }
    xStretch = (double) x;
    yStretch = (double) y;
    drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    bandOn = true;
  }
}

// Function to clear scene. 
void clearScene()
{
   delete sceneRoot;
   sceneRoot = new TransformNode(NULL);
   setOperation(DRAW);
   glutPostRedisplay();
}


// Function to draw guides used in translating, rotating
// shearing and scaling.
void drawGuides()
{
  setColor(GRAY);
  if (operation == TRANSLATE)
  {
    drawLine(0,-yCenter,0,yCenter);
    drawLine(-xCenter,0,xCenter,0);
  }
  else if (operation == ROTATE)
  {
    drawCircle(0,0,minCenter/2.0);
    drawLine(0,-yCenter,0,yCenter);
    drawLine(-xCenter,0,xCenter,0);
  }
  else if (operation == SCALE)
  {
    double xQuarter = xCenter/2.0;
    double yQuarter = yCenter/2.0;
    drawRectangle(-xQuarter,-yQuarter,xQuarter,yQuarter);
    drawLine(0,-yCenter,0,yCenter);
    drawLine(-xCenter,0,xCenter,0);
  }
  else if (operation == SHEAR)
  {
    drawLine(0,-yCenter,0,yCenter);
    drawLine(-xCenter,0,xCenter,0);
    drawLine(-xCenter,-yCenter,xCenter,yCenter);
    drawLine(-xCenter,yCenter,xCenter,-yCenter);
  }
}

// Function to display the scene
void displayScene()
{
  glClear(GL_COLOR_BUFFER_BIT);
  sceneRoot->draw(displayHelpers);
  if (guides) drawGuides();
  glFlush();
}

// Function to test whether selection members are all siblings
// in the scene graph. 
bool onlySiblingsSelected()
{
  if (!lastSelected) return false;
  TransformNode* oldParent = lastSelected->getParent();
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
    if ( (*iter)->getParent() != oldParent )
    {
      cerr << "Operation not valid for selection set." << endl;
      cerr << "Non-siblings are selected." << endl;
      return false;
    }
  return true;
}

// Function to verify that selection set includes no two nodes, one of
// which is an ancestor of the other. 
bool noAncestorDescendantSelections()
{
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
  {
    TransformNode* current = (*iter)->getParent();
    while (current)
    {
       if (selections.find(current)!=selections.end())
       {
         cerr << "Operation not valid for selection set." << endl;
         cerr << "An ancestor of a selected item is also selected." << endl;
         return false;
       }
       current = current->getParent();
    }
  }
  return true;
}

// Function to verify that no ancestor of the last selected node 
// is a member of the selection set. 
bool noParentAncestorSelections()
{
   TransformNode* current = lastSelected->getParent();
   while (current)
   {
     if (selections.find(current)!=selections.end())
     {
       cerr << "Operation not valid for selection set." << endl;
       cerr << "An ancestor of the last selected item is also selected." << endl;
       return false;
     }
     current = current->getParent();
   }
   return true;
}

// Function to process the Group menu command. 
void groupSelectedObjects()
{
  if (!onlySiblingsSelected()) return;
  if (lastSelected == sceneRoot)
  {
    sceneRoot = new TransformNode(NULL);
    sceneRoot->addChild(lastSelected);
    lastSelected->setParent(sceneRoot);
  }
  TransformNode* oldParent = lastSelected->getParent();
  oldParent->groupObjects(selections);
}

// Function to process the Parent menu command. 
void parentSelectedObjects()
{
  if (!noParentAncestorSelections()) return;
  TransformNode* newParent = lastSelected;
  selections.erase(newParent);
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
      (*iter)->changeParent(newParent);
  selections.insert(newParent);
}

// Function to process the Copy menu command. 
void copySelectedObjects()
{
  if (!noAncestorDescendantSelections()) return;
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
  {
    TransformNode* target = *iter;
    if (target == sceneRoot)
    {
      sceneRoot = new TransformNode(NULL);
      sceneRoot->addChild(target);
      target->setParent(sceneRoot);
    }
    TransformNode* parent = target->getParent();
    TransformNode* newThing = target->clone();
    parent->addChild(newThing);
    newThing->setParent(parent);
    target->translate(COPY_OFF_X,COPY_OFF_Y);
  }
  glutPostRedisplay();
}

// Function to process the Delete menu command. 
void deleteSelectedObjects()
{
  if (lastSelected==NULL) return;
  if (!noAncestorDescendantSelections()) return;
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
  {
    TransformNode* target = *iter;
    if (target == sceneRoot)
    {
      sceneRoot = new TransformNode(NULL);
      sceneRoot->addChild(target);
      target->setParent(sceneRoot);
    }
    target->getParent()->removeChild(target);
    delete target;
  }
  selections.clear();
  glutPostRedisplay();
}

// Callback for processing ASCII keyboard events.
void keyboardAsciiFunction( GLubyte key, int, int)
{
  if ( key == 27 /* ESC */ ) std::exit( 0 );
}

// Callback for processing non-ASCII keyboard events. 
// Arrow keys are interpreted as commands to change the last selected
// node, moving up, down, right or left in the scene graph. 
void keyboardNonAsciiFunction( int key, int, int)
{
  if (operation == SELECT)
  {
    switch (key)
    {
      case GLUT_KEY_LEFT : lastSelectedLeft(); break;
      case GLUT_KEY_RIGHT : lastSelectedRight(); break;
      case GLUT_KEY_UP: lastSelectedUp(); break;
      case GLUT_KEY_DOWN : lastSelectedDown(); break;
    }
    glutPostRedisplay();
  }
}

// Change last selection to parent.
void lastSelectedUp()
{
  TransformNode* node = lastSelected->getParent();
  if (node) 
  {
	  lastSelected->deSelect();
	  selections.erase(lastSelected);
	  lastSelected = node;
	  lastSelected->select();
	  selections.insert(lastSelected);
  }
}

// Change last selection to (first) child. 
void lastSelectedDown()
{
  TransformNode* node = lastSelected->firstChild();
  if (node) 
  {
	  lastSelected->deSelect();
	  selections.erase(lastSelected);
	  lastSelected = node;
	  lastSelected->select();
	  selections.insert(lastSelected);
  }
}

// Change last selection to next sibling.
void lastSelectedRight()
{
  TransformNode* parent = lastSelected->getParent();
  if (parent) 
  {
	  TransformNode* node = parent->nextChild(lastSelected);
	  if (node) 
	  {
		  lastSelected->deSelect();
		  selections.erase(lastSelected);
		  lastSelected = node;
		  lastSelected->select();
		  selections.insert(lastSelected);
	  }
  }
}

// Change last selection to previous sibling. 
void lastSelectedLeft()
{
  TransformNode* parent = lastSelected->getParent();
  if (parent) 
  {
	  TransformNode* node = parent->previousChild(lastSelected);
	  if (node) 
	  {
		  lastSelected->deSelect();
		  selections.erase(lastSelected);
		  lastSelected = node;
		  lastSelected->select();
		  selections.insert(lastSelected);
	  }
  }
}

// Function to clear the selection set. 
void deSelect()
{
  for (set<TransformNode*>::iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
    (*iter)->deSelect();
  selections.clear();
  lastSelected = NULL;
}

// Callback for processing main menu.
void mainMenu( int item )
{
  switch ( item )
  {
    case 0 : setOperation(DRAW); break;
    case 1 : if (noAncestorDescendantSelections()) setOperation(SCALE);  break;
    case 2 : if (noAncestorDescendantSelections()) setOperation(SHEAR);  break;
    case 3 : if (noAncestorDescendantSelections()) setOperation(ROTATE);  break;
    case 4 : if (noAncestorDescendantSelections()) setOperation(TRANSLATE); break;
    case 5 : groupSelectedObjects();  setOperation(DRAW); break;
    case 6 : parentSelectedObjects(); setOperation(DRAW); break;
    case 7 : copySelectedObjects();  setOperation(DRAW); break;
    case 8 : deleteSelectedObjects();  setOperation(DRAW); break;
    case 9 : clearScene(); break;
    case 10 : std::exit( 0 );
  }
  glutPostRedisplay();
}

// Callback for processing figure submenu.
void figureSubMenu( int item )
{
  setOperation(DRAW);
  switch ( item )
    {
    case 1 : figure = LINE; band = LINE; break;
    case 2 : figure = RECTANGLE; band = RECTANGLE; break;
    case 3 : figure = CIRCLE; band = CIRCLE; break;
    case 4 : figure = POLYGON; band = POLYGON; break;
    }
  deSelect();
  guides = false;
  glutPostRedisplay();
}

// Callback for processing snap submenu.
void snapSubMenu( int item )
{ switch ( item )
  {
    case 1 : snap = false; break;
    case 2 : snap = true; break;
  }
}

// Callback for processing color submenu.
void colorSubMenu( int item )
{
  color = intToColor(item);
  setColor(color);
  deSelect();
}

// Callback for processing select submenu.
void selectSubMenu( int item )
{
  switch ( item )
  {
    case 1 : setOperation(SELECT); break;
    case 2 : deSelect(); setOperation(DRAW); guides = false; break;
  }
  glutPostRedisplay();
}

// Callback for processing display submenu.
void displaySubMenu( int item )
{
  switch ( item )
  {
    case 1 : displayHelpers = true; break;
    case 2 : displayHelpers = false; break;
  }
  glutPostRedisplay();
}

// Routine for creating menus.
void setMenus( )
{
  int figuresubmenu, colorsubmenu, snapsubmenu, selectsubmenu, displaysubmenu;

  figuresubmenu = glutCreateMenu( figureSubMenu );
  glutAddMenuEntry( "Line",      1 );
  glutAddMenuEntry( "Rectangle", 2 );
  glutAddMenuEntry( "Circle",    3 );
  glutAddMenuEntry( "Polygon",    4 );

  colorsubmenu = glutCreateMenu( colorSubMenu );
  glutAddMenuEntry( "Black",   0 );
  glutAddMenuEntry( "Blue",    1 );
  glutAddMenuEntry( "Green",   2 );
  glutAddMenuEntry( "Cyan",    3 );
  glutAddMenuEntry( "Red",     4 );
  glutAddMenuEntry( "Magenta", 5 );
  glutAddMenuEntry( "Yellow",  6 );
  glutAddMenuEntry( "White", 7 );
  glutAddMenuEntry( "Gray",  8 );

  snapsubmenu = glutCreateMenu( snapSubMenu );
  glutAddMenuEntry( "Snap Off",  1 );
  glutAddMenuEntry( "Snap On", 2 );

  selectsubmenu = glutCreateMenu( selectSubMenu );
  glutAddMenuEntry( "Select",  1 );
  glutAddMenuEntry( "DeSelect", 2 );

  displaysubmenu = glutCreateMenu( displaySubMenu );
  glutAddMenuEntry( "Helpers",  1 );
  glutAddMenuEntry( "No Helpers", 2 );


  glutCreateMenu( mainMenu );
  glutAddMenuEntry( "Draw", 0);
  glutAddSubMenu( "Figure ...",   figuresubmenu   );
  glutAddSubMenu( "Color ...",    colorsubmenu    );
  glutAddSubMenu( "Snap ...",   snapsubmenu );
  glutAddSubMenu( "Select ...", selectsubmenu );
  glutAddSubMenu( "Display ...", displaysubmenu );
  glutAddMenuEntry( "Scale",  1);
  glutAddMenuEntry( "Shear", 2);
  glutAddMenuEntry( "Rotate", 3);
  glutAddMenuEntry( "Translate", 4);
  glutAddMenuEntry( "Group", 5 );
  glutAddMenuEntry( "Parent", 6);
  glutAddMenuEntry( "Copy", 7 );
  glutAddMenuEntry( "Delete", 8 );
  glutAddMenuEntry( "Clear", 9);
  glutAddMenuEntry( "Exit",  10);
  glutAttachMenu( GLUT_MIDDLE_BUTTON ); }


// Callback for processing reshape events.
void reshape(int w, int h)
{
  windowWidth = w;
  windowHeight = h;
  xCenter = windowWidth/2.0;
  yCenter = windowHeight/2.0;

  glViewport(0,0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}

// Function to insert a new shape node into the scene graph, 
// directly under the root of the graph. The new shape node 
// is described in terms of window/world coordinates. Since 
// the root may have been  transformed, the new shape node 
// must be attached to a transform node containing the inverse
// of the root's transform. 
void installUnderRoot(ShapeNode* newShape)
{
  Matrix* rT = sceneRoot->getTransform();
  TransformNode* newXfn = new TransformNode(sceneRoot,newShape,
	                                        new Matrix(*rT->getInverse()));
  newShape->setTransformNode(newXfn);
  sceneRoot->addChild(newXfn);
}

// Make a new line shape node and install it under the scene graph root. 
void makeLine(double xAnchor, double yAnchor,
              double xNew, double yNew, colorType color)
{
  ShapeNode* newShape = new Line(xAnchor,yAnchor,xNew,yNew,color);
  installUnderRoot(newShape);
}

// Make a new rectangle shape node and install it under the scene graph root. 
void makeRectangle(double xAnchor, double yAnchor,
                   double xNew, double yNew, colorType color)
{
  ShapeNode* newShape = new Rectangle(xAnchor,yAnchor,xNew,yNew,color);
  installUnderRoot(newShape);
}

// Make a new circle shape node and install it under the scene graph root. 
void makeCircle(double xAnchor, double yAnchor,
                double xNew, double yNew, colorType color)
{
  double deltaX = xNew-xAnchor;
  double deltaY = yNew-yAnchor;
  double radius = sqrt( deltaX*deltaX + deltaY*deltaY );
  ShapeNode* newShape = new Circle(xAnchor,yAnchor,radius,color);
  installUnderRoot(newShape);
}

// Function to add a new vertex to a polygon being drawn. 
void addPolygonPoint(double x, double y)
{
  vertices.push_back(new Vector(x,y));
}

// Function to remove a vertex from a polygon being drawn. 
void removePolygonPoint()
{
  delete vertices.back();
  vertices.pop_back();
}

// Function to clear the container holding polygon vertices
// after the polygon has been fully drawn. 
void clearPolygonPoints()
{
  for (list<Vector*>::iterator iter = vertices.begin();
       iter != vertices.end();
       ++iter)
    delete *iter;
  vertices.clear();
}

// Make a new polygon shape node and install it under the scene graph root. 
void makePolygon()
{
  ShapeNode* newShape = new Polygon(vertices,color);
  installUnderRoot(newShape);
  clearPolygonPoints();
}

// Begin the process of acquiring transform parameters from the user. 
// This function is called when the user has pressed the left mouse button
// to begin dragging the mouse to describe a translation, rotation,
// shearing or scaling. 
void processBeginTransform(double x, double y)
{
  drawGuides();
  xAnchor = x;
  yAnchor = y;
  glutPostRedisplay();
}


// End the process of acquiring transform parameters from the user. 
// This function is called when the user has released the left mouse button
// to end dragging the mouse to describe a translation, rotation,
// shearing or scaling. 
void processEndTransform(double x, double y)
{
  switch (operation)
  {
    case SCALE     : processScaling(x,y); break;
    case SHEAR     : processShearing(x,y); break;
    case ROTATE    : processRotation(x,y); break;
    case TRANSLATE : processTranslation(x,y); break;
    default        : break;
  }
  glutPostRedisplay();
}

// Process a shearing command. Compute the shearing parameters from
// xAnchor, yAnchor, x and y. Apply the shearing to each node in the
// selection set. 
void processShearing(double x, double y)
{
  xStretch = x;
  yStretch = y;
  double baseX = xAnchor;
  if (fabs(baseX) < EPSILON) return;
  double deltaX = xStretch - xAnchor;
  double baseY = yAnchor;
  if (fabs(baseY) < EPSILON) return;
  double deltaY = yStretch - yAnchor;
  double shearXY = deltaX/baseY;
  double shearYX = deltaY/baseX;
  double denomenator = 1.0-shearXY*shearYX;
  if (fabs(denomenator) < EPSILON) return;
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
       {
        (*iter)->shear(shearXY,shearYX);
       }
}

// Process a scaling command. Compute the scaling parameters from
// xAnchor, yAnchor, x and y. Apply the scaling to each node in the
// selection set. 
void processScaling(double x, double y)
{
  double xScale, yScale;
  if (fabs(xAnchor)<SNAP_SCALE)
    xScale = 1.0;
  else xScale = (x)/(xAnchor);
  if (fabs(yAnchor)<SNAP_SCALE)
    yScale = 1.0;
  else yScale = (y)/(yAnchor);
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
       {
        (*iter)->scale(xScale,yScale);
       }
}

// Process a rotation command. Compute the rotation parameters from
// xAnchor, yAnchor, x and y. Apply the rotation to each node in the
// selection set. 
void processRotation(double x, double y)
{
  double ax = xAnchor;
  double ay = yAnchor;
  double ar = sqrt( ax*ax + ay*ay );
  ax = ax / ar;
  ay = ay / ar;
  double sx = x;
  double sy = y;
  double sr = sqrt( sx*sx + sy*sy );
  sx = sx / sr;
  sy = sy / sr;
  double theta = acos(ax*sx+ay*sy);
  double sign = ( ax*sy - ay*sx );
  if (sign<0.0) theta = -theta;
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
       {
        (*iter)->rotate(theta);
       }
}

// Process a translation command. Compute the shearing parameters from
// xAnchor, yAnchor, x and y. Apply the translation to each node in the
// selection set. 
void processTranslation(double x, double y)
{
  double deltaX = x - xAnchor;
  double deltaY = y - yAnchor;
  for (set<TransformNode*>::const_iterator iter = selections.begin();
       iter != selections.end();
       ++iter)
    (*iter)->translate(deltaX,deltaY);
}


// Function to handle down click of left button during draw operation.
void processLeftDownDraw(double x, double y)
{
    if (bandOn) drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    if (figure == POLYGON) addPolygonPoint(x,y);
    xAnchor = x;
    yAnchor = y;
    xStretch = xAnchor;
    yStretch = yAnchor;
    drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
    bandOn = true;
    rubberBanding = true;
}

// Function for processing the hits recorded in the selection buffer
// during a selection operation. 
TransformNode* processHits (GLint hits, GLuint buffer[])
{
   GLuint names, *ptr;
   TransformNode* selection = NULL;
   cout << "Number of Hits: " << hits << endl << endl;
   ptr = (GLuint *) buffer;
   for (int i = 1; i <= hits; i++)
   {
	  cout << "Hit Number: " << i << endl;
      names = *ptr; 
	  ptr++;
	  cout << "Number of Names: " << names << endl;
	  cout << "Minimum Depth: " << *ptr << endl; 
	  ptr++;
	  cout << "Maximum Depth: " << *ptr << endl; 
	  ptr++;
      for (int j = 0; j < names; j++)
      { 
	     int identifier = *ptr;
		 selection = TransformNode::nodeLookup(identifier);
		 cout << "Hit Name: " << *ptr << endl;
         ptr++;
      }
	  cout << endl;
   }
   cout << endl;
   return selection;
}


// Function to determine what object the user has selected after 
// he/she has cliked at coordinates (x,y) relative to the center of the 
// window. 
TransformNode* getUserSelection(int x, int y)
{
   x = x + xCenter; 
   y = y + yCenter; 
   GLint viewport[4];
   GLuint selectBuffer[SELECT_BUFFER_SIZE];
   GLint hits;
   glGetIntegerv (GL_VIEWPORT, viewport);
   glSelectBuffer (SELECT_BUFFER_SIZE, selectBuffer);
   glRenderMode(GL_SELECT);
   glInitNames();
   glMatrixMode (GL_PROJECTION);
   glPushMatrix ();
   glLoadIdentity ();
   gluPickMatrix ( x, y, PICK_RANGE, PICK_RANGE, viewport);
   gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
   sceneRoot->draw(false);
   glMatrixMode (GL_PROJECTION);
   glPopMatrix ();
   glFlush();
   hits = glRenderMode(GL_RENDER);   
   return processHits(hits, selectBuffer);
}

// Function to handle down click of left button during select operation. 
// Determine the user's selection, add it to the selection set, and 
// record it in the lastSelected variable. 
void processLeftDownSelect(double x, double y)
{
	lastSelected = getUserSelection(x,y);
	if (lastSelected)
	{
		lastSelected->select();
		selections.insert(lastSelected);
		glutPostRedisplay();
	}
}

// Routine for processing mouse left botton down events.
void processLeftDown(double x, double y)
{
  switch (operation)
  {
    case DRAW      : processLeftDownDraw(x,y); break;
	case SELECT    : processLeftDownSelect(x,y); break;
    case SCALE     : processBeginTransform(x,y); break;
    case SHEAR     : processBeginTransform(x,y); break;
    case ROTATE    : processBeginTransform(x,y); break;
    case TRANSLATE : processBeginTransform(x,y); break;
    default        : break;
  }
}

// Routine for processing mouse left botton up events 
// while in draw mode. 
void processLeftUpDraw(double x, double y)
{
  double xNew, yNew;
  if (figure==POLYGON) return;
  if (bandOn) drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
  bandOn = false;
  rubberBanding = false;
  xNew = x;
  yNew = y;
  switch ( figure )
  {
   case LINE      : makeLine(xAnchor,yAnchor,xNew,yNew,color);
                    break;
   case RECTANGLE : makeRectangle(xAnchor,yAnchor,xNew,yNew,color);
                    break;
   case CIRCLE    : makeCircle(xAnchor,yAnchor,xNew,yNew,color);
                    break;
   case POLYGON   : break;
  }
  glutPostRedisplay();
}

// Routine for processing mouse left button up events.
void processLeftUp(double x, double y)
{
  switch (operation)
  {
    case DRAW      : processLeftUpDraw(x, y); break;
    case SCALE     : processEndTransform(x,y); break;
    case SHEAR     : processEndTransform(x,y); break;
    case ROTATE    : processEndTransform(x,y); break;
    case TRANSLATE : processEndTransform(x,y); break;
    default        : break;
  }
}

// Function to handle down click of right button during draw operation.
// Adds final point to the current polygon, makes polygon figure node
// and redraws the whole polygon.
void processRightDownDraw(double x, double y)
{
   if (figure != POLYGON) return;
   if (bandOn) drawRubberBand(band,xAnchor,yAnchor,xStretch,yStretch);
   bandOn = false;
   rubberBanding = false;
   addPolygonPoint(x,y);
   makePolygon();
   glutPostRedisplay();
}

// Routine for processing mouse right botton down events.
void processRightDown(double x, double y)
{
  switch (operation)
  {
    case DRAW      : processRightDownDraw(x,y); break;
    case SCALE     : processScaling(x,y); break;
    case SHEAR     : processShearing(x,y); break;
    case ROTATE    : processRotation(x,y); break;
    case TRANSLATE : processTranslation(x,y); break;
    default        : break;
  }
}

// Routine for processing mouse events.
void mouse( int button, int state, int x, int y )
{
   double xd, yd;
   y = windowHeight - y;
   y = y - yCenter;
   x = x - xCenter;
   if (snap)
      {
       x = ((x+(SNAP_SCALE/2))/SNAP_SCALE)*SNAP_SCALE;
       y = ((y+(SNAP_SCALE/2))/SNAP_SCALE)*SNAP_SCALE;
      }
   xd = (double) x;
   yd = (double) y;
   if ( button == GLUT_LEFT_BUTTON )
    switch ( state )
    {
      case GLUT_DOWN : processLeftDown(xd,yd); break;
      case GLUT_UP   : processLeftUp(xd,yd); break;
    }
   else if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
           processRightDown(xd,yd);
}


int main(int argc, char *argv[])
{
  // Mask doubleing point exceptions.
   _control87(MCW_EM,MCW_EM);

  // Initialize glut with command line parameters.
  glutInit( &argc, argv );

  // Choose RGB display mode for normal screen window.
  glutInitDisplayMode(GLUT_RGB);

  // Set initial window size, position, and title.
  glutInitWindowSize( INITIAL_WIN_W, INITIAL_WIN_H );
  glutInitWindowPosition( INITIAL_WIN_X, INITIAL_WIN_Y );
  windowWidth = INITIAL_WIN_W;
  windowHeight = INITIAL_WIN_H;
  xCenter = windowWidth/2;
  yCenter = windowHeight/2;
  minCenter = xCenter < yCenter ? xCenter : yCenter;
  glutCreateWindow("Drawing Program");

  // You don't (yet) want to know what this does.
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  gluOrtho2D( 0.0, (double) INITIAL_WIN_W, 0.0, (double) INITIAL_WIN_H ),
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  glTranslatef( xCenter+0.375, yCenter+0.375, 0.0 );

  // Set the color for clearing thescreen window.
  glClearColor( 0.0, 0.0, 0.0, 0.0 );

  // Set the callbacks for the screen window.
  glutDisplayFunc( displayScene );
  glutMouseFunc( mouse );
  glutMotionFunc(rubberBand);
  glutPassiveMotionFunc(rubberBand);
  glutReshapeFunc(reshape);
  glutKeyboardFunc( keyboardAsciiFunction );
  glutSpecialFunc( keyboardNonAsciiFunction );

  // Set the initial state or the drawing system.
  setColor(color);

  // Set up the menus.
  setMenus( );

  // Initialize the scene graph to hold only a root node. 
  sceneRoot = new TransformNode(NULL);

  // Initiate GLUT's main loop. 
  glutMainLoop( );

  return 0;
}


