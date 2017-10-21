// -*- C++ -*-
#ifndef FILESPLITTERTEST_H
#define FILESPLITTERTEST_H

#include <set>
#include <fstream>
#include <iostream>

#include "Relation.h"
#include "Calculus.h"
#include "CalculusReader.h"

#include "FileSplitter.h"

#include "TestSuite.h"


class FileSplitterTest {
	private:

	Calculus* allen;
	FileSplitter* splitter;

	public:

	void setUp() {
		allen = NULL;
		splitter = NULL;

		std::ifstream i;
		i.open("./data/allen.spec");
		TS_ASSERT(i.is_open());
		if (!i.is_open()) return;

		const std::string name = "allen";
		const std::string dir = "./data/";
	        CalculusReader r = CalculusReader(name, dir, &i);
	        allen = r.makeCalculus();
		i.close();

		i.open("./data/allen/calculus/hornalg");
		if (!i.is_open()) return;

		splitter = new FileSplitter(*allen, &i);
		i.close();

	}

	void tearDown() {
		delete splitter;
		delete allen;
	}


	void testConstructor( void ) {
		TS_ASSERT(allen);
		TS_ASSERT(splitter);
	}

	void testSplitBaseRelations( void ) {
		if (allen == NULL || splitter == NULL) return;

		// base relations
		for (size_t i = 0; i < 13; i++) {
			Relation b;
			b.set(i);
			TS_ASSERT(splitter->isSplit(b));

			const Relation t = splitter->getFirstSplit(b);
			TS_ASSERT_EQUALS(b, t);
		}
	}

	void testSplitBaseRelations( void ) {
		if (allen == NULL || splitter == NULL) return;

		for (size_t i = 0; i < 13; i++) {
			Relation b;
			b.set(i);
			TS_ASSERT(splitter->isSplit(b));

			const Relation t = splitter->getFirstSplit(b);
			TS_ASSERT_EQUALS(b, t);
		}
	}

	void testSplitBaseRelations( void ) {
		if (allen == NULL || splitter == NULL) return;

		// TODO: improve

		// EMPTY
		TS_ASSERT(splitter->isSplit(Relation()));
		TS_ASSERT_EQUALS(splitter->getFirstSplit(Relation()), Relation());

		// UNIVERSAL
		const Relation u = allen->getUniversalRelation();
		TS_ASSERT(splitter->isSplit(u));
		TS_ASSERT_EQUALS(splitter->getFirstSplit(u), u);

		const Relation inequal = allen->encodeRelation("< >");
		TS_ASSERT(!splitter->isSplit(inequal));
		TS_ASSERT(splitter->isSplit(
			splitter->getFirstSplit(inequal)
			)
		);
		TS_ASSERT(splitter->isSplit(allen->encodeRelation("< m s")));
	}
};
#endif //FILESPLITTERTEST_H
