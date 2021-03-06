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
#define INITIAL_WIN_W 400
#define INITIAL_WIN_H 200
#define INITIAL_WIN_X 150
#define INITIAL_WIN_Y 50
#define YOU_ARE_IN_BABY -1;

// Variable for use in rubberbanding.
int xAnchor, yAnchor, xStretch, yStretch;
vector<vector<int>> screenGraph;
vector<vector<int>> tempGraph;
vector<vector<vector<int>>> graphStack;
vector<vector<double>> coordinateStack;
int popCount = 0;
vector<int> notInFrac;
int newViewX1, newViewX2, newViewY1, newViewY2;
double viewX1, viewX2, viewY1, viewY2;
bool rubberBanding = false;
bool fancyMode = false;
bool needRedraw = false;
bool needRecalc = false;
bool firstOpen = true;

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
	while (y < windowHeight) {
		double sComplexA, sComplexB;
		sComplexA = viewX1 + (x * ((viewX2 - viewX1) / (windowWidth - 1)));
		sComplexB = viewY1 + (y * ((viewY2 - viewY1) / (windowHeight - 1)));
		double nextA, nextB;
		nextA = sComplexA;
		nextB = sComplexB;
		int i = 0;
		screenGraph[x][y] = YOU_ARE_IN_BABY;
		tempGraph[x][y] = YOU_ARE_IN_BABY;
		while (i < 1000) {
			double* mult = complexMult(nextA, nextA, nextB, nextB);
			double* newComplex = complexAdd(mult[0], sComplexA, mult[1], sComplexB);
			nextA = newComplex[0];
			nextB = newComplex[1];
			if (((nextA * nextA) + (nextB * nextB)) > 4) {
				if (fancyMode) {
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
				}
				tempGraph[x][y] = i;
				screenGraph[x][y] = i;
				break;
			}
			i++;
		}
		y++;
	}
	
}

void setColor(float val) {
	if (val < 0.01){
		glColor3f(0.5,
			0.0,
			val * 50.0);
	}
	else if (val < 0.1) {
		glColor3f(
			0.5 + (val * 5.0),
			0.0,
			0.5 - (val * 5.0));
	}
	else {
		glColor3f(1.0,
			val,
			val * 0.5);
	}
}

void redisplay() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_POINTS);
	for (int x = 0; x < windowWidth; x++) {
		for (int y = 0; y < windowHeight; y++) {
			glVertex2i(x, y);
			
			if (screenGraph[x][y] < 0) {
				glColor3f(0.0, 0.0, 0.0);
			}
			else {
				if (fancyMode) {
					for (vector<int>::size_type vertorIndex = 0; vertorIndex != notInFrac.size(); vertorIndex++) {
						if (screenGraph[x][y] == notInFrac[vertorIndex]) {
							setColor(vertorIndex / (float)notInFrac.size());
							break;
						}
					}
				}
				else {
					setColor(screenGraph[x][y] / 1000.0);
				}
			}
		}
	}
	glEnd();
	glFlush();
	cout << "mandelzoom " << viewX1 << " " << viewX2 << " " << viewY1 << " " << viewY2 << " " << windowWidth << " " << windowHeight << endl;
}

