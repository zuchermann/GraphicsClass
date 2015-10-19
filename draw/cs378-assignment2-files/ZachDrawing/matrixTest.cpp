#include "matrixTest.h"
#include "matrix.h"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <math.h> 

using namespace std;

matrixTest::matrixTest()
{
	// We assume pass at start.
	passTest = true; 

	//tests go here
	cout << endl << "////VECTOR////" << endl;
	//tests vector() constructor
	testVectorConstructor1(1000);
	//tests vector(const double x, const double y) constructor
	testVectorConstructorDD(1000);
	//tests vector(const Vector& oldVector) constructor
	testVectorConstructorV(1000);
	//tests [] operator in Vector class
	testOverLoadedBrackets(1000);

	cout << endl << "////MATRIX////" << endl;
	//tests for empty matrix constructor
	testMatrixConstructor(1000);
	//tests for Matrix(&Matrix) constructor
	testMatrixConstructorM(1000);
	//tests matrix multiplication
	testMatrixMult(1000);
	//tests matrix-vector mult
	testMatrixVectorMult(1000);
	//tests [] for matrix class
	testMatrixBrackets(1000);
	//tests translation
	testTranslation(1000);
	//test Rotation
	testRotation(1000);
	//test shearing
	testShearing(1000);
	//test scaling
	testScaling(1000);

	
}


matrixTest::~matrixTest()
{
}

void matrixTest::testVectorConstructor1(int numTests) {
	while (numTests > 0) {
		Vector testVector = Vector();
		vector<double> testValues = testVector.getValues();
		for (int i = 0; i < 3; i++) {
			//zprint -- will print out each vector component
			//cout << testValues[i] << " ";
			if (testValues[i] != 0) {
				if (!(i == 2) || testValues[i] != 1.0) {
					cout << "***FAIL** in vector() constructor: some coordinate is nonzero or homogeneous coordinate is not 1.0!" << endl;
					passTest = false;
					return;
				}
			}
		}
		//zprint -- will print new line after each vector
		//cout << endl;
		if (testValues.size() != 3) {
			cout << "***FAIL** in vector() constructor: more than three components in a vector!" << endl;
			passTest = false;
			return;
		}
		numTests--;
	}
	cout << "passed tests for Vecotr()" << endl;
}

void matrixTest::testVectorConstructorDD(int numTests) {
	while (numTests > 0) {
		double x = ((double)rand() / (RAND_MAX)) + 1;
		double y = ((double)rand() / (RAND_MAX)) + 1;
		Vector testVector = Vector(x, y);
		vector<double> testValues = testVector.getValues();
		double vectorX = testValues[0];
		double vectorY = testValues[1];
		double vectorH = testValues[2];
		//zprint; prints out each generated vector
		//cout << "Vector = " << vectorX << " " << vectorY << " " << vectorH << endl;
		if (vectorX != x) {
			cout << "***FAIL** in vector(double, double) constructor: vector x-coordinate not equal to input x-coordinate!" << endl;
			passTest = false;
			return;
		}
		if (vectorY != y) {
			cout << "***FAIL** in vector(double, double) constructor: vector y-coordinate not equal to input y-coordinate!" << endl;
			passTest = false;
			return;
		}
		if (vectorH != 1.0) {
			cout << "***FAIL** in vector(double, double) constructor: vector homogeneous coordinate not equal to 1.0!" << endl;
			passTest = false;
			return;
		}
		if (testValues.size() != 3) {
			cout << "***FAIL** in vector(double, double) constructor: vector length not 3!" << endl;
			passTest = false;
			return;
		}
		numTests--;
	}
	cout << "passed tests for Vecotr(double, double)" << endl;
}

