// -*- C++ -*-
#ifndef CARTESIAN_PRODUCT_SPLITTER_TEST_H
#define CARTESIAN_PRODUCT_SPLITTER_TEST_H

#define SILENT_PRECOMPUTATION_SETUP
#define DEFAULT_MEMLIMIT_PRECOMP_COMPOSITION 0
#define DEFAULT_MEMLIMIT_PRECOMP_CONVERSES 0

#include <string>
#include <vector>
#include <fstream>

#include "RelationFixedBitset.h"
#include "Calculus.h"
#include "CombinedCalculusReader.h"
#include "FileSplitter.h"
#include "CartesianProductSplitter.h"

#include "WorldFixture.h"
#include "TestSuite.h"


#ifdef Relation
#undef Relation
#endif

#ifdef NR_OF_WORDS
#undef NR_OF_WORDS
#endif

#define Relation RelationFixedBitset<uint32_t, 1>


Splitter<Relation>* getFileSplitter(const Calculus<Relation>* calculus, const std::string calcName, const std::string subclassName) {
    std::string splitFilename =  "./data/" + calcName + "/calculus/" + subclassName + "alg";

    std::ifstream* f = new std::ifstream();
    f->open(splitFilename.c_str());
    if (! f->is_open()) {
	std::cerr << "Failed to open file " << splitFilename << std::endl;
	return NULL;
    }
    Splitter<Relation>* result = new FileSplitter<Relation>(calculus, f);
    f->close();
    delete f;
    return result;
}

class CartesianProductSplitterTest : public CxxTest::TestSuite {
    Calculus<Relation>* rcc8_point;
    Calculus<Relation>* point3D;
    std::vector<std::vector<Relation> > projectionsp3D;
    std::vector<Splitter<Relation>* > file_splittersp3D;
    Splitter<Relation>* splitterp3D;
    std::vector<std::vector<Relation> > projections;
    std::vector<Splitter<Relation>* > file_splitters;
    Splitter<Relation>* splitter;

  public:
    void setUp() {
        Relation::init();

	// RCC8Point
	std::vector<std::string> dataDirs;
	dataDirs.push_back("./data");
	CombinedCalculusReader<Relation> rcc8_point_read("rcc8+point", dataDirs);
	rcc8_point = rcc8_point_read.makeCalculus(0,0);
	TS_ASSERT(rcc8_point != NULL);
	projections = rcc8_point_read.getProjections();
	Splitter<Relation>* sp1 = getFileSplitter(rcc8_point_read.getCalculus(0).second, rcc8_point_read.getCalculus(0).first, "horn");
	Splitter<Relation>* sp2 = getFileSplitter(rcc8_point_read.getCalculus(1).second, rcc8_point_read.getCalculus(1).first, "all");

	TS_ASSERT(sp1 != NULL);
	TS_ASSERT(sp2 != NULL);

	file_splitters.push_back(sp1);
	file_splitters.push_back(sp2);
	splitter = new CartesianProductSplitter<Relation>(rcc8_point, projections, file_splitters);

	TS_ASSERT(splitter != NULL);

	// 3D Point
	CombinedCalculusReader<Relation> point3D_read("point+point+point", dataDirs);
	point3D = point3D_read.makeCalculus(0,0);

	TS_ASSERT(point3D != NULL);
	projectionsp3D = point3D_read.getProjections();

	Splitter<Relation>* sp = getFileSplitter(point3D_read.getCalculus(0).second, point3D_read.getCalculus(0).first, "all");
	TS_ASSERT(sp != NULL);
	file_splittersp3D.push_back(sp);
	sp = getFileSplitter(point3D_read.getCalculus(1).second, point3D_read.getCalculus(1).first, "all");
	TS_ASSERT(sp != NULL);
	file_splittersp3D.push_back(sp);
	sp = getFileSplitter(point3D_read.getCalculus(2).second, point3D_read.getCalculus(2).first, "all");
	TS_ASSERT(sp != NULL);
	file_splittersp3D.push_back(sp);
	splitterp3D = new CartesianProductSplitter<Relation>(point3D, projectionsp3D, file_splittersp3D);

	TS_ASSERT(splitterp3D != NULL);
    }

    void tearDown() {
        Relation::clean_up();

	delete splitter;
	delete rcc8_point;
	delete splitterp3D;
	delete point3D;
	file_splitters.clear();
	file_splittersp3D.clear();
    }

