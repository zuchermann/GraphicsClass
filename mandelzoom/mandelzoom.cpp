/*--------------------------------------------------------*/
/*  CS-378           Computer Graphics        Zach Kondak */
/*--------------------------------------------------------*/
/*  mandelzoom                                            */
/*--------------------------------------------------------*/

#include <cstdlib>
#include <GL/glut.h>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <cmath> 

using namespace std;

#define INITIAL_X1 -2.0;
#define INITIAL_X2 0.5;
#define INITIAL_Y1 -1.25;
#define INITIAL_Y2 1.25;
#define INITIAL_WIN_W 800
#define INITIAL_WIN_H 800
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50
#define YOU_ARE_IN_BABY -1;

// Variable for use in rubberbanding.
int xAnchor, yAnchor, xStretch, yStretch;
vector<vector<int>> screenGraph;
vector<int> notInFrac;
int newViewX1, newViewX2, newViewY1, newViewY2;
double viewX1, viewX2, viewY1, viewY2;
bool rubberBanding = false;

// Variables for keeping track of the screen window dimensions.
int windowHeight, windowWidth;

double* complexAdd(double a1, double a2, double b1, double b2) {
	double newComplex[2] = { (a1 + a2), (b1 + b2) };
	return newComplex;
}

double* complexMult(double a1, double a2, double b1, double b2) {
	double newComplex[2] = { ((a1 * a2) - (b1 * b2)), ((a1 * b2) + (b1 * a2)) };
	return newComplex;
}


void drawHelp(int x) {
	int y = 0;
	while (y < windowWidth) {
		double sComplexA, sComplexB;
		sComplexA = viewX1 + (x * ((viewX2 - viewX1) / (windowWidth - 1)));
		sComplexB = viewY1 + (y * ((viewY2 - viewY1) / (windowHeight - 1)));
		double nextA, nextB;
		nextA = sComplexA;
		nextB = sComplexB;
		int i = 0;
		screenGraph[x][y] = YOU_ARE_IN_BABY;
		while (i < 1000) {
			double* mult = complexMult(nextA, nextA, nextB, nextB);
			double* newComplex = complexAdd(mult[0], sComplexA, mult[1], sComplexB);
			nextA = newComplex[0];
			nextB = newComplex[1];
			if (((nextA * nextA) + (nextB + nextB)) > 4) {
				for (vector<int>::size_type vertorIndex = 0; vertorIndex != notInFrac.size(); vertorIndex++) {
					if (notInFrac[vertorIndex] == i) break;
					else {
						if (vertorIndex + 1 == notInFrac.size()) notInFrac.push_back(i);
					}
				}
				//cout << notInFrac.size() << endl;
				if (notInFrac.size() == 0) {
					notInFrac.push_back(i);
				}
				screenGraph[x][y] = i;
				break;
			}
			//cout << screenGraph[x][y] << " ";
			i++;
		}
		y++;
	}
}

void setColor(float val) {
	if (val < 0.025){
		glColor3f(0.5,
			0.0,
			val * 20.0);
	}
	else if (val < 0.25) {
		glColor3f(
			0.5 + (val * 2.0),
			0.0,
			0.5);
	}
	else {
		glColor3f(1.0,
			val,
			0.5 + (val * 0.5));
	}
}

void redisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0; x < windowWidth; x++) {
		for (int y = 0; y < windowHeight; y++) {
			glVertex2i(x, y);
			if (screenGraph[x][y] == -1) {
				glColor3f(0.1, 0.1, 0.1);
			}
			else {
				for (vector<int>::size_type vertorIndex = 0; vertorIndex != notInFrac.size(); vertorIndex++) {
					if (screenGraph[x][y] == notInFrac[vertorIndex]) {
						setColor(vertorIndex / (float)notInFrac.size());
						break;
					}
				}
			}
		}
	}
	glEnd();
	glFlush();
}

void recalculateFrac() {
	// Clear the window.
	notInFrac.resize(0);
	screenGraph.resize(windowHeight);
	for (int widthIndex = 0; widthIndex < windowHeight; widthIndex++) {
		screenGraph[widthIndex].resize(windowWidth);
	}
		int x = 0;
		while (x < windowHeight) {
			drawHelp(x);
			//cout << "" << endl;
			x++;
		}
		sort(notInFrac.begin(), notInFrac.end());
		redisplay();
}

void drawRubberBand(int xA, int yA, int xS, int yS)
{
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glBegin(GL_LINES);
	glVertex2i(xA, yA); glVertex2i(xS, yA);
	glVertex2i(xS, yA); glVertex2i(xS, yS);
	glVertex2i(xS, yS); glVertex2i(xA, yS);
	glVertex2i(xA, yS); glVertex2i(xA, yA);
	glEnd();
	glDisable(GL_COLOR_LOGIC_OP);
	glFlush();
}



