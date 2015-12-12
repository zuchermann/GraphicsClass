#include <list>
using namespace std;

class Color;
class Light;
class Figure;
class Plane;
class Sphere;

class Vec
{
  private:
    double x;
    double y;
    double z;

  public:
	~Vec();
    Vec();
    Vec(ifstream& ifs);
	Vec(double ex, double wi, double zee);
	Vec(const Vec& vec2);
	double dot(const Vec& vec2) const;
	Vec* sub(const Vec& vec2) const;
	Vec* normalize(const Vec& vec2) const;
	Vec* scale(double t) const;
	Vec* add(const Vec& vec2) const;
	double getMag() const;
	double getX() const;
	double getY() const;
	double getZ() const;
};

class Ray
{
private:
	Vec vec1;
	Vec vec2;
	Figure* inside;
public:
	~Ray();
	Ray();
	Ray(Vec& vec1, Vec& vec2);
	Vec getV1() const;
	Vec getV2() const;
	Vec calcAt(double t) const;
	Vec getNorm() const;
	Figure* isInside() const;
	void setInside(Figure* isIn);
};


class Color
{
  friend Color operator*(double num, const Color& c);

  protected:
   double red, green, blue;

  public:
	~Color();
	Color();
    Color(ifstream& ifs);
    Color(double r, double g, double b);
	Color(const Color& c);
	double getR();
	double getG();
	double getB();
	Color mult(const Color color2) const;
	Color add(const Color color2) const;
	Color scale(double dp) const;
	Color cut() const;
};

class Light
{
  public:
	~Light();
    Light(ifstream& ifs);
	Color getShading() const;
	Vec getPos() const;
	Color getAtt(double dist) const;
  private:
    Vec position;
    Color shading;
    double c0, c1, c2;
};


class Figure
{
 protected:
   Color ambient;
   Color diffuse;
   Color specular;
   Color reflectivity;
   Color transmissivity;
   double shininess;
   double indexOfRefraction;
   int rFlag, tFlag;

 public:
   ~Figure();
   Figure();
   virtual double intersection(const Ray& r, double minT, double maxT) const;
   virtual Vec norm(const Vec vec) const;
   void initFigure(ifstream& ifs);
   Color getAmbient();
   Color getDiffuse();
   Color getSpec();
   Color getRef();
   double getIOR();
   bool isR();
   bool isT();
   double getShininess() const;
};



class Plane : public Figure
{
  private:
    Vec abcVector;
    double dScalar;
    Vec direction1;
    Vec direction2;
  public:
	~Plane();
    Plane(ifstream& ifs);
	double intersection(const Ray& r, double minT, double maxT) const;
	Vec norm(const Vec vec) const;
};

class Sphere : public Figure
{
  private:
    Vec center;
    double radius;
  public:
	~Sphere();
    Sphere(ifstream& ifs);
	double intersection(const Ray & r, double minT, double maxT) const;
	Vec norm(const Vec vec) const;
};

