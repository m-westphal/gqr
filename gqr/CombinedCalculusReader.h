// -*- C++ -*-

// Copyright (C) 2006 Zeno Gantner
// Copyright (C) 2008,2012 Matthias Westphal
//
// This file is part of the Generic Qualitative Reasoner GQR.
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this program; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef COMBINEDCALCULUSREADER_H
#define COMBINEDCALCULUSREADER_H

#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "CalculusMaker.h"
#include "Relation.h"

/**
 * Class for creating a calculus object from a combination-specification file
 */
class CombinedCalculusReader : public CalculusMaker {
  private:
	// stream handed to constructor
	std::ifstream* stream;

	// directory where .spec is residing
	const std::string dataDir;

	/** Name of the calculus, e.g. 'allen', 'rcc5', 'rcc8' */
	const std::string calculusName;

    /** vector storing the underlying calculi (stores calculus name and instance of calculus class) */
    std::vector<std::pair<std::string, const Calculus*> > calculi;

    /** map storing the refinement rules (aka interdependencies)
        i |-> list of vectors is equivalent to vector without i |-> i calculus for all vectors in list */
    std::map<const size_t, std::vector<std::vector<Relation> > > refinements;

    /** variable tokens (read from combination specification file) */
    std::set<std::string> variables;
    /** map containing the predicates given in a combination specification file */
    std::map<const std::string, std::vector<std::vector<std::string> > > predicates;

    typedef std::pair<std::vector<std::string>, std::vector<std::string> > str_composition;

    /** convert a string based refinement list of relations to a relation
	(this includes lists prefixed with not) */
    Relation compileRefinementList(std::vector<std::string> s, const Calculus* c) const;

    /** formulas = conjunctions of predicates with variables. Predicates may be negated */
    class Formula {
	/** predicate */
	class Condition {
	    public:
		/** is predicate negated ? */
		bool negate;
		/** predicate name */
		std::string predicate;
		/** associated variable names */
		std::vector<std::string> vars;
		/** Initialize new predicate */
		Condition(const bool b, const std::string& r, const std::vector<std::string>& v) : negate(b), predicate(r), vars(v) {};
	};
	public:
	/** parse condition */
	static Condition readCondition(const size_t pos, const std::vector<std::string>& tokens);
	/** conjunction of predicates */
	std::vector<Condition> predicates;
	/** Constructor handles parsing of entire formula */
	Formula(const std::vector<std::string>& s);
    };

    typedef std::pair<std::vector<Relation>, Formula> composition_refinement;
    /** assembled refinements */
    std::vector<std::pair<str_composition, composition_refinement> > composition_refinements;
    /** test if composition refinement rule matches input (might throw false) */
    bool matchRule(const std::vector<std::string>& left, const std::vector<std::string>& right, const std::pair<str_composition, composition_refinement>& r) const;

    /** read the next tokens from ifstream. Ignores comment and empty lines. Only returns an empty vector iff eof is encountered
     *  tokens are split at any of {()TAB } chars. Empty tokens and tokens only including 'TAB' are removed. */
    const std::vector<std::string> next_tokens(std::ifstream& file) const;

    /** dump tokens to std::cout */
    static void unexpectedLine(const std::vector<std::string>& tokens);

    /** find the of the list starting at p. Throws false in case of an error otherwise returns position of end list char ")" */
    static size_t find_end_of_list(const size_t p, const std::vector<std::string>& tokens);

    /** read function for combination specifications files */
    bool readCombinationFile();
    /** read combination size and check if it's not too big for the relation type */
    bool readCombSize();
    /** read the calculi that are to be combined and initialize them */
    bool readCalculi();
    /** read Predicates and variables definitions */
    bool readPredicatesAndVariables();
    /** read (base relation) refinements */
    bool readRefinements();
    /** read composition refinements */
    bool readCompositionRefinements();

    /** check data structures/rules for errors */
    bool checkRules() const;

    /** generate base relations of combined calculi */
    void generateBaseRelations();
    /** recursively generate next base relation tuple */
    void generateBaseRelationsRec(std::vector<std::string>& );
    /** generate composition table of combined calculi */
    void generateCompositionTable();
    /** build a Cartesian product given entries for each calculi */
    void generateCartesianProduct(Relation& result, std::string assemble, std::vector<Relation>& iresults, const size_t index) const;
    /** generate converses of base relations for combined calculi */
    void generateConverses();
    /** generate weights of relations for combined calculi */
    void generateWeights();

    /** Map new base relation to old ones */
    std::map<std::string, std::vector<std::string> > relationSplitter;

    /** Map new base relation to old ones (bitsets) */
    std::vector<std::vector<Relation> > relationSplitterBitsets;

    /** string based composition table */
    std::vector<std::vector<Relation> > compositionTable;
    /** map strings to relations */
    std::vector<std::string> baseRelations;
    /** converses table for base relations*/
    std::vector<size_t> converses;
    /** weights based on strings */
    std::vector<size_t> weights;
    /** string representation of the identity relation */
    size_t identity;

  public:
    /**
     * Create a new Calculus object
     *
     * @param memlimitComposition amount of memory that could be used for precomputations of compositions
     * @param memlimitConverses amount of memory that could be used for precomputations of converses
     *
     * @return pointer to the Calculus object
     */
    virtual Calculus* makeCalculus();

    /** @todo Interdependencies should be optimized for speed / lookup tables / caching etc */
    /** return the interdependencies table: table[refines_calculus_nr][n-1 Tuple of relations with refined relation] */
    std::vector<std::vector<std::vector<Relation> > > compileInterdependencies();

    CombinedCalculusReader(const std::string& name, const std::string& dir, std::ifstream* stream);

    /** Destructor */
    virtual ~CombinedCalculusReader() {};
};

#endif
