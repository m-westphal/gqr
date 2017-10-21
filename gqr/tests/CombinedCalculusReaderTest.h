// -*- C++ -*-
#ifndef COMBINED_CALCULUS_READER_TEST_H
#define COMBINED_CALCULUS_READER_TEST_H


#include <iostream>

#include "Relation.h"
#include "Calculus.h"
#include "CombinedCalculusReader.h"

#include "WorldFixture.h"
#include "TestSuite.h"

class CombinedCalculusReaderTest : public CxxTest::TestSuite {

  public:
    void setUp() { }

    void tearDown() { }

    void test3DPoint( void ) {
	std::ifstream stream;

	const std::string filename = "./data/point+point+point.combination";
	stream.open(filename.c_str());
	TS_ASSERT(stream.is_open());

	CombinedCalculusReader point_3D_read("point_3d", "./data", &stream);

	Calculus* point_3D = point_3D_read.makeCalculus();
	TS_ASSERT(point_3D != NULL);
	TS_ASSERT((point_3D != NULL) && point_3D->getNumberOfBaseRelations() == 3*3*3);
	TS_ASSERT((point_3D != NULL) && point_3D->checkConverseTable());
	TS_ASSERT((point_3D != NULL) && point_3D->checkCompositionTable());
	delete point_3D;
    }

    void testRCC8Point( void ) {
	std::ifstream stream;

	const std::string filename = "./data/rcc8+point.combination";
	stream.open(filename.c_str());
	TS_ASSERT(stream.is_open());

	CombinedCalculusReader rcc8_point_read("rcc8+point", "./data", &stream);

	Calculus* rcc8_point = rcc8_point_read.makeCalculus();
	TS_ASSERT(rcc8_point != NULL);
	TS_ASSERT((rcc8_point != NULL) && rcc8_point->getNumberOfBaseRelations() == 14);
	TS_ASSERT((rcc8_point != NULL) && rcc8_point->checkConverseTable());
	TS_ASSERT((rcc8_point != NULL) && rcc8_point->checkCompositionTable());
	delete rcc8_point;
    }

    void testRectangleAlgebra( void ) {
	std::ifstream stream;

	const std::string filename = "./data/allen+allen.combination";
	stream.open(filename.c_str());
	TS_ASSERT(stream.is_open());

	CombinedCalculusReader rectangle_algebra_read("allen+allen", "./data", &stream);
	Calculus* rectangle_algebra = rectangle_algebra_read.makeCalculus();
	TS_ASSERT(rectangle_algebra != NULL);
	TS_ASSERT((rectangle_algebra != NULL) && rectangle_algebra->getNumberOfBaseRelations() == 13*13);
	TS_ASSERT((rectangle_algebra != NULL) && rectangle_algebra->checkConverseTable());
	/** @todo this takes too long: */
//	TS_ASSERT((rectangle_algebra != NULL) && rectangle_algebra->checkCompositionTable());
	delete rectangle_algebra;
    }
};
#endif // COMBINED_CALCULUS_READER_TEST_H
