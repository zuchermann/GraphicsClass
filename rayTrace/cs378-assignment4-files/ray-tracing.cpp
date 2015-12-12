/*
Ray-Tracing.
Generates 3d images via ray-tracing.

NOTE: I mistakenly implemented all of the methods in Vec
to have return type Vec*. This forced me to have a lot of 
calls to Vec's delete() method throughout the code to prevent
memory leaks.

NOTE: This program takes a very long time to render the file
"scene.17" (around 20 minutes on my machine). 

Zach Kondak
*/

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
double SHAD_RES = 10.0;
double SPACE_INDEX = 1.0;

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

//initializes pixMap
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

//Generate a ppm file from the pixls
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

//sorry about the nested ifs. They made sense before the program got really big
//finds nearest obj. Will return transparent objs only when mayBeTransparent = true
pair<double, Figure*> nearestIntersection(const Ray& r,
	double minT, double maxT, double epsilon,
	bool mayBeTransparent = true)
{
	double firstInter = 0;
	Figure* firstFig = nullptr;
	for (list<Figure*>::iterator iterator = shapeList.begin(), end = shapeList.end(); iterator != end; ++iterator) {
		double inter = (*iterator)->intersection(r, minT, maxT);
		if (inter > 0.0)
		{
			if (inter < firstInter || firstInter == 0) 
			{
				if (inter < maxT) {
					if (inter > minT)
					{
						if (inter > epsilon && (mayBeTransparent || !(*iterator)->isT()))
						{
							firstInter = inter;
							firstFig = (*iterator);
						}
					}
				}
			}
		}
	}
	return pair<double, Figure*>(firstInter, firstFig);
}

//calculates diffuse shading
Color diffuseShade(Figure* obj, Light* light, double dotProduct, double dist)
{
	Color dif = obj->getDiffuse();
	Color shade = light->getAtt(dist);
	Color product = dif.mult(shade);
	return product.scale(dotProduct);
}

//calculates specular shading
Color specularShade(Figure* obj, const Vec& normal,
	Light* light, const Vec& lightDirection, double dotProduct,
	const Ray& ray, double dist)
{
	Color spec = obj->getSpec();
	Color shade = light->getAtt(dist);
	Color product = spec.mult(shade);
	return product.scale(dotProduct);
}

//alg for calculating shading. Calls diffuseShade and SpecularShade
Color RT_lights(Figure* obj, const Ray& ray, const Vec& thePoint, const Vec& normal)
{
	Color c = Color();
	for (list<Light*>::iterator iterator = lightList.begin(), end = lightList.end(); iterator != end; ++iterator) {
		Light* theLight = *iterator;

		pair<double, Figure*> inter = nearestIntersection(Ray(Vec(thePoint), theLight->getPos()), MIN_T / SHAD_RES, 1.0, EPSILON, false);
		if (inter.first <= 0) {
			Vec* toLight = thePoint.normalize(theLight->getPos());
			double dotProduct = toLight->dot(normal);
			Vec* subt = (thePoint.sub(theLight->getPos()));
			double dist = abs(subt->getMag());
			Color dif = diffuseShade(obj, theLight, max(dotProduct, 0.0), dist);
			Vec* Q = normal.scale(normal.dot(*toLight));
			Vec* S = Q->sub(*toLight);
			Vec* S2 = S->scale(2.0);
			Vec* R = toLight->add(*S2);
			Vec* norm = ray.getNorm();
			Vec* scaledNorm = norm->scale(-1.0);
			dotProduct = max(0.0, R->dot(*scaledNorm));
			Color spec = specularShade(obj, normal, theLight, *toLight, pow(dotProduct, obj->getShininess()), ray, dist);
			c = c.add(dif.add(spec));
			delete(toLight);
			delete(Q);
			delete(S);
			delete(R);
			delete(S2);
			delete(subt);
			delete(norm);
			delete(scaledNorm);
		}
	}
	return c;
}

//stub. body found bellow
Color RT_Trace(const Ray& r, int depth);

