#ifndef FIGURE_H
#define FIGURE_H

#include <list>
#include <map>
#include <set>
#include "matrix.h"
#include "graphics.h"

using namespace std;

class TransformNode;
class ShapeNode;


class TransformNode
{
public:
	TransformNode(TransformNode* parent);
	TransformNode(TransformNode* parent, ShapeNode* shapeNode, Matrix* transform);
	~TransformNode();
	void translate(double deltaX, double deltaY);
	void rotate(double theta);
	void shear(double shearXY, double shearYX);
	void scale(double scaleX, double scaleY);
	virtual void draw(bool displayHelpers) const;
	TransformNode* getParent() const;
	void setParent(TransformNode* parent);
	void changeParent(TransformNode* newParent);
	void groupObjects(set<TransformNode*>& groupMembers);
	Matrix* getTransform() const;
	ShapeNode* getShapeNode() const;
	virtual TransformNode* clone() const;
	void addChild(TransformNode* child);
	void removeChild(TransformNode* child);
	TransformNode* firstChild() const;
	TransformNode* lastChild() const;
	TransformNode* nextChild(TransformNode* child) const;
	TransformNode* previousChild(TransformNode* child) const;
	void select();
	void deSelect();
	static TransformNode* nodeLookup(int identifier);

private:
	ShapeNode* shapeNode;
	Matrix* transform;
	TransformNode* parent;
	std::vector<TransformNode*> children;
	static int count;
	unsigned int identifier;
	static std::vector<std::pair<int, TransformNode*>> allNodes;
};

class ShapeNode
{
public:
	ShapeNode(colorType color);
	virtual void draw() const = 0;
	virtual void draw(Matrix* stackTop) const = 0;
	void setTransformNode(TransformNode* transformNode);
	TransformNode* getTransformNode();
	virtual ShapeNode* clone() const = 0;
	void select();
	void deselect();
private:
	colorType color;
	TransformNode* transform;
protected:
	bool isSelected;
};

class Line : public ShapeNode
{
public:
	Line(double x0, double y0, double x1, double y1, colorType c);
	virtual ShapeNode* clone() const;
	virtual void draw() const;
	virtual void draw(Matrix* stackTop) const;
private:
	double x0, y0;
	double x1, y1;
	colorType color;
};

class Rectangle : public ShapeNode
{
public:
	Rectangle(double x0, double y0, double x1, double y1, colorType c);
	virtual ShapeNode* clone() const;
	virtual void draw() const;
	virtual void draw(Matrix* stackTop) const;
private:
	double x0, y0;
	double x1, y1;
	colorType color;
};


class Circle : public ShapeNode
{
public:
	Circle(double cX, double cY, double radius, colorType c);
	virtual ShapeNode* clone() const;
	virtual void draw() const;
	virtual void draw(Matrix* stackTop) const;
private:
	double xC, yC;
	double radius;
	colorType color;
};

class Polygon : public ShapeNode
{
public:
	Polygon(list<Vector*> vertices, colorType c);
	~Polygon();
	virtual ShapeNode* clone() const;
	virtual void draw() const;
	virtual void draw(Matrix* stackTop) const;
private:
	list<Vector*> vertices;
	colorType color;
};

#endif