void matrixTest::testVectorConstructorV(int numTests) {
	while (numTests > 0) {
		double x1 = ((double)rand() / (RAND_MAX)) + 1;
		double y1 = ((double)rand() / (RAND_MAX)) + 1;
		Vector testVector1 = Vector(x1, y1);
		Vector testVector2 = Vector(testVector1);
		double xv1 = testVector1.getValues()[0];
		double yv1 = testVector1.getValues()[1];
		double h1 = testVector1.getValues()[2];
		double xv2 = testVector2.getValues()[0];
		double yv2 = testVector2.getValues()[1];
		double h2 = testVector2.getValues()[2];
		if ((xv1 != x1) || (yv1 != y1)) {
			cout << "***FAIL** in vector(&Vector) constructor: problem with vector(double double) constructor, input coordinates not equal to vector coordinates!" << endl;
			passTest = false;
			return;
		}
		if ((xv1 != xv2)) {
			cout << "***FAIL** in vector(&Vector) constructor: the x-coordinate of the copied vector does not equal the x-coordinate of the original vector!" << endl;
			passTest = false;
			return;
		}
		if ((yv1 != yv2)) {
			cout << "***FAIL** in vector(&Vector) constructor: the y-coordinate of the copied vector does not equal the y-coordinate of the original vector!" << endl;
			passTest = false;
			return;
		}
		if (h2 != h1) {
			cout << "***FAIL** in vector(&Vector) constructor: the homogeneous coordinate of the copied vector does not equal the homogeneous coordinate of the original vector!" << endl;
			passTest = false;
			return;
		}
		testVector1.plus1();
		xv1 = testVector1.getValues()[0];
		yv1 = testVector1.getValues()[1];
		h1 = testVector1.getValues()[2];
		xv2 = testVector2.getValues()[0];
		yv2 = testVector2.getValues()[1];
		h2 = testVector2.getValues()[2];
		//cout << xv1 << " " << yv1 << " " << xv2 << " " << yv2 << endl;
		if ((xv2 != x1) || (yv2 != y1)) {
			cout << "***FAIL** in vector(&Vector) constructor: copy is not deep and it should be!" << endl;
			passTest = false;
			return;
		}
		if ((xv1 != (xv2 + 1.0))) {
			cout << "***FAIL** in vector(&Vector) constructor: copy of x-coordinate is not deep and it should be!" << endl;
			passTest = false;
			return;
		}
		if ((yv1 != (yv2 + 1.0))) {
			cout << "***FAIL** in vector(&Vector) constructor: copy of y-coordinate is not deep and it should be! " << endl;
			passTest = false;
			return;
		}
		if ((h1 != (h2 + 1.0))) {
			cout << "***FAIL** in vector(&Vector) constructor: copy of homogeneous coordinate is not deep and it should be!" << endl;
			passTest = false;
			return;
		}
		if (testVector2.getValues().size() != 3) {
			cout << "***FAIL** in vector(&Vector) constructor: vector length not 3!" << endl;
			passTest = false;
			return;
		}
		numTests--;
	}
	cout << "passed tests for Vector(&Vector)" << endl;
}

void matrixTest::testOverLoadedBrackets(int numTests) {
	while (numTests > 0) {
		double x1 = ((double)rand() / (RAND_MAX)) + 1;
		double y1 = ((double)rand() / (RAND_MAX)) + 1;
		Vector testVector = Vector(x1, y1);
		Vector testVector2 = Vector();
		if (testVector[0] != x1) {
			cout << "***FAIL** in [] operator in Vector class: expected " << testVector[0] << " to be equal to " << x1 << " while accessing x-coordinate" << endl;
			passTest = false;
			return;
		}
		if (testVector[1] != y1) {
			cout << "***FAIL** in [] operator in Vector class: expected " << testVector[1] << " to be equal to " << y1 << " while accessing y-coordinate" << endl;
			passTest = false;
			return;
		}
		if (testVector[2] != 1.0) {
			cout << "***FAIL** in [] operator in Vector class: expected " << testVector[2] << " to be equal to " << 1.0 << " while accessing homogeneous coordinate" << endl;
			passTest = false;
			return;
		}
		double newX = x1 + 1.0;
		double newY = y1 + 1.0;
		testVector[0] = newX;
		if (testVector[0] != (x1 + 1.0)) {
			cout << "***FAIL** in [] operator in Vector class: expected " << testVector[0] << " to be equal to " << x1 << " plus 1! problem lies in trying to modify x-coordinate with [] operator" << endl;
			passTest = false;
			return;
		}
		if ((testVector[1] != y1) || (testVector[2] != 1.0)) {
			cout << "***FAIL** in [] operator in Vector class: either y or homogeneous coordinate have changed as a result of modifying only the x-coordinate with [] operator" << endl;
			passTest = false;
			return;
		}
		testVector[1] = newY;
		if (testVector[1] != (y1 + 1.0)) {
			cout << "***FAIL** in [] operator in Vector class: expected " << testVector[1] << " to be equal to " << y1 << " plus 1! problem lies in trying to modify y-coordinate with [] operator" << endl;
			passTest = false;
			return;
		}
		if ((testVector[0] != (x1 + 1.0)) || (testVector[2] != 1.0)) {
			cout << "***FAIL** in [] operator in Vector class: either x or homogeneous coordinate have changed as a result of attempting to modify only the y-coordinate with [] operator" << endl;
			passTest = false;
			return;
		}
		testVector[2] = 2.0;
		if (testVector[2] != 2.0) {
			cout << "***FAIL** in [] operator in Vector class: expected " << testVector[2] << " to be equal to " << 2.0 << "  problem lies in trying to modify homogeneous coordinate with [] operator" << endl;
			passTest = false;
			return;
		}
		if ((testVector[0] != (x1 + 1.0)) || (testVector[1] != (y1 + 1.0))) {
			cout << "***FAIL** in [] operator in Vector class: either x or y coordinate have changed as a result of attempting to modify only the homogeneous coordinate with [] operator" << endl;
			passTest = false;
			return;
		}
		if ((testVector2[0] != 0.0) || (testVector2[1] != 0.0) || (testVector2[2] != 1.0)) {
			cout << "***FAIL** in [] operator in Vector class: modifying the coordinates with [] operator of one vector (incorrectly) changed the coordinates of a seperate vector" << endl;
			passTest = false;
			return;
		}
		numTests--;
	}
	cout << "passed tests for [] op in Vector class" << endl;
}