//calculates reflections. Recursively calls RT_Trace
Color RT_reflect(Figure* obj, const Ray& ray, const Vec& i, const Vec& normal, double depth)
{
	if (obj->isR()) {
		Vec* V = ray.getV2().normalize(ray.getV1());
		Vec* Q = normal.scale(normal.dot(*V));
		Vec* S = Q->sub(*V);
		Vec* S2 = S->scale(2.0);
		Vec* R = V->add(*S2);
		Vec* inter = new Vec(i);
		Vec* dest = R->add(*inter);
		Ray* theRay = new Ray(*inter, *dest);
		Color newColor = RT_Trace(*theRay, depth + 1).mult(obj->getRef());
		delete(V);
		delete(Q);
		delete(S);
		delete(S2);
		delete(R);
		delete(inter);
		delete(dest);
		delete(theRay);
		return newColor;
	}
	else return Color(0, 0, 0);
}

//calctulates refractions. Recursively calls RT_Trace
Color RT_transmit(Figure* obj, const Ray& ray, const Vec& i, const Vec& normal,
	bool inside, double depth)
{
	Color returnColor = Color(0, 0, 0);
	if (obj->isT()) {
		double ratio = 0;
		Ray transmittedRay = Ray();
		double dir = 1.0;
		if (!inside) {
			ratio = SPACE_INDEX / obj->getIOR();
		}
		else {
			ratio = obj->getIOR() / SPACE_INDEX;
			dir = -1.0;
		}
		Vec* norm = normal.scale(dir);
		Vec* V = ray.getV2().normalize(ray.getV1());
		double dp = norm->dot(*V);
		double rsqrd = pow(ratio, 2.0);
		double coeff = (ratio * dp) - pow(((1 - rsqrd) + (rsqrd * pow(dp, 2.0))), 0.5);
		Vec* scaledN = norm->scale(coeff);
		Vec* rV = V->scale(ratio);
		Vec* T = scaledN->sub(*rV);
		Vec* dest = i.add(*T);
		Vec* iCopy = new Vec(i);
		Ray* tranRay = new Ray(*iCopy, *dest);
		if (inside) {
			tranRay->setInside(nullptr);
		}
		else tranRay->setInside(obj);
		returnColor = RT_Trace(*tranRay, depth + 1);
		delete(scaledN);
		delete(rV);
		delete(V);
		delete(T);
		delete(dest);
		delete(iCopy);
		delete(tranRay);
		delete(norm);
	}
	return returnColor;
}

//Calculates the pixel color by summing the resulring colors from RT_lights, RT_reflect, RT_transmit.
Color RT_shade(Figure* obj, const Ray& ray, const Vec& thePoint, const Vec& normal,
	bool inside, double depth)
{
	Color ambColor = (ambient.mult(obj->getAmbient()));
	Color shadeColor = RT_lights(obj, ray, thePoint, normal);
	Color reflect = RT_reflect(obj, ray, thePoint, normal, depth);
	Color trans = RT_transmit(obj, ray, thePoint, normal, inside, depth);
	Color sum = shadeColor.add(ambColor.add(reflect.add(trans)));
	return sum;
}

//Where the tracing happens. Calls RT_shade if an object is hit by the ray.
Color RT_Trace(const Ray& r, int depth) {
	if (depth < maxDepth + 1) {
		pair<double, Figure*> nearestObj = nearestIntersection(r, MIN_T, MAX_T, EPSILON);
		if (nearestObj.first > 0) {
			Vec* intersectVec = new Vec(r.calcAt(nearestObj.first));
			bool isIn = false;
			if (r.getInside() != nullptr) isIn = true;
			Vec* norm = (nearestObj.second)->norm(*intersectVec);
			Color returnColor = RT_shade(nearestObj.second, r, *intersectVec, *norm, isIn, depth).cut();
			delete(norm);
			delete(intersectVec);
			return returnColor;
		}
		else return backgroundColor;
	}
	else return Color(0.0, 0.0, 0.0);
}

//Makes initial (first order) calls to RT_Trace. 
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
			delete(R);
			delete(vec1);
			delete(vec2);
			y++;
		}
		x++;
	}
}

//***CLASSES START HERE***//

//
//**Vec
//
Vec::~Vec(){}

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

