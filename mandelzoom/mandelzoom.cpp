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

using namespace std;

#define INITIAL_X1 -2.0;
#define INITIAL_X2 0.5;
#define INITIAL_Y1 -1.25;
#define INITIAL_Y2 1.25;
#define INITIAL_WIN_W 400
#define INITIAL_WIN_H 400
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50

// Variable for use in rubberbanding.
int xAnchor, yAnchor, xStretch, yStretch;
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
		while (i < 1000) {
			double* mult = complexMult(nextA, nextA, nextB, nextB);
			double* newComplex = complexAdd(mult[0], sComplexA, mult[1], sComplexB);
			nextA = newComplex[0];
			nextB = newComplex[1];
			if (((nextA * nextA) + (nextB + nextB)) > 4) {
				glBegin(GL_POINTS);
				glVertex2i(x, y);
				glEnd();
				break;
			}
			i++;
		}
		y++;
	}
}

void drawPicture() {
	// Clear the window.
	glClear(GL_COLOR_BUFFER_BIT);
		int x = 0;
		while (x < windowHeight) {
			drawHelp(x);
			x++;
		}
		glFlush();
}



void mouse(int button, int state, int x, int y) {
	drawPicture();
}

void reshape(int w, int h) {

}

void escExit(GLubyte key, int, int) {

}

void rubberBand(int x, int y) {

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
	glColor3f(1.0, 1.0, 1.0);

	// Set the callbacks for the normal screen window.
	glutDisplayFunc(drawPicture);
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