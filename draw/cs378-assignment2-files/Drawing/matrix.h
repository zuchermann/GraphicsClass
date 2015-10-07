#ifndef MATRIX_H
#define MATRIX_H

class Vector
{
  public:
   Vector();
   Vector(const Vector& oldVector);
   Vector(const double x, const double y);
   ~Vector();
   double& operator[](int index) const;
  private:
  //Student must implement.
};

class Matrix
{
  public:
   Matrix();
   Matrix(const Matrix& oldMatrix);
   ~Matrix();
   Matrix* multiply(const Matrix* otherMatrix) const;
   Vector* multiply(const Vector* theVector) const;
   double* operator[](int index) const;
   static Matrix* translation(double deltaX, double deltaY);
   static Matrix* rotation(double theta);
   static Matrix* shearing(double shearXY, double shearYX);
   static Matrix* scaling(double scaleX, double scaleY);
   Matrix* getInverse() const;
  private:
   //Student must implement.
   double** data;
   double getDeterminant() const;
};


#endif
