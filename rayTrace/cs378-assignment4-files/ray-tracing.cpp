#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <fstream>
#include <cassert>
#include <list>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#include "ray-tracing.h"

double EPSILON = 0.01;
double MIN_T = 0.1;
double MAX_T = 99999;

list<Figure*> shapeList;
list<Light*> lightList;



double cameraX, cameraY, cameraZ;
int horizontalResolution, verticalResolution;
double zCoor;
double minX, maxX;
double minY, maxY;

int maxDepth;
Color backgroundColor;
Color ambient;
vector<vector<Color>> pixls;

void initPix() {
	int x = 0;
	while (x < horizontalResolution)
	{
		pixls.push_back(vector<Color>());
		int y = 0;
		while (y < verticalResolution)
		{
			pixls.at(x).push_back(backgroundColor);
			y++;
		}
		x++;
	}
}

void generatePic() {
	ofstream myfile;
	myfile.open("picture.ppm");
	myfile << "P3\n";
	myfile << horizontalResolution << " " << verticalResolution << "\n";
	myfile << 255 << "\n";
	int y = 0;
	while (y < verticalResolution)
	{
		int x = 0;
		while (x < horizontalResolution)
		{
			double redD = (pixls.at(x).at(y).getR() * 255) + 0.5;
			int redI = (int) redD;
			double greenD = (pixls.at(x).at(y).getG() * 255) + 0.5;
			int greenI = (int)greenD;
			double blueD = (pixls.at(x).at(y).getB() * 255) + 0.5;
			int blueI = (int)blueD;
			x++;
			myfile << redI << " " << greenI << " " << blueI << " \n";
		}
		y++;
	}
	myfile.close();
}

pair<double, Figure*> nearestIntersection(const Ray& r,
	double minT, double maxT, double epsilon,
	bool mayBeTransparent = true)
{
	double firstInter = 0;
	Figure* firstFig = nullptr;
	for (list<Figure*>::const_iterator iterator = shapeList.begin(), end = shapeList.end(); iterator != end; ++iterator) {
		double inter = (*iterator)->intersection(r, minT, maxT);
		if (inter > 0.0)
		{
			if (inter < firstInter || firstInter == 0) 
			{
				if (inter < maxT) {
					if (inter > minT)
					{
						if (inter > epsilon)
						{
							firstInter = inter;
							firstFig = (*iterator);
						}
					}
				}
			}
		}
	}
	return *(new pair<double, Figure*>(firstInter, firstFig));
}

Color diffuseShade(Figure* obj, Light* light, double dotProduct, double dist)
{
	Color dif = obj->getDiffuse();
	Color shade = light->getAtt(dist);
	Color product = dif.mult(shade);
	return product.scale(dotProduct);
}

Color specularShade(Figure* obj, const Vec& normal,
	Light* light, const Vec& lightDirection, double dotProduct,
	const Ray& ray, double dist)
{
	Color spec = obj->getSpec();
	Color shade = light->getAtt(dist);
	Color product = spec.mult(shade);
	return product.scale(dotProduct);
}

Color RT_lights(Figure* obj, const Ray& ray, const Vec& thePoint, const Vec& normal)
{
	Color c = *(new Color());
	for (list<Light*>::iterator iterator = lightList.begin(), end = lightList.end(); iterator != end; ++iterator) {
		//nearestIntersection(ray, MIN_T, MAX_T, EPSILON, false);
		Light* theLight = *iterator;
		Vec* toLight = thePoint.normalize(theLight->getPos());
		double dotProduct = toLight->dot(normal);
		double dist = abs((thePoint.sub(theLight->getPos()))->getMag());
		Color dif = diffuseShade(obj, theLight, max(dotProduct, 0.0), dist);
		Vec* Q = normal.scale(normal.dot(*toLight));
		Vec* S = Q->sub(*toLight);
		Vec* R = toLight->add(*(S->scale(2.0)));
		dotProduct = max(0.0, R->dot(*(ray.getNorm().scale(-1.0))));
		Color spec = specularShade(obj, normal, theLight, *toLight, pow(dotProduct, obj->getShininess()), ray, dist);
		c = c.add(dif.add(spec));
	}
	return c;
}

Color RT_shade(Figure* obj, const Ray& ray, const Vec& thePoint, const Vec& normal,
	bool entering, double depth)
{
	Color ambColor = (ambient.mult(obj->getAmbient()));
	Color shadeColor = RT_lights(obj, ray, thePoint, normal);
	//RT_reflect(obj, ray, i, normal, depth);
	//RT_transmit(obj, ray, i, normal, entering, depth);
	Color sum = shadeColor.add(ambColor);
	return sum;
}

