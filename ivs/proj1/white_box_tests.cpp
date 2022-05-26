//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Jakub Hlava <xhlava52@stud.fit.vutbr.cz>
// $Date:       $2020-02-22
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Jakub Hlava
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"
#include <time.h>
#include <stdlib.h>

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

TEST(MatrixCreation, 1by1) {
    EXPECT_NO_THROW(Matrix());
}

TEST(MatrixCreation, 3by3) {
    EXPECT_NO_THROW(Matrix(3, 3));
}

TEST(MatrixCreation, BadSize) {
    EXPECT_ANY_THROW(Matrix(0, 3));
    EXPECT_ANY_THROW(Matrix(3, 0));
	EXPECT_ANY_THROW(Matrix(-1, -1));
}

TEST(MatrixOperations, Set) {
	Matrix* m = new Matrix(3, 3);
	EXPECT_TRUE(m->set(0, 0, 1));
	EXPECT_FALSE(m->set(5, 5, 1));

	std::vector<std::vector<double>> v = { {1,1,1},{1,1,1},{1,1,1} };
	EXPECT_TRUE(m->set(v));

	v.resize(4);
	for (std::vector<double> vect : v) {
		vect.resize(4);
	}
	EXPECT_FALSE(m->set(v));
}

TEST(MatrixOperations, Get) {
	Matrix* m = new Matrix(3, 3);
	std::vector<std::vector<double>> v = { {1,1,1},{1,1,1},{1,1,1} };
	m->set(v);
	EXPECT_EQ(m->get(1, 1), (double)1);
	EXPECT_ANY_THROW(m->get(-1, -1));
	EXPECT_ANY_THROW(m->get(5, 5));
}

TEST(MatrixOperations, SolveEquation) {
	Matrix* matrix4 = new Matrix(4, 4);
	Matrix* matrix3 = new Matrix(3, 3);
	Matrix* matrix2 = new Matrix(2, 2);
	Matrix* matrix1 = new Matrix();
	Matrix* badSize = new Matrix(3, 2);
	Matrix* singular = new Matrix(3, 3);
	
	std::vector<std::vector<double>> v4 = { {2,2,-1,1},{4,3,-1,2},{8,5,-3,4},{3,3,-2,2} };
	std::vector<std::vector<double>> v3 = { {1,2,3},{3,2,1},{3,1,2} };
	std::vector<std::vector<double>> v2 = { {5,-3}, {21,-14} };
	std::vector<std::vector<double>> vsin = { {1,2,3},{1,2,3},{1,2,3} };
	std::vector<std::vector<double>> vbad = { {5,2,27},{3,8,44} };

	std::vector<double> m4right = { 4,6,12,6 };
	std::vector<double> m3right = { 14,10,11 };
	std::vector<double> m2right = { 14,35 };
	std::vector<double> m1right = { 1 };

	matrix4->set(v4);
	matrix3->set(v3);
	matrix2->set(v2);
	matrix1->set(0, 0, 1);
	singular->set(vsin);
	
	EXPECT_ANY_THROW(matrix3->solveEquation(m2right));
	EXPECT_ANY_THROW(singular->solveEquation(m3right));
	EXPECT_ANY_THROW(badSize->solveEquation(m2right));

	std::vector<double> result4 = matrix4->solveEquation(m4right);
	std::vector<double> result3 = matrix3->solveEquation(m3right);
	std::vector<double> result2 = matrix2->solveEquation(m2right);
	std::vector<double> result1 = matrix1->solveEquation(m1right);

	std::vector<double> correct4 = { 1,1,-1,-1 };
	std::vector<double> correct3 = { 1,2,3 };
	std::vector<double> correct2 = { 13,17 };

	EXPECT_EQ(result4, correct4);
	EXPECT_EQ(result3, correct3);
	EXPECT_EQ(result2, correct2);
	EXPECT_EQ(result1[0], (double)1);
}

class Operations : public ::testing::Test {
protected:
	Matrix nullMatrix = Matrix(3, 3);
	Matrix randMatrix = Matrix(3, 3);
	Matrix unitMatrix = Matrix(3, 3);
	Matrix badSizeMatrix = Matrix(2, 2);
	Matrix testMatrix = Matrix(3, 3);
	void SetUp() override {
		for (int i = 0; i < 9; i++) {
			nullMatrix.set(i / 3, i % 3, 0);
		}
		for (int i = 0; i < 9; i++) {
			srand((unsigned)time(NULL));
			randMatrix.set(i / 3, i % 3, rand() % 100);
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (i == j) {
					unitMatrix.set(i, j, 1);
				}
				else {
					unitMatrix.set(i, j, 0);
				}
			}
		}
		for (int i = 0; i < 4; i++) {
			badSizeMatrix.set(i / 2, i % 2, 1);
		}
	}
	
};

TEST_F(Operations, Multiply) {
	testMatrix = unitMatrix * unitMatrix;
	EXPECT_EQ(testMatrix.get(0, 0), 1);
	EXPECT_EQ(testMatrix.get(0, 1), 0);
	testMatrix = unitMatrix * 2;
	EXPECT_EQ(testMatrix.get(0, 0), 2);
	testMatrix = unitMatrix * randMatrix;
	EXPECT_EQ(testMatrix.get(0, 0), randMatrix.get(0, 0));
	testMatrix = randMatrix * 10;
	EXPECT_EQ(testMatrix.get(0, 0), randMatrix.get(0, 0) * 10);
	testMatrix = randMatrix * nullMatrix;
	EXPECT_EQ(testMatrix.get(0, 0), 0);
	EXPECT_ANY_THROW(testMatrix * badSizeMatrix);
}

TEST_F(Operations, Add) {
	testMatrix = unitMatrix + unitMatrix;
	EXPECT_EQ(testMatrix.get(0, 0), 2);
	testMatrix = randMatrix + randMatrix;
	EXPECT_EQ(testMatrix.get(0, 0), randMatrix.get(0, 0)*2);
	EXPECT_ANY_THROW(testMatrix + badSizeMatrix);
}

TEST_F(Operations, Equals) {
	std::vector<std::vector<double>> content = { {1,0,0}, {0,1,0}, {0,0,1} };
	testMatrix.set(content);
	EXPECT_TRUE(testMatrix == unitMatrix);
	EXPECT_FALSE(nullMatrix == unitMatrix);
	EXPECT_ANY_THROW(testMatrix == badSizeMatrix);
}









/*** Konec souboru white_box_tests.cpp ***/