    void testBaseRelations( void ) {
	TS_ASSERT(rcc8_point->getNumberOfBaseRelations() == 14);
	for (size_t i = 0; i < rcc8_point->getNumberOfBaseRelations(); i++) {
	    Relation baserel;
	    baserel.set(i);
	    TS_ASSERT(splitter->isSplit(baserel));
	    std::set<Relation>* split = splitter->split(baserel);
	    TS_ASSERT(split->size() == 1);
	    if (!split->empty())
		TS_ASSERT_EQUALS(*split->begin(), baserel);
	    delete split;
	}
	TS_ASSERT(point3D->getNumberOfBaseRelations() == 27);
	for (size_t i = 0; i < point3D->getNumberOfBaseRelations(); i++) {
	    Relation baserel;
	    baserel.set(i);
	    TS_ASSERT(splitterp3D->isSplit(baserel));
	    std::set<Relation>* split = splitterp3D->split(baserel);
	    TS_ASSERT(split->size() == 1);
	    if (!split->empty())
		TS_ASSERT_EQUALS(*split->begin(), baserel);
	    delete split;
	}
    }

    void testAllRelations( void ) {
	for (size_t i = 1; i < (1 << rcc8_point->getNumberOfBaseRelations()); i++) {
	    Relation relation(i);
	    std::set<Relation>* split = splitter->split(relation);
	    TS_ASSERT(split != NULL);
	    if (split != NULL) {
		Relation assemble;
		for (std::set<Relation>::const_iterator it = split->begin(); it != split->end(); it++) {
		    TS_ASSERT(*it != (Relation(0u)));
		    TS_ASSERT(rcc8_point->refinesRelation(*it, relation));
		    assemble |= *it;
		    // assert *it to be of Cartesian product form
		    Relation projection1;
		    Relation projection2;
		    for (size_t j = 0; j < rcc8_point->getNumberOfBaseRelations(); j++)
			if ((*it)[j]) {
			    projection1 |= projections[j][0];
			    projection2 |= projections[j][1];
			}
		    TS_ASSERT(file_splitters[0]->isSplit(projection1));
		    TS_ASSERT(file_splitters[1]->isSplit(projection2));
		    // so far *it <= S_1 x S_2
		    // assert equality: compl(*it) \cap S_1 x S_2 == \emptyset
		    for (size_t j = 0; j < rcc8_point->getNumberOfBaseRelations(); j++)
			if (!(*it)[j]) {
			   TS_ASSERT( (projections[j][0] & projection1).none() ||
				(projections[j][1] & projection2).none());
			}
		}
		TS_ASSERT(assemble == relation);
	    }
	    delete split;
	}
	for (size_t i = 1; i < (1 << point3D->getNumberOfBaseRelations()); i++) {
	    /** @todo this test is too slow */
	    // workaround: skip some relations
	    if (i%10000 != 0)
		continue;

	    Relation relation(i);
	    std::set<Relation>* split = splitterp3D->split(relation);
	    TS_ASSERT(split != NULL);
	    if (split != NULL) {
		Relation assemble;
		for (std::set<Relation>::const_iterator it = split->begin(); it != split->end(); it++) {
		   TS_ASSERT(point3D->refinesRelation(*it, relation));
		    assemble |= *it;
		    /** @todo assert *it to be of Cartesian product form */
		    Relation projection1;
		    Relation projection2;
		    Relation projection3;
		    for (size_t j = 0; j < point3D->getNumberOfBaseRelations(); j++)
			if ((*it)[j]) {
			    projection1 |= projectionsp3D[j][0];
			    projection2 |= projectionsp3D[j][1];
			    projection3 |= projectionsp3D[j][2];
			}
		   TS_ASSERT(file_splittersp3D[0]->isSplit(projection1));
		   TS_ASSERT(file_splittersp3D[1]->isSplit(projection2));
		   TS_ASSERT(file_splittersp3D[2]->isSplit(projection3));
		    // so far *it <= S_1 x S_2 x S_3
		    // assert equality: compl(*it) \cap S_1 x S_2 x S_3 == \emptyset
		    for (size_t j = 0; j < point3D->getNumberOfBaseRelations(); j++)
			if (!(*it)[j]) {
			   TS_ASSERT( (projectionsp3D[j][0] & projection1).none() ||
				(projectionsp3D[j][1] & projection2).none() ||
				(projectionsp3D[j][2] & projection3).none());
			}
		}
		TS_ASSERT(assemble == relation);
	    }
	    delete split;
	}
    }
};
#endif // CARTESIAN_PRODUCT_SPLITTER_TEST_H