Color RT_Trace(const Ray& r, int depth) {
	pair<double, Figure*> nearestObj = nearestIntersection(r, MIN_T, MAX_T, EPSILON);
	if (nearestObj.first > 0) {
		Vec* intersectVec = new Vec(r.calcAt(nearestObj.first));
		return RT_shade(nearestObj.second, r, *intersectVec, (nearestObj.second)->norm(*intersectVec), false, depth);
	}
	else return backgroundColor;
}

void RT_alg() {
	double pixWidth = (maxX - minX) / horizontalResolution;
	double pixHeight = (maxY - minY) / verticalResolution;

	int x = 0;
	while (x < horizontalResolution)
	{
		int y = 0;
		while (y < verticalResolution)
		{
			Vec* vec1 = new Vec(cameraX, cameraY, cameraZ);
			Vec* vec2 = new Vec((minX + ((x + 1) * pixWidth)), (minY + ((y + 1) * pixHeight)), zCoor);
			Ray* R = new Ray(*vec1, *vec2);
			pixls[x][verticalResolution - 1 - y] = RT_Trace(*R, 1);
			y++;
		}
		x++;
	}
}

Vec::Vec() : x(0.0),y(0.0),z(0.0) {}

Vec::Vec(ifstream& ifs)
{
  ifs >> x >> y >> z;
}

Vec::Vec(double ex, double wi, double zee) : x(ex), y(wi), z(zee) {}

Vec::Vec(const Vec& vec) {
	this->x = vec.x;
	this->y = vec.y;
	this->z = vec.z;
}

double Vec::dot(const Vec& vec2) const 
{
	return ((this->x * vec2.x) + (this->y * vec2.y) + (this->z * vec2.z));
}

Vec* Vec::sub(const Vec& vec2) const
{
	Vec* newVec = new Vec((this->x - vec2.x), (this->y - vec2.y), (this->z - vec2.z));
	return newVec;
}

Vec* Vec::normalize(const Vec& vec2) const
{
	Vec* dir = vec2.sub(*this);
	double mag = sqrt(pow(dir->x, 2) + pow(dir->y, 2) + pow(dir->z, 2));
	return new Vec(((dir->x) / mag), ((dir->y) / mag), ((dir->z) / mag));
}

Vec* Vec::scale(double t) const {
	return new Vec(((this->x) * t), ((this->y) * t), ((this->z) * t));
}

Vec* Vec::add(const Vec& vec2) const {
	return new Vec(((this->x) + (vec2.x)), ((this->y) + (vec2.y)), ((this->z) + (vec2.z)));
}

double Vec::getMag() const {
	return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
}

Vec Ray::calcAt(double t) const {
	Vec* unit = (this->vec2).sub(this->vec1);
	return *((unit->scale(t))->add(this->getV1()));
}


Ray::Ray()
{
	this->vec1 = *(new Vec());
	this->vec2 = *(new Vec());
}

Ray::Ray(Vec& vec1, Vec& vec2) {
	this->vec1 = vec1;
	this->vec2 = vec2;
}

Vec Ray::getV1() const 
{
	return this->vec1;
}

Vec Ray::getV2() const
{
	return this->vec2;
}

Vec Ray::getNorm() const
{
	return *(this->vec1).normalize(this->vec2);
}

Color::Color(): red(0.0), green(0.0), blue(0.0) {};

Color::Color(ifstream& ifs)
{
  ifs >> red >> green >> blue;
}

Color::Color(double r, double g, double b) : red(r), green(g), blue(b)
{}

Color::Color(const Color& c)
{
	this->red = c.red;
	this->green = c.green;
	this->blue = c.blue;
}

double Color::getR() {
	return this->red;
}

double Color::getG() {
	return this->green;
}

double Color::getB() {
	return this->blue;
}

Color Color::mult(const Color color2) const {
	return *(new Color((color2.red * this->red), (color2.green * this->green), (color2.blue * this->blue)));
}

Color Color::add(const Color color2) const {
	return *(new Color((color2.red + this->red), (color2.green + this->green), (color2.blue + this->blue)));
}

Color Color::scale(double dp) const {
	return *(new Color((dp * this->red), (dp * this->green), (dp * this->blue)));
}