void processLeftDown(int x, int y)
// Function for processing mouse left botton down events.
{
	if (!rubberBanding)
	{
		int xNew = x;
		int yNew = windowHeight - y;
		xAnchor = xNew;
		yAnchor = yNew;
		xStretch = xNew;
		yStretch = yNew;
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		rubberBanding = true;
	}
}

void adjustView() {
	int newHeight = abs(newViewY2 - newViewY1);
	int newWidth = abs(newViewX2 - newViewX1);
	if (newWidth != newHeight) {
		if (newWidth > newHeight) {
			newViewY2 = newViewY1 + newWidth;
		}
		else {
			newViewX2 = newViewX1 + newHeight;
		}
	}
	double x1ComplexA, y1ComplexB, x2ComplexA, y2ComplexB;
	x1ComplexA = viewX1 + (newViewX1 * ((viewX2 - viewX1) / (windowWidth - 1)));
	y1ComplexB = viewY1 + (newViewY1 * ((viewY2 - viewY1) / (windowHeight - 1)));
	x2ComplexA = viewX1 + (newViewX2 * ((viewX2 - viewX1) / (windowWidth - 1)));
	y2ComplexB = viewY1 + (newViewY2 * ((viewY2 - viewY1) / (windowHeight - 1)));
	viewX1 = x1ComplexA;
	viewY1 = y1ComplexB;
	viewX2 = x2ComplexA;
	viewY2 = y2ComplexB;
}

void zoom(int x1, int y1, int x2, int y2) {
	if (x1 < x2){
		newViewX1 = x1;
		newViewX2 = x2;
	}
	else {
		newViewX1 = x2;
		newViewX2 = x1;
	}
	if (y1 < y2) {
		newViewY1 = y1;
		newViewY2 = y2;
	}
	else {
		newViewY1 = y2;
		newViewY2 = y1;
	}

	adjustView();
	recalculateFrac();
}

void processLeftUp(int x, int y)
// Function for processing mouse left botton up events.
{
	if (rubberBanding)
	{
		int xNew, yNew;
		
		rubberBanding = false;
		xNew = x;
		yNew = windowHeight - y;
		zoom(xAnchor, yAnchor, xNew, yNew);
		glFlush();
	}
}

void mouse(int button, int state, int x, int y) {
	{
		if (button == GLUT_LEFT_BUTTON)
			switch (state)
			{
			case GLUT_DOWN: processLeftDown(x, y); break;
			case GLUT_UP: processLeftUp(x, y); break;
			}
	}
}

void reshape(int w, int h) {
	windowWidth = w;
	windowHeight = h;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}

void escExit(GLubyte key, int, int) {
	if (key == 27 /* ESC */) std::exit(0);
}

void rubberBand(int x, int y) {
	if (rubberBanding)
	{
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		y = windowHeight - y;
		xStretch = x;
		yStretch = y;
		drawRubberBand(xAnchor, yAnchor, xStretch, yStretch);
		glFlush();
	}
}



int main(int argc, char * argv[])
{
	// Mask floating point exceptions.
	_control87(MCW_EM, MCW_EM);

	// Initialize glut with command line parameters.
	glutInit(&argc, argv);

	// Choose RGB display mode for normal screen window.
	glutInitDisplayMode(GLUT_RGB);

	// Set initial window size, position, and title.
	glutInitWindowSize(INITIAL_WIN_W, INITIAL_WIN_H);
	glutInitWindowPosition(INITIAL_WIN_X, INITIAL_WIN_Y);
	windowWidth = INITIAL_WIN_W;
	windowHeight = INITIAL_WIN_H;
	viewX1 = INITIAL_X1;
	viewX2 = INITIAL_X2;
	viewY1 = INITIAL_Y1;
	viewY2 = INITIAL_Y2
	glutCreateWindow("Mandelzoom");

	// You don't (yet) want to know what this does.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)INITIAL_WIN_W, 0.0, (double)INITIAL_WIN_H), glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0.0);

	// Set the color for clearing the normal screen window.
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Set the color for drawing the house.
	glColor3f(0.0, 0.0, 1.0);

	// Set the callbacks for the normal screen window.
	glutDisplayFunc(recalculateFrac);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(escExit);
	glutMotionFunc(rubberBand);
	glutPassiveMotionFunc(rubberBand);

	// Set up the menus.
	/*
	setMenus();
	*/

	glutMainLoop();
}