void recalculateFrac(bool putOnStack) {
	// Clear the window.
	notInFrac.resize(0);
	tempGraph.clear();
	screenGraph.clear();
	screenGraph.resize(windowWidth);
	tempGraph.resize(windowWidth);
	for (int widthIndex = 0; widthIndex < windowWidth; widthIndex++) {
		screenGraph[widthIndex].resize(windowHeight);
		tempGraph[widthIndex].resize(windowHeight);
	}
	int x = 0;
	while (x < windowWidth) {
			drawHelp(x);
			x++;
	}


	if (putOnStack) {
		graphStack.push_back(tempGraph);
		vector<double> tempCoordinates;
		tempCoordinates.push_back(viewX1);
		tempCoordinates.push_back(viewX2);
		tempCoordinates.push_back(viewY1);
		tempCoordinates.push_back(viewY2);
		tempCoordinates.push_back(0.0);
		coordinateStack.push_back(tempCoordinates);
	}
	
	else {
		graphStack[graphStack.size() - (1 + popCount)] = tempGraph;
	}

	if (fancyMode) {
		sort(notInFrac.begin(), notInFrac.end());
	}

		
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
	double oldProp = (windowWidth / (double) windowHeight);
	double newProp = (newWidth / (double) newHeight);
	if (oldProp != newProp) {
		if (newProp > oldProp) {
			newViewY2 = newViewY1 + (int) ((1.0 / oldProp) * newWidth);
		}
		else {
			newViewX2 = newViewX1 + (int)( oldProp * newHeight);
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
	needRecalc = true;
}

void processLeftUp(int x, int y)
// Function for processing mouse left botton up events.
{
	if (rubberBanding)
	{
		while (popCount > 0) {
			coordinateStack.pop_back();
			graphStack.pop_back();
			popCount--;
		}

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
	int oldW = windowWidth;
	int oldH = windowHeight;
	if ((w != 0) && (h != 0))
	{
		windowWidth = w;
		windowHeight = h;
	}
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);

	if ((w != 0) && (h != 0))
	{

		
		if (firstOpen) {
			needRecalc = true;
		}
		else {
			if ((oldW == windowWidth) && (oldH = windowHeight)) {
				needRedraw = true;
			}
			else {
				while (popCount > 0) {
					coordinateStack.pop_back();
					graphStack.pop_back();
					popCount--;
				}

				for (vector<int>::size_type vertorIndex = 0; vertorIndex != coordinateStack.size(); vertorIndex++) {
					coordinateStack[vertorIndex].pop_back();
					coordinateStack[vertorIndex].push_back(1.0);
				}
				recalculateFrac(false);
			}
		}
		firstOpen = false;
	}
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

void checkStatus() {
	if (needRecalc) {
		needRecalc = false;
		needRedraw = false;
		recalculateFrac(true);
	}
	else if (needRedraw) {
		needRedraw = false;
		redisplay();
	}
}

void display() {
	if (!needRecalc) {
		needRedraw = true;
	}
}

void pop() {
	if (popCount < (coordinateStack.size() - 1)) {
		popCount++;
		viewX1 = coordinateStack[(coordinateStack.size() - 1) - popCount][0];
		viewX2 = coordinateStack[(coordinateStack.size() - 1) - popCount][1];
		viewY1 = coordinateStack[(coordinateStack.size() - 1) - popCount][2];
		viewY2 = coordinateStack[(coordinateStack.size() - 1) - popCount][3];
		if (coordinateStack[(coordinateStack.size() - 1) - popCount].at(4) != 0.0) {
			coordinateStack[(coordinateStack.size() - 1) - popCount].at(4) = (0.0);
			recalculateFrac(false);
		}
		else {
			screenGraph = graphStack[(coordinateStack.size() - 1) - popCount];
			needRedraw = true;
		}
		glFlush();
	}
	
}

void push() {
	if ((coordinateStack.size() > 0) && (popCount > 0)) {
		popCount--;
		viewX1 = coordinateStack[(coordinateStack.size() - 1) - popCount][0];
		viewX2 = coordinateStack[(coordinateStack.size() - 1) - popCount][1];
		viewY1 = coordinateStack[(coordinateStack.size() - 1) - popCount][2];
		viewY2 = coordinateStack[(coordinateStack.size() - 1) - popCount][3];
		if (coordinateStack[(coordinateStack.size() - 1) - popCount].at(4) != 0.0) {
			coordinateStack[(coordinateStack.size() - 1) - popCount].at(4) = 1.0;
			recalculateFrac(false);
		}
		else {
			screenGraph = graphStack[(coordinateStack.size() - 1) - popCount];
			redisplay();
		}
		glFlush();
	}
	
}

void colorNorm() {
	if (fancyMode == true) {
		fancyMode = false;
		for (vector<int>::size_type vertorIndex = 0; vertorIndex != coordinateStack.size(); vertorIndex++) {
			coordinateStack[vertorIndex].pop_back();
			coordinateStack[vertorIndex].push_back(1.0);
		}
		recalculateFrac(false);
	}
}

void colorFancy() {
	if (fancyMode == false) {
		fancyMode = true;
		for (vector<int>::size_type vertorIndex = 0; vertorIndex != coordinateStack.size(); vertorIndex++) {
			coordinateStack[vertorIndex].pop_back();
			coordinateStack[vertorIndex].push_back(1.0);
		}
		recalculateFrac(false);
	}
}

void mainMenu(int item)
// Callback for processing main menu.
{
	switch (item)
	{
	case 1: pop(); break;
	case 2: push(); break;
	case 3: std::exit(0); break;
	case 4: colorNorm(); break;
	case 5: colorFancy(); break;
	}
}



void setMenus()
// Function for creating menus.
{
	glutCreateMenu(mainMenu);
	glutAddMenuEntry("Pop", 1);
	glutAddMenuEntry("Push", 2);
	glutAddMenuEntry("Exit", 3);
	glutAddMenuEntry("Normal Color Mode", 4);
	glutAddMenuEntry("Fancy Color Mode (WARNING: costly; was an experiment)", 5);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
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
	glutInitWindowSize(atoi(argv[5]), atoi(argv[6]));
	glutInitWindowPosition(INITIAL_WIN_X, INITIAL_WIN_Y);
	windowWidth = INITIAL_WIN_W;
	windowHeight = INITIAL_WIN_H;
	viewX1 =  atof(argv[1]);
	viewX2 = atof(argv[2]);
	viewY1 = atof(argv[3]);
	viewY2 = atof(argv[4]);
	glutCreateWindow("Mandelzoom");

	// You don't (yet) want to know what this does.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)atoi(argv[5]), 0.0, (double)atoi(argv[6])), glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.375, 0.375, 0.0);

	// Set the color for clearing the normal screen window.
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// Set the color for drawing the house.
	glColor3f(0.0, 0.0, 1.0);

	// Set the callbacks for the normal screen window.
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(escExit);
	glutMotionFunc(rubberBand);
	glutPassiveMotionFunc(rubberBand);
	glutIdleFunc(checkStatus);

	// Set up the menus.
	setMenus();

	glutMainLoop();
}