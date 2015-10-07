#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stack>
#include <list>
#include "matrix.h"

using namespace std;

const double TWO_PI = 6.283185307179586476925286766559;
const int CIRCLE_SECTIONS = 360;

typedef enum {BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW, GRAY} colorType;

colorType intToColor(int item);

void setColor(colorType color);


class TransformStack
{
  public:
   TransformStack();
   void push(Matrix* transform);
   void pop();
   Matrix* top();
  private:
  //Student must implement.
};

void setHighlight(bool highlight);

bool getHighlight();

void gPush(Matrix* transform);

void gPop();

void drawLine(double x0, double y0, double x1, double y1);

void drawLine(Vector* p0, Vector* p1);

void drawRectangle(double x0, double y0, double x1, double y1);

void drawCircle(double x0, double y0, double x1, double y1);

void drawCircle(double xC, double yC, double radius);

void drawPolygon(const list<Vector*>& vertices, bool close = true);

void drawTransformGismo();

#endif