void matrixTest::testMatrixConstructor(int numTests) {
	while (numTests > 0) {
		Matrix testMatrix = Matrix();
		array<array<double, 3>, 3> testData = testMatrix.getData();
		int row = 0;
		int col = 0;
		while (row < 3) {
			col = 0;
			while (col < 3) {
				//cout << testData[row][col] << " ";
				if (col == row) {
					if (testData[row][col] != 1.0) {
						cout << "***FAIL** in Matrix() constructor: some entry along the diagonal is not 1" << endl;
						passTest = false;
						return;
					}
				}
				else
				{
					if (testData[row][col] != 0.0) {
						cout << "***FAIL** in Matrix() constructor: some entry off the diagonal is not 0. Instead is: "<< testData[row][col] << endl;
						passTest = false;
						return;
					}
				}
				col++;
			}
			//cout << endl;
			row++;
		}
		//cout << endl;
		numTests--;
	}
	cout << "passed tests for Matrix()" << endl;
}

void matrixTest::testMatrixConstructorM(int numTests) {
	while (numTests > 0) {
		Matrix testMatrix = Matrix();
		Matrix testMatrix2 = Matrix(testMatrix);
		array<array<double, 3>, 3> m1Data = testMatrix.getData();
		array<array<double, 3>, 3> m2Data = testMatrix2.getData();
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				if (m1Data[row][col] != m2Data[row][col])
				{
					cout << "***FAIL** in Matrix(&Matrix) constructor: expected " << m1Data[row][col] << " to be equal to " << m2Data[row][col] << endl;
					passTest = false;
					return;
				}
			}
		}
		testMatrix.add1();
		m1Data = testMatrix.getData();
		m2Data = testMatrix2.getData();
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				if (m1Data[row][col] != (m2Data[row][col] + 1.0))
				{
					cout << "***FAIL** in Matrix(&Matrix) constructor: expected " << m1Data[row][col] << " to be equal to " << m2Data[row][col] << " plus 1. Error from copy not being deep!" << endl;
					passTest = false;
					return;
				}
			}
		}
		numTests--;
	}
	cout << "passed tests for Matrix(&Matrix)" << endl;
}

void matrixTest::testMatrixMult(int numTests) {
	while (numTests > 0) {
		//test Identity
		Matrix ident = Matrix();
		Matrix rand1 = Matrix();
		rand1.randMatrix();
		Matrix result1 = *(ident.multiply(&rand1));
		array<array<double, 3>, 3> identVal = ident.getData();
		array<array<double, 3>, 3> rand1Val = rand1.getData();
		array<array<double, 3>, 3> result1Val = result1.getData();
		for (int i = 0; i < 2; i++) {
			for (int row = 0; row < 3; row++) {
				for (int col = 0; col < 3; col++) {
					//cout << identVal[row][col] << " ";
					if (row == col) {
						if (identVal[row][col] != 1.0) {
							cout << "***FAIL** in matrix multiplication: expected " << identVal[row][col] << " to be equal to 1.0! Multiplicatopn is modifying value of an operand" << endl;
							passTest = false;
							return;
						}
					}
					else {
						if (identVal[row][col] != 0.0) {
							cout << "***FAIL** in matrix multiplication: expected " << identVal[row][col] << " to be equal to 0.0! Multiplicatopn is modifying value of an operand" << endl;
							passTest = false;
							return;
						}
					}
					if (rand1Val[row][col] != result1Val[row][col]) {
						cout << "***FAIL** in matrix multiplication: expected " << rand1Val[row][col] << " to be equal to " << result1Val[row][col] << " Multiplication of a matrix with Identity should not change matrix" << endl;
						passTest = false;
						return;
					}
				}
				//cout << endl;
			}
			result1 = *(rand1.multiply(&ident));
			identVal = ident.getData();
			rand1Val = rand1.getData();
			result1Val = result1.getData();
		}
		numTests--;
	}
	cout << "passed tests for matrix mult" << endl;
}

