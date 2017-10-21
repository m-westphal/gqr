// -*- C++ -*-
#ifndef CALCULUS_READER_TEST_H
#define CALCULUS_READER_TEST_H

#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "Calculus.h"
#include "CalculusReader.h"

#include "TestSuite.h"

class CalculusReaderTest : public CxxTest::TestSuite {

  private:
    std::vector<std::string>* allenConverses;

  public:

    void setUp() {
        Relation::init();

        allenConverses = new std::vector<std::string>;

        allenConverses->push_back(std::string("="));  allenConverses->push_back(std::string("="));
        allenConverses->push_back(std::string("<"));  allenConverses->push_back(std::string(">"));
        allenConverses->push_back(std::string(">"));  allenConverses->push_back(std::string("<"));
        allenConverses->push_back(std::string("d"));  allenConverses->push_back(std::string("di"));
        allenConverses->push_back(std::string("di")); allenConverses->push_back(std::string("d"));
        allenConverses->push_back(std::string("o"));  allenConverses->push_back(std::string("oi"));
        allenConverses->push_back(std::string("oi")); allenConverses->push_back(std::string("o"));
        allenConverses->push_back(std::string("m"));  allenConverses->push_back(std::string("mi"));
        allenConverses->push_back(std::string("mi")); allenConverses->push_back(std::string("m"));
        allenConverses->push_back(std::string("s"));  allenConverses->push_back(std::string("si"));
        allenConverses->push_back(std::string("si")); allenConverses->push_back(std::string("s"));
        allenConverses->push_back(std::string("f"));  allenConverses->push_back(std::string("fi"));
        allenConverses->push_back(std::string("fi")); allenConverses->push_back(std::string("f"));
    }

    void tearDown() {
        Relation::clean_up();

        delete allenConverses;
    }

    void testReadCalculus( void ) {
	std::ifstream i;
	i.open("./data/rcc5.spec");
	TS_ASSERT(i.is_open());
	if (!i.is_open()) return;

	const std::string name1 = "rcc5";
	const std::string dir = "./data/";
        CalculusReader r = CalculusReader(name1, dir, &i);
        Calculus* t = r.makeCalculus();
	TS_ASSERT(t);
	delete t;
	i.close();

	i.open("./data/allen.spec");
	const std::string name2 = "allen";
	TS_ASSERT(i.is_open());
	if (!i.is_open()) return;
        CalculusReader r2 = CalculusReader(name2, dir, &i);
        t = r2.makeCalculus();

	TS_ASSERT(t);
	delete t;
	i.close();
    }

    void testRCC5( void )
    {
	std::ifstream i;
	i.open("./data/rcc5.spec");
	TS_ASSERT(i.is_open());
	if (!i.is_open()) return;

	const std::string name1 = "rcc5";
	const std::string dir = "./data/";
        CalculusReader r = CalculusReader(name1, dir, &i);
        Calculus* rcc5 = r.makeCalculus();

        const Relation EQ = rcc5->encodeRelation("=");
        const Relation PP = rcc5->encodeRelation("PP");
        const Relation PPC = rcc5->encodeRelation("PPC");
        const Relation PO = rcc5->encodeRelation("PO");
        const Relation DC = rcc5->encodeRelation("DC");

        TS_ASSERT_EQUALS(EQ, rcc5->getComposition(EQ, EQ));
        TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), rcc5->getComposition(PO, PO));
        TS_ASSERT_EQUALS(DC,rcc5->getComposition(PP, DC));
        TS_ASSERT_EQUALS(PP|DC|PO, rcc5->getComposition(DC|EQ, PP));

        TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), EQ|PP|PO|DC|PPC);

        delete rcc5;
    }

};
#endif // CALCULUS_READER_TEST_H
