#include <iostream>
#include <algorithm>
#include <GL/glut.h>
#include "scene.h"
#include "graphics.h"


int TransformNode::count = 0;
TransformStack tStack = TransformStack();
vector<pair<int, TransformNode*>> TransformNode::allNodes;

TransformNode::TransformNode(TransformNode* p)
{
	parent = p;
	shapeNode = nullptr;
	transform = new Matrix();
	identifier = count;
	pair<int, TransformNode*> newPair = { identifier, this };
	allNodes.push_back(newPair);
	cout << "adding shape with id " << identifier << endl;
	count++;
}

TransformNode::TransformNode(TransformNode* p, ShapeNode* s, Matrix* t)
{
	parent = p;
	shapeNode = s;
	transform = t;
	identifier = count;
	pair<int, TransformNode*> newPair = { identifier, this };
	allNodes.push_back(newPair);
	cout << "adding shape with id " << identifier << endl;
	count++;
}


TransformNode::~TransformNode()
{
	delete shapeNode;
	delete transform;
	for (int i = 0; i < children.size(); i++) {
		delete children.at(i);
	}
}

void TransformNode::translate(double deltaX, double deltaY)
{
	Matrix mult = *Matrix::translation(deltaX, deltaY);
	transform = mult.multiply(transform);
}

void TransformNode::rotate(double theta)
{
	Matrix mult = *Matrix::rotation(theta);
	transform = mult.multiply(transform);
}

void TransformNode::shear(double shearXY, double shearYX)
{
	Matrix mult = *Matrix::shearing(shearXY, shearYX);
	transform = mult.multiply(transform);
}

void TransformNode::scale(double scaleX, double scaleY)
{
	Matrix mult = *Matrix::scaling(scaleX, scaleY);
	transform = mult.multiply(transform);
}

void TransformNode::draw(bool displayHelpers) const
{
	glPushName(identifier);
	tStack.push(transform);
	for (int i = 0; i < children.size(); i++) {
		(*children[i]).draw(displayHelpers);
	}
	if (shapeNode != nullptr) {
		(*shapeNode).draw();
	}
	glPopName();
	tStack.pop();
}

TransformNode* TransformNode::getParent() const
{
	return parent;
}

void TransformNode::setParent(TransformNode* p)
{
	parent = p;
}

void TransformNode::changeParent(TransformNode* newParent)
{
	TransformNode* oldParent = getParent();

	TransformNode* oldParentParent = getParent();
	Matrix* cwt = new Matrix();
	while (oldParentParent != nullptr) {
		cwt = new Matrix(*cwt->multiply(oldParentParent->getTransform()));
		oldParentParent = oldParentParent->getParent();
	}
	TransformNode* newParentParent = newParent;
	Matrix* cwti = new Matrix();
	while (newParentParent != nullptr) {
		cwti = new Matrix(*((cwti->multiply(newParentParent->getTransform()))->getInverse()));
		newParentParent = newParentParent->getParent();
	}
	Matrix* newTransform = new Matrix(*(cwti->multiply((new Matrix(*(transform->multiply(cwt)))))));
	transform = newTransform;
	oldParent->removeChild(this);
	newParent->addChild(this);
}

void TransformNode::groupObjects(set<TransformNode*>& groupMembers)
{

}

Matrix* TransformNode::getTransform() const
{
	return transform;
}

ShapeNode* TransformNode::getShapeNode() const
{
	return shapeNode;
}

TransformNode* TransformNode::clone() const
{
	return NULL;
}

void TransformNode::addChild(TransformNode* child)
{
	children.push_back(child);
}

void TransformNode::removeChild(TransformNode* child)
{
	vector<TransformNode*> newChildren;
	for (int i = 0; i < children.size(); i++) {
		if (children.at(i) != child) {
			newChildren.push_back(children.at(i));
		}
	}
	children = newChildren;
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
	for (int i = 0; i < children.size(); i++) {
		children.at(i)->select();
	}
	if (shapeNode) {
		shapeNode->select();
	}
}

void TransformNode::deSelect()
{
	for (int i = 0; i < children.size(); i++) {
		children.at(i)->deSelect();
	}
	if (shapeNode) {
		shapeNode->deselect();
	}
}

TransformNode* TransformNode::nodeLookup(int identifier)
{
	for (int i = 0; i < allNodes.size(); i++) {
		if (allNodes.at(i).first == identifier) {
			cout << "selected shape with id " << identifier << endl;
			return allNodes.at(i).second;
		}
	}
	return NULL;
}