void Color::cut() {
	this->red = min(1.0, red);
	this->green = min(1.0, green);
	this->blue = min(1.0, blue);
}

Figure::Figure(){}

void Figure::initFigure(ifstream& ifs)
{
 ambient = Color(ifs);
 diffuse = Color(ifs);
 specular = Color(ifs);
 reflectivity = Color(ifs);
 transmissivity = Color(ifs);
 ifs >> shininess >> indexOfRefraction >> rFlag >> tFlag;
}

Color Figure::getAmbient() {
	return this->ambient;
}

Color Figure::getDiffuse() {
	return this->diffuse;
}

Color Figure::getSpec() {
	return this->specular;
}

double Figure::intersection(const Ray& r, double minT, double maxT) const
{
	return 0;
}

Vec Figure::norm(const Vec vec) const {
	return *(new Vec());
}

double Figure::getShininess() const {
	return this->shininess;
}

Light::Light(ifstream& ifs) : position(ifs), shading(ifs)
{
  ifs >> c0 >> c1 >> c2;
}

Vec Light::getPos() const {
	return *(new Vec(this->position));
}

Color Light::getShading() const {
	return *(new Color(this->shading));
}

Color Light::getAtt(double dist) const {
	double att = this->c0 + (this->c1 * dist) + (this->c2 * pow(dist, 2));
	return *(new Color(this->shading.scale(att)));
}

Sphere::Sphere(ifstream& ifs) : center(ifs)
{
  ifs >> radius;
  initFigure(ifs);
}

double Sphere::intersection(const Ray& r, double minT, double maxT) const
{
	Vec P0 = r.getV1();
	Vec P1 = r.getV2();
	Vec h = *(P1.sub(P0));
	Vec k = *(P0.sub(this->center));
	double a = h.dot(h);
	double b = (2 * (h.dot(k)));
	double c = (k.dot(k) - pow((this->radius), 2.0));
	double discriminant = (pow(b, 2) - ((4 * a) * c));
	double a2 = 2 * a;
	if (discriminant <= 0 || a2 == 0) {
		return 0;
	}
	double t0 = (((-1 * b) + sqrt(discriminant)) / a2);
	double t1 = (((-1 * b) - sqrt(discriminant)) / a2);
	if (t0 < maxT || t1 < maxT) 
	{
		if (t0 > minT || t1 > minT)
		{
			if (t0 < t1)
			{
				if (t0 > minT) return t0;
				else
				{
					if (t1 < maxT) return t1;
					else return 0;
				}
			}
			else
			{
				if (t1 > minT) return t1;
				else {
					if (t0 < maxT) return t0;
					else return 0;
				}
			}
		}
		else return 0;
	}
	else return 0;
}

Vec Sphere::norm(const Vec vec) const {
	return *(this->center.normalize(vec));
}


Plane::Plane(ifstream& ifs) : abcVector(ifs)
{
  ifs >> dScalar;
  initFigure(ifs);
  direction1 = Vec(ifs);
  direction2 = Vec(ifs);
}

double Plane::intersection(const Ray& r, double minT, double maxT) const
{
	return 0;
}

Vec Plane::norm(const Vec vec) const {
	return *(new Vec());
}

void parseSceneFile(char* sceneName)
{
  double bgr, bgg, bgb;
  double ar, ag, ab;
  ifstream ifs;
  assert (sceneName != 0);
  ifs.open(sceneName);
  ifs >> cameraX;
  ifs >> cameraY;
  ifs >> cameraZ;
  ifs >> zCoor;
  ifs >> minX >> maxX;
  ifs >> minY >> maxY;
  ifs >> bgr >> bgg >> bgb;
  backgroundColor = Color(bgr,bgg,bgb);
  ifs >> ar >> ag >> ab;
  ambient = Color(ar,ag,ab);
  ifs >> maxDepth;
  ifs >> horizontalResolution >> verticalResolution;
  int numLights, numSpheres, numPlanes;
  ifs >> numLights;
  for (int i=0; i<numLights; ++i) lightList.push_front(new Light(ifs));
  ifs >> numSpheres;
  for (int i=0; i<numSpheres; ++i) shapeList.push_front(new Sphere(ifs));
  ifs >> numPlanes;
  for (int i=0; i<numPlanes; ++i) shapeList.push_front(new Plane(ifs));
  ifs.close();
}


int main(int, char *argv[])
{
    parseSceneFile(argv[1]);
	initPix();
	RT_alg();
	generatePic();
    return 0;
}

