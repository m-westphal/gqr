// -*- C++ -*-

#ifndef ALLENCALCULUSTEST_H
#define ALLENCALCULUSTEST_H

#include <iostream>
#include <string>
#include <map>

#include "Calculus.h"
#include "CalculusReader.h"

#include "TestSuite.h"

class AllenCalculusTest : public CxxTest::TestSuite
{
  Calculus* allen;
  Relation EQ, D, DI, LT, GT, M, MI, S, SI, O, OI, F, FI;

 public:
  void setUp()
  {
	allen = NULL;
	std::ifstream i;
	i.open("./data/allen.spec");
	TS_ASSERT(i.is_open());
	if (!i.is_open()) return;

	const std::string name1 = "allen";
	const std::string dir = "./data/";
        CalculusReader r = CalculusReader(name1, dir, &i);
        allen = r.makeCalculus();
	i.close();
	TS_ASSERT(allen);
	if(!allen) return;


    EQ = allen->encodeRelation("=");
    D = allen->encodeRelation("d");
    DI = allen->encodeRelation("di");
    LT = allen->encodeRelation("<");
    GT = allen->encodeRelation(">");
    M = allen->encodeRelation("m");
    MI = allen->encodeRelation("mi");
    S = allen->encodeRelation("s");
    SI = allen->encodeRelation("si");
    O = allen->encodeRelation("o");
    OI = allen->encodeRelation("oi");
    F = allen->encodeRelation("f");
    FI = allen->encodeRelation("fi");
  }

  void tearDown() {
    delete allen;
  }

  void testGetNumberOfBaseRelations ( void ) {
   TS_ASSERT(allen); if(!allen) return;
   TS_ASSERT_EQUALS(13, allen->getNumberOfBaseRelations());
  }

  void testIdentityCondition( void ) {
   TS_ASSERT(allen); if(!allen) return;
   const  Relation id = allen->getIdentityRelation();
    TS_ASSERT_EQUALS(id, allen->getConverse(id));
  }

  void testUniversal( void ) {
   TS_ASSERT(allen); if(!allen) return;
    const std::string all = "= < > m mi d di o oi s si f fi";
    const Relation r = allen->encodeRelation(all);
    TS_ASSERT_EQUALS(r, allen->getUniversalRelation());
  }


  void testGetConverse( void ) {
   TS_ASSERT(allen); if(!allen) return;
   TS_ASSERT_EQUALS(D, allen->getConverse(DI));
   TS_ASSERT_EQUALS(DI, allen->getConverse(D));
   TS_ASSERT_EQUALS(M, allen->getConverse(MI));
   TS_ASSERT_EQUALS(MI, allen->getConverse(M));
   TS_ASSERT_EQUALS(F, allen->getConverse(FI));
   TS_ASSERT_EQUALS(FI, allen->getConverse(F));
   TS_ASSERT_EQUALS(S, allen->getConverse(SI));
   TS_ASSERT_EQUALS(SI, allen->getConverse(S));
   TS_ASSERT_EQUALS(O, allen->getConverse(OI));
   TS_ASSERT_EQUALS(OI, allen->getConverse(O));
   TS_ASSERT_EQUALS(LT, allen->getConverse(GT));
   TS_ASSERT_EQUALS(GT, allen->getConverse(LT));
   TS_ASSERT_EQUALS(allen->getConverse(EQ), EQ);
   TS_ASSERT_EQUALS(EQ, allen->getIdentityRelation());
  }

  void testGetComposition( void )
  {
   TS_ASSERT(allen); if(!allen) return;
   TS_ASSERT_EQUALS((Relation()), allen->getComposition(Relation(), Relation()));

   TS_ASSERT_EQUALS(LT, allen->getComposition(M, M));
   TS_ASSERT_EQUALS(EQ, allen->getComposition(EQ, EQ));
   TS_ASSERT_EQUALS(GT|OI|MI|DI|SI, allen->getComposition(DI, GT));

   TS_ASSERT_EQUALS(LT|S|M, allen->getComposition(LT|EQ|M, S));

   TS_ASSERT_EQUALS(allen->getUniversalRelation(), allen->getComposition(D, DI));
   TS_ASSERT_EQUALS(allen->getUniversalRelation(), allen->getComposition(LT, GT));
   TS_ASSERT_EQUALS(allen->getUniversalRelation(), allen->getComposition(GT, LT));
  }

  void testEncodeRelation( void )
  {
   TS_ASSERT(allen); if(!allen) return;
   TS_ASSERT_EQUALS((Relation()), allen->encodeRelation(""));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("= < s"));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("= s <"));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("< = s"));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("< s ="));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("s < ="));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("s = <"));
   TS_ASSERT_EQUALS(EQ | LT | S, allen->encodeRelation("= < s ="));

   TS_ASSERT_EQUALS(EQ|LT|S, allen->encodeRelation("    = < s =    "));
   TS_ASSERT_EQUALS(EQ|LT|S, allen->encodeRelation(" = < s"));
   TS_ASSERT_EQUALS(EQ|LT|S, allen->encodeRelation("= < s "));
   TS_ASSERT_EQUALS(EQ|LT|S, allen->encodeRelation("= <  s "));

   TS_ASSERT_EQUALS(EQ, allen->encodeRelation("= "));

   TS_ASSERT_EQUALS(EQ|S|SI, allen->encodeRelation(" = s  si    "));
  }

  void testGetBaseRelationNames( void ) {
   TS_ASSERT(allen); if(!allen) return;
    TS_ASSERT_EQUALS(13, allen->getNumberOfBaseRelations());
    std::string all;
    for (size_t i = 0; i < 13; i++)
	all += " " + allen->getBaseRelationName(i) + " ";

    TS_ASSERT(all.find(" = ") != std::string::npos);
    TS_ASSERT(all.find(" < ") != std::string::npos);
    TS_ASSERT(all.find(" > ") != std::string::npos);
    TS_ASSERT(all.find(" d ") != std::string::npos);
    TS_ASSERT(all.find(" di ") != std::string::npos);
    TS_ASSERT(all.find(" f ") != std::string::npos);
    TS_ASSERT(all.find(" fi ") != std::string::npos);
    TS_ASSERT(all.find(" s ") != std::string::npos);
    TS_ASSERT(all.find(" si ") != std::string::npos);
    TS_ASSERT(all.find(" o ") != std::string::npos);
    TS_ASSERT(all.find(" oi ") != std::string::npos);
    TS_ASSERT(all.find(" m ") != std::string::npos);
    TS_ASSERT(all.find(" mi ") != std::string::npos);
  }

};
#endif //ALLENCALCULUSTEST_H
