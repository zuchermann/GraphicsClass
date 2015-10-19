#include "graphics.h"
#include "drawing.h"

#include <GL/glut.h>

#include <cfloat>
#include <cmath>
#include <iostream>

#define CIRCLE_SLICES = 360;

using namespace std;

const colorType HIGHLIGHT_COLOR = WHITE;

const double TRANSFORM_GISMO_SCALE = 25.0;


bool highlight = false;

bool getHighlight()
{
	return highlight;
}

void setHighlight(bool h)
{
	if (h) setColor(HIGHLIGHT_COLOR);
	highlight = h;
}


colorType intToColor(int item)
{
	switch (item) {
	case 0: return BLACK;
	case 1: return BLUE;
	case 2: return GREEN;
	case 3: return CYAN;
	case 4: return RED;
	case 5: return MAGENTA;
	case 6: return YELLOW;
	case 7: return WHITE;
	case 8: return GRAY;
	default: return BLACK;
	}
}


void setColor(colorType color)
{
	if (!highlight)
		switch (color)
		{
		case BLACK: glColor3f(0.0, 0.0, 0.0); break;
		case BLUE: glColor3f(0.0, 0.0, 1.0); break;
		case GREEN: glColor3f(0.0, 1.0, 0.0); break;
		case CYAN: glColor3f(0.0, 1.0, 1.0); break;
		case RED: glColor3f(1.0, 0.0, 0.0); break;
		case MAGENTA: glColor3f(1.0, 0.0, 1.0); break;
		case YELLOW: glColor3f(1.0, 1.0, 0.0); break;
		case WHITE: glColor3f(1.0, 1.0, 1.0); break;
		case GRAY: glColor3f(0.5, 0.5, 0.5); break;
		}
}



TransformStack transformStack;

TransformStack::TransformStack()
{
	matrices.push_back(new Matrix());
}

void TransformStack::push(Matrix* transform)
{
	Matrix oldBack = *(matrices.back());
	Matrix* newBack = oldBack.multiply(transform);
	matrices.push_back(newBack);
}

void TransformStack::pop()
{
	matrices.pop_back();
}

Matrix* TransformStack::top()
{
	return matrices.back();
}


void gPush(Matrix* transform)
{

}

void gPop()
{

}

void drawLine(double x0, double y0, double x1, double y1)
{
	glBegin(GL_LINES);
	glVertex2d(x0, y0); glVertex2i(x1, y1);
	glEnd();
	glFlush();
}

void drawLine(Vector* p0, Vector* p1)
{

}

void drawRectangle(double x0, double y0, double x1, double y1)
{
	glBegin(GL_LINE_LOOP);
	glVertex2d(x0, y0); glVertex2d(x0, y1); glVertex2d(x1, y1); glVertex2d(x1, y0);
	glEnd();
	glFlush();
}

void drawRectangle4v(Vector v0, Vector v1, Vector v2, Vector v3) {
	glBegin(GL_LINE_LOOP);
	glVertex2d(v0[0], v0[1]); glVertex2d(v1[0], v1[1]); glVertex2d(v2[0], v2[1]); glVertex2d(v3[0], v3[1]);
	glEnd();
	glFlush();
}

void drawCircleDDD(double cX, double cY, double radius){
	GLdouble theta, delta;
	delta = TWO_PI / CIRCLE_SECTIONS;
	glBegin(GL_LINE_LOOP);
	for (theta = 0; theta < (2 * TWO_PI); theta += delta)
		glVertex2d(cX + radius*cos(theta), cY + radius*sin(theta));
	glEnd();
	glFlush();
}

void drawCircle(double x0, double y0, double x1, double y1)
{
	glBegin(GL_LINE_LOOP);
	double radius = sqrt(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0)));
	cout << "radius: " << radius << endl;
	GLdouble theta, delta;
	delta = TWO_PI / CIRCLE_SECTIONS;
	for (theta = 0; theta < TWO_PI; theta += delta) {
		glVertex2d((x0 + radius*cos(theta)), y0 + (radius*sin(theta)));
	}
	glEnd();
	glFlush();
}

void drawCircle(double xC, double yC, double radius, Matrix* stackTop) {
	GLdouble theta, delta;
	delta = TWO_PI / CIRCLE_SECTIONS;
	glBegin(GL_LINE_LOOP);
	for (theta = 0; theta < (2 * TWO_PI); theta += delta) {
		Matrix mat = *stackTop;
		Vector* vec = new Vector(xC + radius*cos(theta), yC + radius*sin(theta));
		Vector* newVec = mat.multiply(vec);
		glVertex2d((*newVec)[0], (*newVec)[1]);
	}
	glEnd();
	glFlush();
}

void drawCircle(double cX, double cY, double radius)
{
	drawCircleDDD(cX, cY, radius);
}

void drawPolygon(const list<Vector*>& vertices, bool close)
{
	glBegin(GL_LINE_LOOP);
	for (std::list<Vector*>::const_iterator it = vertices.cbegin(); it != vertices.cend(); ++it) {
		Vector v1 = **it;
		glVertex2d(v1[0], v1[1]);
	}
	glEnd();
	glFlush();
}

void drawTransformGismo()
{
	double halfSize = TRANSFORM_GISMO_SCALE / 2.0;
	drawRectangle(-halfSize, -halfSize, halfSize, halfSize);
}
