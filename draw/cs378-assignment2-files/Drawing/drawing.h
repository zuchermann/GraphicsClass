#ifndef DRAWING_H
#define DRAWING_H

#include <GL/glut.h>
#include <cfloat>
#include <cmath>
#include "drawing.h"
#include "matrix.h"
#include "scene.h"
#include "graphics.h"

using namespace std;

extern int xCenter;
extern int yCenter;

// Enumerated types to keep track of the operation the user has selected,
// the type of figure the user is drawing, its filled/unfilled status and its color.
typedef enum { DRAW, SCALE, SHEAR, ROTATE, TRANSLATE, SELECT, NONE} operationType;
typedef enum { LINE, RECTANGLE, CIRCLE, POLYGON } figureType;

void setOperation(operationType op);

void drawRubberBand(figureType band,
                    double xAnchor, double yAnchor,
                    double xStretch, double yStretch);

void rubberBand( int x, int y );

void blank( );

void clearScene();

void drawGuides();

void displayScene();

void keyboardAsciiFunction( GLubyte key, int, int);

void keyboardNonAsciiFunction( int key, int, int);

void lastSelectedUp();

void lastSelectedDown();

void lastSelectedRight();

void lastSelectedLeft();

void deSelect();

void mainMenu( int item );

void figureSubMenu( int item );

void snapSubMenu( int item );

void colorSubMenu( int item );

void setMenus( );

void reshape(int w, int h);

void installUnderRoot(ShapeNode* newShape);

void makeLine(double xAnchor, double yAnchor,
              double xNew, double yNew, colorType color);

void makeRectangle(double xAnchor, double yAnchor,
                   double xNew, double yNew, colorType color);

void makeCircle(double xAnchor, double yAnchor,
                double xNew, double yNew, colorType color);

void addPolygonPoint(double x, double y);

void removePolygonPoint();

void clearPolygonPoints();

void makePolygon();

void processBeginTransform(double x, double y);

void processEndTransform(double x, double y);

void processScaling(double x, double y);

void processShearing(double x, double y);

void processRotation(double x, double y);

void processTranslation(double x, double y);

void processLeftDownDraw(double x, double y);

TransformNode* getUserSelection(int x, int y);

void processLeftDownSelect(double x, double y);

void processLeftDownDelete(double x, double y);

void processLeftDownMove(double x, double y);

void processLeftDownCopy(double x, double y);

void processLeftDown(double x, double y);

void processLeftUpDraw(double x, double y);

void processLeftUp(double x, double y);

void processRightDownDraw(double x, double y);

void processRightDownMove(double x, double y);

void processRightDownCopy(double x, double y);

void processRightDown(double x, double y);

void mouse( int button, int state, int x, int y );

void groupSelectedObjects();

void parentSelectedObjects();

void copySelectedObjects();

void deleteSelectedObjects();

bool onlySiblingsSelected();

bool noAncestorDescendantSelections();

bool noParentAncestorSelections();


#endif





