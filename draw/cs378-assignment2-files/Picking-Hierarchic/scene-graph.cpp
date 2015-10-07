#include "scene-graph.h"
#include <stack>
#include <cmath>

using namespace std;

stack<Location> locationStack;

void setColor( colorType color )
// Routine for setting the current color.
{
  switch ( color ) {
    case BLACK   : glColor3f( 0.0, 0.0, 0.0 ); break;
    case BLUE    : glColor3f( 0.0, 0.0, 1.0 ); break;
    case CYAN    : glColor3f( 0.0, 1.0, 1.0 ); break;
    case GREEN   : glColor3f( 0.0, 1.0, 0.0 ); break;
    case MAGENTA : glColor3f( 1.0, 0.0, 1.0 ); break;
    case RED     : glColor3f( 1.0, 0.0, 0.0 ); break;
    case WHITE   : glColor3f( 1.0, 1.0, 1.0 ); break;
    case YELLOW  : glColor3f( 1.0, 1.0, 0.0 ); break; } }


Location::Location() : x(0.0), y(0.0) {};
Location::Location(float cx, float cy) : x(cx), y(cy) { }
Location Location::operator+(const Location& place) {return Location(x+place.x, y+place.y);}

Location operator*(float f, const Location& place) {return Location(f*place.x,f*place.y);}


int PlaceNode::count = 0;

PlaceNode::PlaceNode() : identifier(count), shapeNode(NULL) { count++; }

PlaceNode::PlaceNode(list<PlaceNode*> cs, Location p) 
 : identifier(count), components(cs), place(p), shapeNode(NULL) 
{ count++; }

PlaceNode::PlaceNode(ShapeNode* sn, Location p) 
 : identifier(count), place(p), shapeNode(sn) 
{ count++; }

void PlaceNode::draw()
{
  glPushName(identifier);
  Location newPlace = locationStack.top() + place;
  locationStack.push(newPlace);
  if (shapeNode) shapeNode->draw();
  for (list<PlaceNode*>::iterator iter = components.begin();
	   iter != components.end();
	   ++iter)
      (*iter)->draw();
  locationStack.pop();
  glPopName();
}

void setPolygonFillMode(bool filled)
{
	if (filled) 
	{
	  glPolygonMode(GL_FRONT,GL_FILL);
	  glPolygonMode(GL_BACK,GL_FILL);  
	}
	else
	{
	  glPolygonMode(GL_FRONT,GL_LINE);
	  glPolygonMode(GL_BACK,GL_LINE);  
	}
}


void drawCircle(float x, float y, float radius, colorType color, bool filled)
{
  setPolygonFillMode(filled);
  GLdouble theta, delta;
  setColor(color);
  delta = 2*PI/CIRCLE_SLICES;
  glBegin(GL_POLYGON);
    for (theta = 0; theta < 2*PI; theta += delta)
        glVertex2f(x+radius*cos(theta),y+radius*sin(theta));
  glEnd();
}

Circle::Circle(float r, colorType c, bool f) : radius(r), color(c), filled(f) { }
void Circle::draw() 
{ 
  Location place = locationStack.top();
  drawCircle(place.x,place.y,radius,color,filled); 
}

void drawLine(float xOld, float yOld, float xNew, float yNew, colorType color)
// Draw a line from (xOld,yOld) to (xNew,yNew).
{
  setColor(color);
  glBegin( GL_LINES );
  glVertex2f( xOld, yOld );
  glVertex2f( xNew, yNew );
  glEnd( );
}

Line::Line(float xe, float ye, colorType c) : x(xe), y(ye), color(c) {}
void Line::draw() 
{ 
	Location place = locationStack.top();
	drawLine(place.x,place.y,place.x+x,place.y+y,color); 
}

void drawRectangle(float left, float bottom, float width, float height, 
				   colorType color, bool filled)
{
  setPolygonFillMode(filled);
  setColor(color);
  glBegin(GL_POLYGON);
  glVertex2f(left,bottom);
  glVertex2f(left+width,bottom);
  glVertex2f(left+width,bottom+height);
  glVertex2f(left,bottom+height);
  glEnd();
}

Rectangle::Rectangle(float w, float h, colorType c, bool f) 
 :  width(w), height(h), color(c), filled(f) {}
void Rectangle::draw()
{
  Location place = locationStack.top();
  drawRectangle(place.x,place.y,width,height,color,filled);
}

void drawTriangle(float left, float bottom, float width, float height, 
				   colorType color, bool filled)
{
  setPolygonFillMode(filled);
  setColor(color);
  glBegin(GL_POLYGON);
  glVertex2f(left,bottom);
  glVertex2f(left+width,bottom);
  glVertex2f(left+width/2.0,bottom+height);
  glEnd();
}

Triangle::Triangle(float w, float h, colorType c, bool f) 
 : width(w), height(h), color(c), filled(f)  {}
void Triangle::draw()
{
  Location place = locationStack.top();
  drawTriangle(place.x,place.y,width,height,color,filled);
}


