#pragma once
class matrixTest
{
public:
	matrixTest();
	~matrixTest();
private:
	bool passTest;
	//tests for Vector
	void testVectorConstructor1(int numTests);
	void testVectorConstructorDD(int numTests);
	void testVectorConstructorV(int numTests);
	void testOverLoadedBrackets(int numTests);
	//tests for Matrix
	void testMatrixConstructor(int numTests);
	void testMatrixConstructorM(int numTests);
	void testMatrixMult(int numTests);
	void testMatrixVectorMult(int numTests);
	void testMatrixBrackets(int numTests);
	void testTranslation(int numTests);
	void testRotation(int numTests);
	void testShearing(int numTests);
	void testScaling(int numTests);
};