//normalize takes two vectors. Will automatically adjust 
//vectors so that the first is located at the origin.
Vec* Vec::normalize(const Vec& vec2) const
{
	Vec* dir = vec2.sub(*this);
	double mag = sqrt(pow(dir->x, 2) + pow(dir->y, 2) + pow(dir->z, 2));
	Vec* newVec = new Vec(((dir->x) / mag), ((dir->y) / mag), ((dir->z) / mag));
	delete(dir);
	return newVec;
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


double Vec::getX() const {
	return x;
}

double Vec::getY() const {
	return y;
}

double Vec::getZ() const {
	return z;
}

//
//**Ray
//
Ray::~Ray() {}

Ray::Ray()
{
	this->inside = nullptr;
	this->vec1 = Vec();
	this->vec2 = Vec();
}

Ray::Ray(Vec& vec1, Vec& vec2) {
	this->inside = nullptr;
	this->vec1 = vec1;
	this->vec2 = vec2;
}

//calculates the point along the ray at time t
Vec Ray::calcAt(double t) const {
	Vec* unit = (this->vec2).sub(this->vec1);
	Vec* scaled = (unit->scale(t));
	Vec* sum = scaled->add(this->getV1());
	Vec returnVec = Vec(*sum);
	delete(unit);
	delete(scaled);
	delete(sum);
	return returnVec;
}

Vec Ray::getV1() const 
{
	return this->vec1;
}

Vec Ray::getV2() const
{
	return this->vec2;
}

Vec* Ray::getNorm() const
{
	return (this->vec1).normalize(this->vec2);
}

Figure* Ray::getInside() const
{
	return inside;
}

void Ray::setInside(Figure* inside) {
	this->inside = inside;
}

//
//Color
//

Color:: ~Color() {}

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
	return Color((color2.red * this->red), (color2.green * this->green), (color2.blue * this->blue));
}

Color Color::add(const Color color2) const {
	return  Color((color2.red + this->red), (color2.green + this->green), (color2.blue + this->blue));
}

Color Color::scale(double dp) const {
	return Color((dp * this->red), (dp * this->green), (dp * this->blue));
}

Color Color::cut() const{
	return Color(min(1.0, red), min(1.0, green), min(1.0, blue));
}

//
//Light
//

Light::~Light() {}

Light::Light(ifstream& ifs) : position(ifs), shading(ifs)
{
	ifs >> c0 >> c1 >> c2;
}

Vec Light::getPos() const {
	return Vec(this->position);
}

Color Light::getShading() const {
	return Color(this->shading);
}

//attenuate light
Color Light::getAtt(double dist) const {
	double att = this->c0 + (this->c1 * dist) + (this->c2 * pow(dist, 2));
	return Color(this->shading.scale(att));
}

//
//Figure
//

Figure::~Figure() {}

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

Vec* Figure::norm(const Vec vec) const {
	return new Vec();
}

double Figure::getShininess() const {
	return this->shininess;
}

Color Figure::getRef() {
	return this->reflectivity;
}

bool Figure::isR() {
	return (this->rFlag != 0);
}

bool Figure::isT() {
	return (this->tFlag != 0);
}

double Figure::getIOR() {
	return indexOfRefraction;
}

//
//Sphere
//

Sphere::~Sphere() {};

Sphere::Sphere(ifstream& ifs) : center(ifs)
{
  ifs >> radius;
  initFigure(ifs);
}

//calculates the nearest point of interesction of ray r with this sphere 
double Sphere::intersection(const Ray& r, double minT, double maxT) const
{
	Vec P0 = r.getV1();
	Vec P1 = r.getV2();
	Vec* h = P1.sub(P0);
	Vec* k = P0.sub(this->center);
	double a = h->dot(*h);
	double b = (2 * (h->dot(*k)));
	double c = (k->dot(*k) - pow((this->radius), 2.0));
	double discriminant = (pow(b, 2) - ((4 * a) * c));
	double a2 = 2 * a;
	delete(h);
	delete(k);
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

Vec* Sphere::norm(const Vec vec) const {
	return this->center.normalize(vec);
}

//
//Plane
//

Plane::~Plane() {};

Plane::Plane(ifstream& ifs) : abcVector(ifs)
{
  ifs >> dScalar;
  initFigure(ifs);
  direction1 = Vec(ifs);
  direction2 = Vec(ifs);
}

double Plane::intersection(const Ray& r, double minT, double maxT) const
{
	Vec P0 = r.getV1();
	Vec P1 = r.getV2();
	double t = 0.0;
	Vec* subt = P1.sub(P0);
	double deNume = (subt)->dot(abcVector);
	delete(subt);
	if (deNume != 0.0) {
		double nume = (dScalar - P0.dot(abcVector));
		double temp = nume / deNume;
		if (temp > minT && temp < maxT) {
			t = temp;
		}
	}

	return t;
}

Vec* Plane::norm(const Vec vec) const {
	//return *Vec().normalize(abcVector);
	return abcVector.scale(-1.0);
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