void matrixTest::testMatrixVectorMult(int numTests) {
	while (numTests > 0) {
		Matrix ident = Matrix();
		double x1 = ((double)rand() / (RAND_MAX)) + 1;
		double y1 = ((double)rand() / (RAND_MAX)) + 1;
		Vector* testVector = new Vector(x1, y1);
		Vector* result1 = ident.multiply(testVector);
		
		if ((*result1)[0] != x1) {
			cout << "***FAIL** in matrix-vector multiplication: expected x-coordinate " << (*result1)[0] << " to be equal to " << x1 << " Multiplication of a vector with Identity should not change vector!" << endl;
			passTest = false;
			return;
		}
		if ((*result1)[1] != y1) {
			cout << "***FAIL** in matrix-vector multiplication: expected y-coordinate " << (*result1)[0] << " to be equal to " << x1 << " Multiplication of a vector with Identity should not change vector!" << endl;
			passTest = false;
			return;
		}
		if ((*result1)[2] != 1.0) {
			cout << "***FAIL** in matrix-vector multiplication: expected homogeneous coordinate " << (*result1)[0] << " to be equal to 1. Multiplication of a vector with Identity should not change vector!" << endl;
			passTest = false;
			return;
		}
		numTests--;
	}
	cout << "passed tests for matrix-vector mult" << endl;
}

void matrixTest::testMatrixBrackets(int numTests) {
	while (numTests > 0) {
		Matrix randM = Matrix();
		randM.randMatrix();
		array<array<double, 3>, 3> randData = randM.getData();
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				double ans1 = randM[row][col];
				double ans2 = randData[row][col];
				if (ans1 != ans2) {
					cout << "***FAIL** in [] operator in Matrix class: expected " << ans1 << " to be equal to " << ans2 << endl;
					passTest = false;
					return;
				}
			}
		}
		numTests--;
	}
	cout << "passed tests for [] for matrix" << endl;
}

void matrixTest::testTranslation(int numTests) {
	while (numTests > 0) {
		double dx = ((double)rand() / (RAND_MAX)) + 1;
		double dy = ((double)rand() / (RAND_MAX)) + 1;
		Matrix testMatrix = *Matrix::translation(dx, dy);
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				double entry = testMatrix[row][col];
				if (row == col) {
					if (entry != 1.0) {
						cout << "***FAIL** in translate() in Matrix class: expected " << entry << " to be equal to 1.0!" << endl;
						passTest = false;
						return;
					}
				}
				else {
					double exp = 1.0;
					if (col == 2) {
						if (row == 0) {
							exp = dx;
						}
						if (row == 1) {
							exp = dy;
						}
						if (exp != entry) {
							cout << "***FAIL** in translate() in Matrix class: expected " << entry << " to be equal to" << exp << endl;
							passTest = false;
							return;
						}
					}
					else {
						if (entry != 0.0) {
							cout << "***FAIL** in translate() in Matrix class: expected " << entry << " to be equal to 0.0!" << endl;
							passTest = false;
							return;
						}
					}
				}
			}
		}
		numTests--;
	}
	cout << "passed tests for transformation in matrix" << endl;
}

