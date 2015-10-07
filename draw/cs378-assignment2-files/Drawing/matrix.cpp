#include <cfloat>
#include <cmath>
#include <cstdlib>
#include "matrix.h"

using namespace std;

Vector::Vector()
{

}

Vector::Vector(const Vector& oldVector) 
{

}

Vector::Vector(const double x, const double y)
{

}


Vector::~Vector()
{

}

double& Vector::operator[](int index) const
{
	return *(new double);
}

Matrix::Matrix() 
{

}

Matrix::Matrix(const Matrix& oldMatrix) 
{

}

Matrix::~Matrix()
{

}

Matrix* Matrix::multiply(const Matrix* otherMatrix) const
{
	return NULL;
}

Vector* Matrix::multiply(const Vector* theVector) const
{
	return NULL;
}

double* Matrix::operator[](int index) const
{
	return NULL;
}

Matrix* Matrix::translation(double deltaX, double deltaY)
{
	return NULL;
}

Matrix* Matrix::rotation(double theta)
{
	return NULL;
}
Matrix* Matrix::shearing(double shearXY, double shearYX)
{
	return NULL;
}

Matrix* Matrix::scaling(double scaleX, double scaleY)
{
	return NULL;
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
