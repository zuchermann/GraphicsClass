#include "SceneTest.h"
#include "Scene.h"
#include "matrix.h"
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <math.h> 


SceneTest::SceneTest()
{
	cout << endl << "////TRANSFORM NODE////" << endl;
	//test constructor 1 in transform node
	testTNConstructor();
	//test constructor 2 in transform node
	testTNConstructor2();
	testTranslate();
}


SceneTest::~SceneTest()
{
}

bool matrixEqual(Matrix mat1, Matrix mat2) {
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			double entry1 = mat1[row][col];
			double entry2 = mat2[row][col];
			if (entry1 != entry2) {
				return false;
			}
		}
	}
	return true;
}

void SceneTest::testTNConstructor() {
	TransformNode nodeTest = TransformNode(nullptr);
	TransformNode nodeTest2 = TransformNode(&nodeTest);
	Matrix ident = Matrix();
	if (!(matrixEqual(*nodeTest.getTransform(), ident)) || !(matrixEqual(*nodeTest2.getTransform(), ident))) {
		cout << "***FAIL*** TN constructor" << endl;
		return;
	}
	if ((nodeTest.getShapeNode() != nullptr) || (nodeTest2.getShapeNode() != nullptr)) {
		cout << "***FAIL 2*** TN constructor" << endl;
		return;
	}
	if ((nodeTest.getParent() != nullptr) || (nodeTest2.getParent() != &nodeTest)) {
		cout << "***FAIL 3*** TN constructor ";
		cout << &nodeTest << " should equal " << nodeTest2.getParent() << endl;
		return;
	}
	cout << "passed tests for TransformNode() constructor" << endl;
}

void SceneTest::testTNConstructor2() {
	TransformNode nodeTest = TransformNode(nullptr, nullptr, new Matrix());
	TransformNode nodeTest2 = TransformNode(&nodeTest, nullptr, new Matrix());
	Matrix ident = Matrix();
	if (!(matrixEqual(*nodeTest.getTransform(), ident)) || !(matrixEqual(*nodeTest2.getTransform(), ident))) {
		cout << "***FAIL*** TN constructor 2" << endl;
		return;
	}
	if ((nodeTest.getShapeNode() != nullptr) || (nodeTest2.getShapeNode() != nullptr)) {
		cout << "***FAIL 2*** TN constructor 2" << endl;
		return;
	}
	if ((nodeTest.getParent() != nullptr) || (nodeTest2.getParent() != &nodeTest)) {
		cout << "***FAIL 3*** TN constructor 2. ";
		cout << &nodeTest << " should equal " << nodeTest2.getParent() << endl;
		return;
	}
	cout << "passed tests for TransformNode(*,*,*) constructor" << endl;
}

void SceneTest::testTranslate() {
	TransformNode testNode = TransformNode(nullptr);
	testNode.translate(6.0, 6.0);
	Matrix mat = *testNode.getTransform();
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			double entry = mat[row][col];
			//cout << entry << " ";
			if (((col == 2) && (row == 0)) || ((col == 2) && (row == 1))) {
				if (entry != 6.0) {
					cout << "***FAIL 1*** TN translate" << endl;
				}
			}
			else {
				if (row == col) {
					if (entry != 1.0) {
						cout << "***FAIL 2*** TN translate" << endl;
					}
				}
				else {
					if (entry != 0.0)
					{
						cout << "***FAIL 3*** TN translate. exp " << entry << " = 0.0" << endl;
					}
				}
			}
		}
		//cout << endl;
	}
	cout << "passed tests for Translate()" << endl;
}