void matrixTest::testRotation(int numTests) {
	Matrix ident = Matrix();
	Matrix rot = *Matrix::rotation(0.0);
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			double entry = rot[row][col];
			double exp = ident[row][col];
			if (ident[row][col] != rot[row][col]) {
				cout << "***FAIL** in rotate() in Matrix class: expected " << entry << " to be equal to " << exp << "! rotation by zero degrees should produce identity matrix." << endl;
				passTest = false;
				return;
			}
		}
	}
	while (numTests > 0) {
		double theta = ((double)rand() / (RAND_MAX)) * 3;
		Matrix testMatrix = *Matrix::rotation(theta);
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				double thisEntry = testMatrix[row][col];
				if ((row == 2) || (col == 2)) {
					if (row == col) {
						if (thisEntry != 1.0) {
							cout << "***FAIL** in rotate() in Matrix class: expected " << thisEntry << " to be equal to 1.0" << endl;
							passTest = false;
							return;
						}
					}
					else {
						if (thisEntry != 0.0) {
							cout << "***FAIL** in rotate() in Matrix class: expected " << thisEntry << " to be equal to 0.0" << endl;
							passTest = false;
							return;
						}
					}
				}
				else {
					if (row == 0) {
						if (col == 0) {
							if (thisEntry != cos(theta)) {
								cout << "***FAIL** in rotate() in Matrix class: expected " << thisEntry << " [0][0] to be equal to cos(theta): " << cos(theta) << endl;
								passTest = false;
								return;
							}
						}
						if (col == 1) {
							if (thisEntry != (0 - sin(theta))) {
								cout << "***FAIL** in rotate() in Matrix class: expected " << thisEntry << " [0][1] to be equal to sin(theta): -" << sin(theta) << endl;
								passTest = false;
								return;
							}
						}
					}
					if (row == 1) {
						if (col == 0) {
							if (thisEntry != sin(theta)) {
								cout << "***FAIL** in rotate() in Matrix class: expected " << thisEntry << " [1][0] to be equal to sin(theta): " << sin(theta) << endl;
								passTest = false;
								return;
							}
						}
						if (col == 1) {
							if (thisEntry != cos(theta)) {
								cout << "***FAIL** in rotate() in Matrix class: expected " << thisEntry << " [1][1] to be equal to cos(theta): " << cos(theta) << endl;
								passTest = false;
								return;
							}
						}
					}
				}
			}
		}
		numTests--;
	}
	cout << "passed tests for rotation in matrix" << endl;
}

void matrixTest::testShearing(int numTests) {
	Matrix testMatrix1 = *Matrix::shearing(0.0, 0.0);
	Matrix ident = Matrix();
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			double entry = testMatrix1[row][col];
			double exp = ident[row][col];
			if (ident[row][col] != testMatrix1[row][col]) {
				cout << "***FAIL** in shearing() in Matrix class: expected " << entry << " to be equal to " << exp << "! rotation by (0,0) should produce identity matrix." << endl;
				passTest = false;
				return;
			}
		}
	}
	while (numTests > 0) {
		double shearXY = ((double)rand() / (RAND_MAX)) + 1;
		double shearYX = ((double)rand() / (RAND_MAX)) + 1;
		Matrix testMatrix = *Matrix::shearing(shearXY, shearYX);
		for (int row = 0; row < 3; row++) {
			for (int col = 0; col < 3; col++) {
				double thisEntry = testMatrix[row][col];
				if ((row == 2) || (col == 2)) {
					if (row == col) {
						if (thisEntry != 1.0) {
							cout << "***FAIL** in shearing() in Matrix class: expected " << thisEntry << " to be equal to 1.0" << endl;
							passTest = false;
							return;
						}
					}
					else {
						if (thisEntry != 0.0) {
							cout << "***FAIL** in shearing() in Matrix class: expected " << thisEntry << " to be equal to 0.0" << endl;
							passTest = false;
							return;
						}
					}
				}
				else {
					if (row == 0) {
						if (col == 0) {
							if (thisEntry != 1.0) {
								cout << "***FAIL** in shearing() in Matrix class: expected " << thisEntry << " [0][0] to be equal to 0.0!" << endl;
								passTest = false;
								return;
							}
						}
						if (col == 1) {
							if (thisEntry != shearXY) {
								cout << "***FAIL** in shearing() in Matrix class: expected " << thisEntry << " [0][1] to be equal to shearXY: " << shearXY << endl;
								passTest = false;
								return;
							}
						}
					}
					if (row == 1) {
						if (col == 0) {
							if (thisEntry != shearYX) {
								cout << "***FAIL** in shearing() in Matrix class: expected " << thisEntry << " [1][0] to be equal to shearYX: " << shearYX << endl;
								passTest = false;
								return;
							}
						}
						if (col == 1) {
							if (thisEntry != 1.0) {
								cout << "***FAIL** in shearing() in Matrix class: expected " << thisEntry << " [1][1] to be equal to 1.0!" << endl;
								passTest = false;
								return;
							}
						}
					}
				}
			}
		}
		numTests--;
	}
	cout << "passed tests for shearing in matrix" << endl;
}

void matrixTest::testScaling(int numTests) {
	while (numTests > 0) {
		//no reason to test just yet
		numTests--;
	}
	cout << "passed tests for scaling in matrix" << endl;
}