ShapeNode::ShapeNode(colorType c)
{
	color = c;
	isSelected = false;
}

void ShapeNode::select() {
	isSelected = true;
}

void ShapeNode::deselect() {
	isSelected = false;
}

void ShapeNode::setTransformNode(TransformNode* tn)
{
	ShapeNode::transform = tn;
}

TransformNode* ShapeNode::getTransformNode()
{
	return transform;
}

Line::Line(double xx0, double yy0, double xx1, double yy1, colorType c)
	: ShapeNode(c)
{
	x0 = xx0;
	y0 = yy0;
	x1 = xx1;
	y1 = yy1;
	color = c;
}


ShapeNode* Line::clone() const
{
	return new Line(x0, y0, x1, y1, color);
}

void Line::draw() const
{
	Matrix mat = *tStack.top();
	Vector* v1 = new Vector(x0, y0);
	Vector* v2 = new Vector(x1, y1);
	Vector* v1r = mat.multiply(v1);
	Vector* v2r = mat.multiply(v2);
	if (isSelected) {
		setColor(WHITE);
	}
	else {
		setColor(color);
	}
	drawLine((*v1r)[0], (*v1r)[1], (*v2r)[0], (*v2r)[1]);
}

void Line::draw(Matrix* topStack) const
{
}

Rectangle::Rectangle(double xx0, double yy0, double xx1, double yy1, colorType c)
	: ShapeNode(c)
{
	x0 = xx0;
	y0 = yy0;
	x1 = xx1;
	y1 = yy1;
	color = c;
}


ShapeNode* Rectangle::clone()  const
{
	return new Rectangle(x0, y0, x1, y1, color);
}

void Rectangle::draw() const
{
	Matrix mat = *tStack.top();
	Vector* v1 = new Vector(x0, y0);
	Vector* v2 = new Vector(x0, y1);
	Vector* v3 = new Vector(x1, y0);
	Vector* v4 = new Vector(x1, y1);
	Vector* v1r = mat.multiply(v1);
	Vector* v2r = mat.multiply(v2);
	Vector* v3r = mat.multiply(v3);
	Vector* v4r = mat.multiply(v4);
	if (isSelected) {
		setColor(WHITE);
	}
	else {
		setColor(color);
	}
	drawRectangle4v((*v1r), (*v2r), (*v4r), (*v3r));
}

void Rectangle::draw(Matrix* topStack) const
{

}

Circle::Circle(double ccX, double ccY, double r, colorType c)
	: ShapeNode(c)
{
	color = c;
	xC = ccX;
	yC = ccY;
	radius = r;
	isSelected = false;
}

ShapeNode* Circle::clone() const
{
	return new Circle(xC, yC, radius, color);
}

void Circle::draw() const
{
	if (isSelected) {
		setColor(WHITE);
	}
	else {
		setColor(color);
	}
	drawCircle(xC, yC, radius, tStack.top());
}

void Circle::draw(Matrix* topStack) const
{

}


Polygon::Polygon(list<Vector*> vs, colorType c) : ShapeNode(c)
{
	vertices = list<Vector*>();
	Vector vecCopy = Vector();
	Vector* newVec = nullptr;
	for (std::list<Vector*>::const_iterator it = vs.cbegin(); it != vs.cend(); ++it) {
		Vector vecCopy = **it;
		newVec = new Vector(vecCopy);
		vertices.push_back(newVec);
	}
	color = c;
	isSelected = false;
}

Polygon::~Polygon()
{
	delete &vertices;
}

ShapeNode* Polygon::clone() const
{
	list<Vector*> newVertices;
	for (std::list<Vector*>::const_iterator it = vertices.cbegin(); it != vertices.cend(); ++it){
		Vector newVertex = **it;
		newVertices.push_back(&newVertex);
	}
	return new Polygon(newVertices, color);
}

void Polygon::draw() const
{
	if (isSelected) {
		setColor(WHITE);
	}
	else {
		setColor(color);
	}

	Matrix mat = *tStack.top();
	list<Vector*> newVertices;
	for (std::list<Vector*>::const_iterator it = vertices.cbegin(); it != vertices.cend(); ++it) {
		Vector newVertex = **it;
		Vector* resultVertex = mat.multiply(&newVertex);
		newVertices.push_back(resultVertex);
	}
	drawPolygon(newVertices, true);
}

void Polygon::draw(Matrix* topStack) const
{

}