#include <cfloat>
#include <cmath>
#include <cstdlib>
#include "matrix.h"
#include <vector>
#include <iostream>
#include <math.h> 

using namespace std;



Vector::Vector()
{
	Vector::values.resize(3);
	Vector::values[0] = 0.0;
	Vector::values[1] = 0.0;
	Vector::values[2] = 1.0;
}


Vector::Vector(const Vector& oldVector)
{
	Vector::values.resize(3);
	Vector::values[0] = oldVector.values[0];
	Vector::values[1] = oldVector.values[1];
	Vector::values[2] = oldVector.values[2];
}

Vector::Vector(const double x, const double y)
{
	Vector::values.resize(3);
	Vector::values[0] = x;
	Vector::values[1] = y;
	Vector::values[2] = 1.0;
}


Vector::~Vector()
{
	Vector::values.clear();
}

vector<double> Vector::getValues() {
	return Vector::values;
}

//only created to help test the Vector(const Vector& oldVector) constructor; used to make sure it makes a deep copy.
void Vector::plus1() {
	Vector::values[0] = Vector::values[0] + 1.0;
	Vector::values[1] = Vector::values[1] + 1.0;
	Vector::values[2] = Vector::values[2] + 1.0;
}

double& Vector::operator[](int index)
{
	double* newPTR = &(Vector::values.at(index));
	return *(newPTR);
}

Matrix::Matrix()
{
	int row = 0;
	while (row < 3) {
		int col = 0;
		while (col < 3) {
			Matrix::data[row][col] = 0.0;
			col++;
		}
		row++;
	}
	Matrix::data[0][0] = 1.0;
	Matrix::data[1][1] = 1.0;
	Matrix::data[2][2] = 1.0;
}

void Matrix::add1() {
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			Matrix::data[row][col] = Matrix::data[row][col] + 1.0;
		}
	}
}

void Matrix::randMatrix() {
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			Matrix::data[row][col] = ((double)rand() / (RAND_MAX)) + 1;
		}
	}
}

array<array<double, 3>, 3> Matrix::getData(){
	return Matrix::data;
}

Matrix::Matrix(Matrix& oldMatrix)
{
	array<array<double, 3>, 3> oldValues = oldMatrix.getData();
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			Matrix::data[row][col] = oldValues[row][col];
		}
	}
}

Matrix::~Matrix()
{
}

Matrix* Matrix::multiply(const Matrix* otherMatrix) const
{
	Matrix* answer = new Matrix();
	array<array<double, 3>, 3> otherData = otherMatrix->data;

	answer->data[0][0] = (data[0][0] * otherData[0][0]) + (data[0][1] * otherData[1][0]) + (data[0][2] * otherData[2][0]);
	answer->data[0][1] = (data[0][0] * otherData[0][1]) + (data[0][1] * otherData[1][1]) + (data[0][2] * otherData[2][1]);
	answer->data[0][2] = (data[0][0] * otherData[0][2]) + (data[0][1] * otherData[1][2]) + (data[0][2] * otherData[2][2]);
	answer->data[1][0] = (data[1][0] * otherData[0][0]) + (data[1][1] * otherData[1][0]) + (data[1][2] * otherData[2][0]);
	answer->data[1][1] = (data[1][0] * otherData[0][1]) + (data[1][1] * otherData[1][1]) + (data[1][2] * otherData[2][1]);
	answer->data[1][2] = (data[1][0] * otherData[0][2]) + (data[1][1] * otherData[1][2]) + (data[1][2] * otherData[2][2]);
	answer->data[2][0] = (data[2][0] * otherData[0][0]) + (data[2][1] * otherData[1][0]) + (data[2][2] * otherData[2][0]);
	answer->data[2][1] = (data[2][0] * otherData[0][1]) + (data[2][1] * otherData[1][1]) + (data[2][2] * otherData[2][1]);
	answer->data[2][2] = (data[2][0] * otherData[0][2]) + (data[2][1] * otherData[1][2]) + (data[2][2] * otherData[2][2]);

	return answer;
}

Vector* Matrix::multiply(const Vector* theVector) const
{
	Vector* answer = new Vector();
	Vector theVectorCopy = Vector(*theVector);
	vector<double> vectorData = theVectorCopy.getValues();

	(*answer)[0] = (vectorData[0] * data[0][0]) + (vectorData[1] * data[0][1]) + (vectorData[2] * data[0][2]);
	(*answer)[1] = (vectorData[0] * data[1][0]) + (vectorData[1] * data[1][1]) + (vectorData[2] * data[1][2]);
	(*answer)[2] = (vectorData[0] * data[2][0]) + (vectorData[1] * data[2][1]) + (vectorData[2] * data[2][2]);

	return answer;
}

double* Matrix::operator[](int index) const
{
	array<double, 3> rowCopy = data[index];
	double* answer = &rowCopy[0];
	return answer;
}

Matrix* Matrix::translation(double deltaX, double deltaY)
{
	Matrix* answer = new Matrix();
	answer->data[0][2] = deltaX;
	answer->data[1][2] = deltaY;
	return answer;
}

Matrix* Matrix::rotation(double theta)
{
	Matrix* answer = new Matrix();
	answer->data[0][0] = cos(theta);
	answer->data[0][1] = (0 - sin(theta));
	answer->data[1][0] = sin(theta);
	answer->data[1][1] = cos(theta);
	return answer;
}
Matrix* Matrix::shearing(double shearXY, double shearYX)
{
	Matrix* answer = new Matrix();
	answer->data[0][1] = shearXY;
	answer->data[1][0] = shearYX;
	return answer;
}

Matrix* Matrix::scaling(double scaleX, double scaleY)
{
	Matrix* answer = new Matrix();
	answer->data[0][0] = scaleX;
	answer->data[1][1] = scaleY;
	return answer;
}

Matrix* Matrix::getInverse() const
{
	Matrix* answer = new Matrix();
	double det = getDeterminant();

	answer->data[0][0] = -data[1][2] * data[2][1] + data[1][1] * data[2][2];
	answer->data[0][1] = data[0][2] * data[2][1] - data[0][1] * data[2][2];
	answer->data[0][2] = -data[0][2] * data[1][1] + data[0][1] * data[1][2];
	answer->data[1][0] = data[1][2] * data[2][0] - data[1][0] * data[2][2];
	answer->data[1][1] = -data[0][2] * data[2][0] + data[0][0] * data[2][2];
	answer->data[1][2] = data[0][2] * data[1][0] - data[0][0] * data[1][2];
	answer->data[2][0] = -data[1][1] * data[2][0] + data[1][0] * data[2][1];
	answer->data[2][1] = data[0][1] * data[2][0] - data[0][0] * data[2][1];
	answer->data[2][2] = -data[0][1] * data[1][0] + data[0][0] * data[1][1];

	for (int i = 0; i<3; i++)
		for (int j = 0; j<3; j++)
			answer->data[i][j] /= det;

	return answer;
}

double Matrix::getDeterminant() const
{
	return  -data[0][2] * data[1][1] * data[2][0] + data[0][1] * data[1][2] * data[2][0] +
		data[0][2] * data[1][0] * data[2][1] - data[0][0] * data[1][2] * data[2][1] -
		data[0][1] * data[1][0] * data[2][2] + data[0][0] * data[1][1] * data[2][2];
}
