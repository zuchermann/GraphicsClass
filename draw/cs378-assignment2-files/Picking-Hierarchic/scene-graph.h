#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include <cstdlib>
#include <list>
#include <stack>
#include <GL/glut.h>

using namespace std;

#define PI 3.14159
#define CIRCLE_SLICES 360

typedef enum { BLACK, BLUE, CYAN, GREEN, MAGENTA, RED, WHITE, YELLOW } colorType;

class Location
{
public:
  float x;
  float y;
  Location();
  Location(float,float);
  Location operator+(const Location&);
};

extern stack<Location> locationStack;

Location operator*(float, const Location&);

class PlaceNode;
class ShapeNode;

class PlaceNode 
{
public:
  PlaceNode();
  PlaceNode(ShapeNode*,Location);
  PlaceNode(list<PlaceNode*>,Location);
  virtual void draw();
protected:
  list<PlaceNode*> components;
  Location place;
  ShapeNode* shapeNode;
  unsigned int identifier;
  static int count;
};

class ShapeNode 
{
public:
  virtual void draw()=0;
};

class Circle : public ShapeNode
{
 protected:
  float radius;
  colorType color;
  bool filled;
 public:
  Circle(float, colorType, bool);
  virtual void draw();
};

class Line : public ShapeNode
{
 protected:
  float x;
  float y;
  colorType color;
 public:
  Line(float, float, colorType);
  virtual void draw();
};


class Rectangle : public ShapeNode
{
protected:
  float width;
  float height;
  colorType color;
  bool filled;
public:
  Rectangle(float, float, colorType, bool);
  virtual void draw();
};

class Triangle : public ShapeNode
{
protected:
  float width;
  float height;
  colorType color;
  bool filled;
public:
  Triangle(float, float, colorType, bool);
  virtual void draw();
};



#endif

