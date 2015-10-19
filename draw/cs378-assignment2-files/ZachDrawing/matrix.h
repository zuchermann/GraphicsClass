#ifndef MATRIX_H
#define MATRIX_H
#include <vector>
#include <array>

class Vector
{
public:
	Vector();
	Vector(const Vector& oldVector);
	Vector(const double x, const double y);
	~Vector();
	std::vector<double> getValues();
	void plus1();
	double& operator[](int index);
private:
	std::vector<double> values;
};

class Matrix
{
public:
	Matrix();
	Matrix(Matrix& oldMatrix);
	~Matrix();
	Matrix* multiply(const Matrix* otherMatrix) const;
	Vector* multiply(const Vector* theVector) const;
	double* operator[](int index) const;
	static Matrix* translation(double deltaX, double deltaY);
	static Matrix* rotation(double theta);
	static Matrix* shearing(double shearXY, double shearYX);
	static Matrix* scaling(double scaleX, double scaleY);
	void randMatrix();
	void add1();
	Matrix* getInverse() const;
	std::array<std::array<double, 3>, 3> getData();
private:
	//Student must implement.
	std::array<std::array<double, 3>, 3> data;
	double getDeterminant() const;
};


#endif
