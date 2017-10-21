// -*- C++ -*-
#ifndef CALCULUS_TEST_H
#define CALCULUS_TEST_H

#include <iostream>
#include <fstream>
#include <string>

#include "TestSuite.h"

#include "Relation.h"
#include "Calculus.h"
#include "CalculusReader.h"

class CalculusTest : public CxxTest::TestSuite {

    Calculus* rcc5;
    Relation EQ, PP, PPC, PO, DC;

  public:
    void setUp() {
        Relation::init();

	rcc5 = NULL;

	std::ifstream i;
	i.open("./data/rcc5.spec");
	if (!i.is_open()) return;

	const std::string name = "rcc5";
	const std::string dir = "./data/";
        CalculusReader r = CalculusReader(name, dir, &i);
        rcc5 = r.makeCalculus();
	i.close();

	if (!rcc5) return;
        EQ = rcc5->encodeRelation("=");
        PP = rcc5->encodeRelation("PP");
        PPC = rcc5->encodeRelation("PPC");
        PO = rcc5->encodeRelation("PO");
        DC = rcc5->encodeRelation("DC");
    }

    void tearDown() {
	Relation::clean_up();

        delete rcc5;
    }

    /** @todo test constructor in an isolated manner, e.g. for point algebra */
    /** @todo (design) two test classes, one for allen, one for rcc5 */

    void testConstructor ( void ) {
	std::ifstream i;
	i.open("./data/allen.spec");
	TS_ASSERT(i.is_open());
	if (!i.is_open()) return;

	const std::string name = "allen";
	const std::string dir = "./data/";
        CalculusReader r = CalculusReader(name, dir, &i);
	Calculus* allen = r.makeCalculus();
	TS_ASSERT(allen);
	if (!allen) return;

        TS_ASSERT_EQUALS(13, allen->getNumberOfBaseRelations());
        TS_ASSERT_EQUALS(allen->getUniversalRelation(), allen->getComposition(allen->getUniversalRelation(), allen->getIdentityRelation()));
        TS_ASSERT_EQUALS(allen->getUniversalRelation(), allen->getComposition(allen->getIdentityRelation(), allen->getUniversalRelation()));

        delete allen;
    }


    void testGetNumberOfBaseRelations ( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
        TS_ASSERT_EQUALS(5, rcc5->getNumberOfBaseRelations());
    }

    void testIdentityCondition( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
        Relation id = rcc5->getIdentityRelation();
        TS_ASSERT_EQUALS(id, rcc5->getConverse(id));
    }

    void testAllCondition( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
        TS_ASSERT_EQUALS(EQ|DC|PO|PP|PPC, rcc5->getUniversalRelation());
    }

