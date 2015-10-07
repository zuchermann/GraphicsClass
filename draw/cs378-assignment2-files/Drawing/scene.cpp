#include <iostream>
#include <algorithm>
#include <GL/glut.h>
#include "scene.h"




TransformNode::TransformNode(TransformNode* p)
{

}

TransformNode::TransformNode(TransformNode* p, ShapeNode* s, Matrix* t)
{

}


TransformNode::~TransformNode()
{

}


void TransformNode::translate(double deltaX, double deltaY)
{

}

void TransformNode::rotate(double theta)
{

}

void TransformNode::shear(double shearXY, double shearYX)
{

}

void TransformNode::scale(double scaleX, double scaleY)
{

}

void TransformNode::draw(bool displayHelpers) const
{

}

TransformNode* TransformNode::getParent() const 
{ 
   return NULL;
}

void TransformNode::setParent(TransformNode* p) 
{ 

}

void TransformNode::changeParent(TransformNode* newParent)
{

}

void TransformNode::groupObjects(set<TransformNode*>& groupMembers)
{
 
}

Matrix* TransformNode::getTransform() const
{
   return NULL;
}

TransformNode* TransformNode::clone() const
{
   return NULL;
}

void TransformNode::addChild(TransformNode* child)
{

}

void TransformNode::removeChild(TransformNode* child)
{

}

TransformNode* TransformNode::firstChild() const
{
   return NULL;
}

TransformNode* TransformNode::lastChild() const
{
	return NULL;
}

TransformNode* TransformNode::nextChild(TransformNode* child) const
{
   return NULL;
}

TransformNode* TransformNode::previousChild(TransformNode* child) const
{
   return NULL;
}


void TransformNode::select() 
{

}

void TransformNode::deSelect() 
{

}

TransformNode* TransformNode::nodeLookup(int identifier)
{
   return NULL;
}


ShapeNode::ShapeNode(colorType c) 
{
}

void ShapeNode::setTransformNode(TransformNode* tn)
{

}

TransformNode* ShapeNode::getTransformNode()
{
	return NULL;
}

Line::Line(double xx0, double yy0, double xx1, double yy1, colorType c)
	: ShapeNode(c)
{
}


ShapeNode* Line::clone() const
{
   return NULL;
}

void Line::draw() const
{

}



Rectangle::Rectangle(double xx0, double yy0, double xx1, double yy1, colorType c)
	: ShapeNode(c)
{
}


ShapeNode* Rectangle::clone()  const
{
   return NULL;
}

void Rectangle::draw() const
{

}

Circle::Circle(double ccX, double ccY, double r, colorType c) 
	: ShapeNode(c)

{}

ShapeNode* Circle::clone() const
{
   return NULL;
}

void Circle::draw() const
{

}


Polygon::Polygon(const list<Vector*>& vs, colorType c) 
	: ShapeNode(c)
{

}

Polygon::~Polygon()
{

}

ShapeNode* Polygon::clone() const
{
   return NULL;
}

void Polygon::draw() const
{

}