    void testGetConverse( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
       TS_ASSERT_EQUALS(EQ, rcc5->getConverse(EQ));
       TS_ASSERT_EQUALS(DC, rcc5->getConverse(DC));
       TS_ASSERT_EQUALS(PO, rcc5->getConverse(PO));
       TS_ASSERT_EQUALS(PP, rcc5->getConverse(PPC));
       TS_ASSERT_EQUALS(PPC, rcc5->getConverse(PP));

       TS_ASSERT_EQUALS(EQ | PP | PPC, rcc5->getConverse(EQ | PPC | PP));
       TS_ASSERT_EQUALS(DC | PO | PPC, rcc5->getConverse(DC | PO | PP));

       TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), rcc5->getConverse(rcc5->getUniversalRelation()));
       TS_ASSERT_EQUALS((Relation()), rcc5->getConverse(Relation()));
    }

    void testGetComposition( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
        TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), rcc5->getComposition(PO, PO));
        TS_ASSERT_EQUALS(EQ, rcc5->getComposition(EQ, EQ));
        TS_ASSERT_EQUALS(DC,rcc5->getComposition(PP, DC));

        TS_ASSERT_EQUALS(PP | DC | PO, rcc5->getComposition(DC | EQ, PP));

        TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), rcc5->getComposition(DC, DC));
    }

    void testEncodeRelation( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
       TS_ASSERT_EQUALS((Relation()), rcc5->encodeRelation(""));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation("PP"));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation("  PP"));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation("PP  "));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation(" PP"));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation("PP "));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation(" PP "));
       TS_ASSERT_EQUALS(PP, rcc5->encodeRelation("  PP  "));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("= DC PP"));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("= PP DC"));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("DC = PP"));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("DC PP ="));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("PP DC ="));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("PP = DC"));
       TS_ASSERT_EQUALS(EQ | DC | PP, rcc5->encodeRelation("= DC PP ="));

       TS_ASSERT_EQUALS(EQ|DC|PP, rcc5->encodeRelation("    = DC PP =    "));
       TS_ASSERT_EQUALS(EQ|DC|PP, rcc5->encodeRelation(" = DC PP"));
       TS_ASSERT_EQUALS(EQ|DC|PP, rcc5->encodeRelation("= DC PP "));
       TS_ASSERT_EQUALS(EQ|DC|PP, rcc5->encodeRelation("= DC  PP "));

       TS_ASSERT_EQUALS(EQ|PP|PPC, rcc5->encodeRelation(" = PP  PPC    "));
    }

    void testGetCompositionEquality( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
       TS_ASSERT_EQUALS(EQ, rcc5->getComposition(EQ, EQ));
       TS_ASSERT_EQUALS(PP, rcc5->getComposition(EQ, PP));
       TS_ASSERT_EQUALS(PPC, rcc5->getComposition(EQ, PPC));
       TS_ASSERT_EQUALS(DC, rcc5->getComposition(EQ, DC));
       TS_ASSERT_EQUALS(PO, rcc5->getComposition(EQ, PO));
       TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), rcc5->getComposition(EQ, rcc5->getUniversalRelation()));

       TS_ASSERT_EQUALS(PP, rcc5->getComposition(PP, EQ));
       TS_ASSERT_EQUALS(PPC, rcc5->getComposition(PPC, EQ));
       TS_ASSERT_EQUALS(DC, rcc5->getComposition(DC, EQ));
       TS_ASSERT_EQUALS(PO, rcc5->getComposition(PO, EQ));
       TS_ASSERT_EQUALS(rcc5->getUniversalRelation(), rcc5->getComposition(rcc5->getUniversalRelation(), EQ));

       TS_ASSERT_EQUALS((Relation()), rcc5->getComposition(EQ, Relation()));
       TS_ASSERT_EQUALS((Relation()), rcc5->getComposition(Relation(), EQ));
    }

    void testRelationToStringEmptyRelation( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
        // @todo define regular expressions to be more liberal ...
        TS_ASSERT_EQUALS(rcc5->relationToString(Relation()), "( )");
    }

    void testRelationToStringUniversalRelation( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
        std::string allString = rcc5->relationToString(rcc5->getUniversalRelation());
        TS_ASSERT(allString.find("=") != std::string::npos);
        TS_ASSERT(allString.find("PP") != std::string::npos);
        TS_ASSERT(allString.find("PPC") != std::string::npos);
        TS_ASSERT(allString.find("DC") != std::string::npos);
        TS_ASSERT(allString.find("PO") != std::string::npos);
    }

    void testRelationToStringBaseRelations( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;
       TS_ASSERT_EQUALS("( = )",   rcc5->relationToString(EQ));
       TS_ASSERT_EQUALS("( PP )",  rcc5->relationToString(PP));
       TS_ASSERT_EQUALS("( PPC )", rcc5->relationToString(PPC));
       TS_ASSERT_EQUALS("( DC )",  rcc5->relationToString(DC));
       TS_ASSERT_EQUALS("( PO )",  rcc5->relationToString(PO));
    }

    void testGetWeight( void ) {
	TS_ASSERT(rcc5);
	if (!rcc5) return;

        TS_ASSERT_EQUALS(0, rcc5->getWeight(Relation()));

	TS_ASSERT_DIFFERS(0, rcc5->getWeight(EQ));
	TS_ASSERT_DIFFERS(0, rcc5->getWeight(PP));
	TS_ASSERT_DIFFERS(0, rcc5->getWeight(PPC));
	TS_ASSERT_DIFFERS(0, rcc5->getWeight(DC));
	TS_ASSERT_DIFFERS(0, rcc5->getWeight(PO));
	TS_ASSERT_EQUALS(rcc5->getWeight(PO|DC), rcc5->getWeight(PO)+rcc5->getWeight(DC));
    }
};
#endif // CALCULUS_TEST_H
