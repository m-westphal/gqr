// -*- C++ -*-

// Copyright (C) 2006 Zeno Gantner
// Copyright (C) 2008,2012 Matthias Westphal
//
// This file is part of the Generic Qualitative Reasoner GQR.
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#include <cassert>
#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <utility>

#include "CombinedCalculusReader.h"

#include "CalculusMaker.h"
#include "CalculusReader.h"

#include "Calculus.h"
#include "Relation.h"
#include "Stringtools.h"

CombinedCalculusReader::CombinedCalculusReader(const std::string& name, const std::string& dir, std::ifstream* s)
	: stream(s), dataDir(dir), calculusName(name) {}

Calculus* CombinedCalculusReader::makeCalculus() {
  assert(stream->is_open());

  if (!readCombinationFile()) {
    std::cerr << "An error occurred while combining the calculi\n";
    return NULL;
  }

  try {
    generateBaseRelations();
  }
  catch ( ... ) {
    std::cerr << "An error occurred while generating the base relations.\n";
    return NULL;
  }
  if(baseRelations.empty())
    return NULL;

  try {
    generateCompositionTable();
  }
  catch(bool b) {
    assert(b == false);
    std::cerr << "An error occurred while generating the combined composition table.\n";
    return NULL;
  }

  generateConverses();
  generateWeights();

  assert(converses.size() == weights.size());
  assert(converses.size() == baseRelations.size());
  assert(converses.size() == relationSplitterBitsets.size());

  Calculus* nCalc = new Calculus(calculusName, baseRelations, identity, converses, compositionTable, weights);
  return nCalc;
}

bool CombinedCalculusReader::readCombinationFile() {
    try {
	if (!readCalculi())
	    return false;

	if (!readRefinements())
	    return false;

	if (!readPredicatesAndVariables())
	    return false;

	if (!readCompositionRefinements())
	    return false;

        if (!readCombSize())
	    return false;
    }
    catch (bool b) {
	std::cerr << "An error occurred\n";
	return false;
    }

    if (!checkRules())
	return false;

    return true;
}

void CombinedCalculusReader::unexpectedLine(const std::vector<std::string>& tokens) {
    std::cerr << "Unexpected line:";
    for (size_t i = 0; i < tokens.size(); i++)
	std::cerr << " " << tokens[i];
    std::cerr << std::endl;
}

const std::vector<std::string> CombinedCalculusReader::next_tokens(std::ifstream& file) const {
    std::vector<std::string> res;

    while (! file.eof() ) {
	std::string line;
	std::getline(file, line);

	line = Stringtools::trim(line);

	// skip empty or comment lines
	if (line.length() == 0 || line.substr(0,1) == "#")
	    continue;

	// split at ' ', '(', ')', '\t'
	for (size_t pos = 0; pos < line.length(); ) {
	    size_t next_start = line.find_first_of(" ()\t", pos);

	    if(pos == next_start)
		next_start = pos+1;
	    else if(next_start == std::string::npos)
		next_start = line.length();

	    // extract substring excluding next_start char
	    std::string s = line.substr(pos, next_start - pos);
	    s = Stringtools::trim(s);

	    if (s.length() > 0)
		res.push_back(s);

	    pos = next_start;
	}

	break;
    }

    return res;
}

size_t CombinedCalculusReader::find_end_of_list(const size_t p, const std::vector<std::string>& tokens) {
    if (p < tokens.size() && tokens[p] == "(") {
	size_t stack = 0;

    for (size_t i = p+1; i < tokens.size(); i++)
	if (tokens[i] == "(") stack++;
	else if (tokens[i] == ")") {
	    if (stack == 0) return i;
	    stack--;
	}
    }

    std::cerr << "Error parsing list in:\n";
    unexpectedLine(tokens);

    throw false;
}

bool CombinedCalculusReader::readCombSize()
{
    size_t numberOfBaseRelations = 0;

    while (!stream->eof()) {
        std::string line;
        std::getline(*stream, line);
        line = Stringtools::trim(line);

	// ignore empty lines and lines starting with #
        if (line == "" || line.substr(0, 1) == "#") {
            continue;
        }

        size_t pos = line.find_first_of(' ');
        if (pos != std::string::npos) {
            const std::string key = line.substr(0, pos);
	    if (key == "calculus_size") {
		const std::string value = line.substr(pos + 1, line.length() - pos - 1);
		numberOfBaseRelations = strtoul(value.c_str(), NULL, 10);
	    }
        }
    }

    if (numberOfBaseRelations == 0)
	return false;

    return true;
}


bool CombinedCalculusReader::readCalculi()
{
    const std::vector<std::string> tokens = next_tokens(*stream);

    if ( tokens.size() < 3 || tokens[0] != "combine") {
	std::cerr << "Expected ''combine $CALCULUS1 $CALCULUS2 ... $CALCULUSn'' line, got:\n";
	unexpectedLine(tokens);
	return false;
    }

    /** @todo xml support */
    /** @todo support reading combined calculi */
    for (size_t i = 1; i < tokens.size(); i++) {
	std::ifstream input;

	const std::string filename = dataDir+"/"+tokens[i]+".spec";
	input.open(filename.c_str());
	if (!input.is_open())
		return false;

	CalculusReader r(tokens[i], dataDir, &input);
	const Calculus* c = r.makeCalculus();

	if (c == NULL) {
	    std::cerr << "Failed to initialize calculus " << tokens[i] << std::endl;
	    return false;
	}
	calculi.push_back(std::make_pair(tokens[i], c));
    }

    assert(!calculi.empty());
    return true;
}

bool CombinedCalculusReader::readPredicatesAndVariables() {
    const std::vector<std::string> tokens = next_tokens(*stream);

    // end sign
    if (tokens.size() == 1 && tokens[0] == ".")
	return true;

    if (! (tokens.size() > 3 && tokens[1] == "::" && tokens[2] == "(" && tokens.back() == ")") ) {
	std::cerr << "Expected ''$NAME :: ( ... )'' or ''.'' line, got:\n";
	unexpectedLine(tokens);
	return false;
    }

    // special "variables" name?
    if (tokens[0] == "variables") {
	if (!variables.empty()) {
	    std::cerr << "Variables list specified twice.\n";
	    return false;
	}

	for (size_t i = 3; i < tokens.size(); i++)
	    variables.insert(tokens[i]);

	return readPredicatesAndVariables();
    }

    // predicate names need to be unique!
    if (predicates.find(tokens[0]) != predicates.end()) {
	std::cerr << "Relation " << tokens[0] << " defined more than once\n";
	return false;
    }

    // add predicate entry to map
    predicates.insert(std::make_pair(tokens[0], std::vector<std::vector<std::string> >()));

    for (size_t i = 3; i+1 < tokens.size();) {
	const size_t e = find_end_of_list(i, tokens);

	// add relation set to predicate
	predicates[tokens[0]].push_back(std::vector<std::string>());
	for (size_t j = i+1; j < e; j++)
	    predicates[tokens[0]].back().push_back(tokens[j]);

	// test arity
	if (predicates[tokens[0]].front().size() != predicates[tokens[0]].back().size()) {
	    std::cerr << "Error: arity of predicate " << tokens[0] << " changes\n";
	    std::cerr << "\tFirst set size: " << predicates[tokens[0]].front().size();
	    std::cerr << ", last seen set size: " << predicates[tokens[0]].back().size() << std::endl;
	    return false;
	}

	if (predicates[tokens[0]].back().empty()) {
	    std::cerr << "Found empty relation subset in " << tokens[0] << ": ";
	    unexpectedLine(tokens);
	    return false;
	}

	// advance i to start of next set or ')'
	i = e+1;
    }

    // try to read more predicates
    return readPredicatesAndVariables();
}

bool CombinedCalculusReader::readRefinements() {
    std::vector<size_t> from;
    size_t to = 0;

    // Read expected "refinement from to" line ...
    {
	const std::vector<std::string> tokens = next_tokens(*stream);

	// ... or end section sign
	if (tokens.size() == 1 && tokens[0] == ".")
	    return true;

	if (tokens.size() < 3 || tokens[0] != "refine") {
	    std::cout << "Expected ''refinement #FROM1 .. #FROMn #TO'' or ''.'' line, got:\n";
	    unexpectedLine(tokens);
	    return false;
	}

	for (size_t i = 1; i < tokens.size(); i++) {
	    size_t f;
	    if(!(std::istringstream(tokens[i])>>f) || f >= calculi.size()) {
		std::cerr << "Error: " << tokens[i] << " is not a valid calculus number\n";
		unexpectedLine(tokens);
		return false;
	    }
	    if (i == tokens.size()-1)
		to = f;
	    else
		from.push_back(f);
	}
    }

    assert(!from.empty());

    // read base relation refinements ...
    while (true) {
	const std::vector<std::string> reftokens = next_tokens(*stream);
	// ... or end sign
	if (reftokens.size() == 1 && reftokens[0] == ".")
	    break;

	if (reftokens.size() < from.size()+3 || reftokens[from.size()] != "::" || reftokens[from.size()+1] != "(") {
	    std::cout << "Expected ''BASERELATION1 .. BASERELATIONn :: ( [negated] list of base relations)'' or ''.'' line, got:\n";
	    unexpectedLine(reftokens);
	    return false;
	}

	std::vector<Relation> match;
	for (size_t i = 0; i < calculi.size(); i++)
	    match.push_back(calculi[i].second->getUniversalRelation());

	for (size_t i = 0; i < from.size(); i++) {
	    const size_t f = from[i];
	    if (f == to) {
		std::cout << "Broken refinement rule:\n";
		unexpectedLine(reftokens);
	    }

	    assert(f <= calculi.size());
	    match[f] = calculi[f].second->encodeRelation(reftokens[i]);
	}


	// copy relevant list entries
	std::vector<std::string> list;
	for (size_t i = from.size()+1; i < reftokens.size(); i++)
	    list.push_back(reftokens[i]);

	match[to] = match[to] & compileRefinementList(list, calculi[to].second);

	std::vector<std::vector<Relation> >& rules = refinements[to];
	// add a refinement rule mapping if its not already present
	if (refinements.find(to) == refinements.end())
	    refinements.insert(std::make_pair(to, std::vector<std::vector<Relation> >()));

	rules.push_back(match);
    }

    return readRefinements();
}

Relation CombinedCalculusReader::compileRefinementList(std::vector<std::string> s, const Calculus* c) const {
    bool negate = false;
    Relation rel;

    // allow empty refinement lists (for now..., not sure whether they are useful at all...)
    if (s.size() == 0) return rel;

    if (s[0] != "(" || s.back() != ")") {
	std::cerr << "Malformed RefinementList: ";
	unexpectedLine(s);
	throw false;
    }
    // remove list brackets
    s.erase(s.begin());
    s.pop_back();

    // support list negation
    if (s[0] == "not") {
	negate = true;
	s.erase(s.begin());
	if (s.size() < 2 || s[0] != "(" || s.back() != ")") {
	    std::cerr << "Malformed RefinementList: malformed negation set: ";
	    unexpectedLine(s);
	    throw false;
	}
	s.erase(s.begin());
	s.pop_back();
    }

    for (size_t i = 0; i < s.size(); i++)
	rel |= c->encodeRelation(s[i]);

    if (negate) {
	Relation neg;
	for (size_t i = 0; i < c->getNumberOfBaseRelations(); i++)
		if (!rel[i]) // TODO: can we optimize this?
			neg.set(i);
	return neg;
    }

    return rel;
}

CombinedCalculusReader::Formula::Formula(const std::vector<std::string>& s) {
    if (s.size() < 2 || s[0] != "(" || s.back() != ")") {
	std::cerr << "Malformed formula: ";
	unexpectedLine(s);
	throw false;
    }

    // Formula == "( )"
    if (s.size() == 2) return;

    if (s[1] != "and") // no conjunction
	predicates.push_back(readCondition(0, s));
    else
	for (size_t i = 2; i+1 < s.size();) {
	    const size_t e = find_end_of_list(i, s);

	    predicates.push_back(readCondition(i, s));
	    i = e+1;
	}
}

CombinedCalculusReader::Formula::Condition CombinedCalculusReader::Formula::readCondition(const size_t pos, const std::vector<std::string>& s) {
    if (s[pos] != "(" || pos+3 > s.size()) {
	std::cerr << "Malformed condition: ";
	unexpectedLine(s);
	throw false;
    }

    size_t r = 0;
    bool neg = false;
    std::string predicate;
    std::vector<std::string> vars;

    if (s[pos+1] == "not") {
	neg = true;
	predicate = s[pos+3];
	r = 2;
    } else
	predicate = s[pos+1];

    const size_t end = find_end_of_list(pos+r, s);

    for (size_t i = pos+r+2; i < end; i++)
	vars.push_back(s[i]);

    return Condition(neg, predicate, vars);
}

bool CombinedCalculusReader::readCompositionRefinements() {
    assert(!calculi.empty());

    const std::vector<std::string> tokens = next_tokens(*stream);

    // end sign
    if (tokens.size() == 1 && tokens[0] == ".")
	return true;


    if (tokens.size() < calculi.size()*2 + std::string("::()").length() + calculi.size()*3 ||
	tokens[calculi.size()] != ":"  || tokens[calculi.size()*2+1] != "::") {
	std::cerr << "Expected \"A B ... n : A' B' ... n' :: ( ... ) ... ( ... ) ( )\" or \".\" line, got:\n";
	unexpectedLine(tokens);
	return false;
    }

    std::vector<std::string> left;
    std::vector<std::string> right;

    size_t index = 0;
    for (; tokens[index] != ":"; index++) {
	assert(index < tokens.size());
	left.push_back(tokens[index]);
    }
    index++;
    for (; tokens[index] != "::"; index++) {
	assert(index < tokens.size());
	right.push_back(tokens[index]);
    }
    index++;

    std::vector<std::vector<std::string> > ref_lists(calculi.size());
    for (size_t i = 0 ; i < calculi.size(); i++) {
	const size_t end = find_end_of_list(index, tokens)+1; // handles out-of-bounds errors
	for (size_t j = index; j < end; j++)
	    ref_lists[i].push_back(tokens[j]);
	index = end;
    }

    // copy formula tokens
    std::vector<std::string> fcont;
    for (; index < tokens.size(); index++)
	fcont.push_back(tokens[index]);

    // compile ref lists
    std::vector<Relation> rl;
    for (size_t i = 0; i < calculi.size(); i++)
	rl.push_back(compileRefinementList(ref_lists[i], calculi[i].second));

    composition_refinements.push_back(
	std::make_pair(
	    std::make_pair(left, right),
	std::make_pair(rl, Formula(fcont))
	));

    return readCompositionRefinements();
}

void CombinedCalculusReader::generateBaseRelationsRec(std::vector<std::string>& v) {
    assert(!calculi.empty());
    assert(v.size() <= calculi.size());

    if (v.size() == calculi.size()) { // generated complete n-tuple
	/** construct a (unique!) name for the combined relation */
	std::string name = v[0];
	for (size_t i = 1; i < calculi.size(); i++)
	    name += "," + v[i];

	/** construct vector of R values */
	std::vector<Relation> projections;
	for (size_t i = 0; i < calculi.size(); i++)
	    projections.push_back(calculi[i].second->encodeRelation(v[i]));

	bool is_identity = true;
	for (size_t i = 0; i < calculi.size(); i++)
	    if (projections[i] != calculi[i].second->getIdentityRelation()) {
		is_identity = false;
		break;
	    }

        // apply base relation refinements (if there are any)
	for (size_t i = 0; i < calculi.size(); i++) {
	    if (refinements.find(i) == refinements.end())
	        continue;

	    const std::vector<std::vector<Relation> >& rules = refinements[i];
	    for (size_t j = 0; j < rules.size(); j++) {
		bool matches = true;
		for (size_t k = 0; k < calculi.size() && matches; k++)
		    if (i != k && (projections[k] & rules[j][k]).none())
			matches = false;
		if (matches)
		    projections[i] = projections[i] & rules[j][i];
	    }
	}

        // insert, if relation is non empty
	for (size_t i = 0; i < calculi.size(); i++)
	    if (projections[i].none())
		return;

	baseRelations.push_back(name);
        relationSplitter.insert(std::make_pair(name, v));
	relationSplitterBitsets.push_back(projections);
	if (is_identity)
	   identity = baseRelations.size()-1;
    }
    else { // v is not a complete n-tupel yet
	const size_t i = v.size();
	for (size_t b = 0; b < calculi[i].second->getNumberOfBaseRelations(); ++b) {
	    v.push_back(calculi[i].second->getBaseRelationName(b));
	    generateBaseRelationsRec(v);
	    v.pop_back();
	}
    }
}

void CombinedCalculusReader::generateBaseRelations() {
    std::vector<std::string> v;
    v.reserve(calculi.size());
    generateBaseRelationsRec(v);
    assert(v.empty());
}

bool CombinedCalculusReader::matchRule(const std::vector<std::string>& left, const std::vector<std::string>& right, const std::pair<str_composition, composition_refinement>& r) const {
  // instantiated variables:
  std::map<std::string, std::string> inst;

  const str_composition& rl = r.first;

  assert(left.size() == right.size());
  assert(left.size() == rl.first.size());
  assert(left.size() == rl.second.size());

  // test rule and instantiate variables, if needed
  for(size_t i = 0; i < rl.first.size(); i++) {
    // left side
    if(variables.find(rl.first[i]) != variables.end()) { // variable
      if(inst.find(rl.first[i]) != inst.end()) {
        std::cerr << "Rule error, variable '" << rl.first[i] << "' instantiated twice\n"; // FIXME print rule(?)
        throw false;
      }

      inst.insert(make_pair(rl.first[i], left[i]));
    }
    else if(rl.first[i] != left[i]) return false;
    // right side
    if(variables.find(rl.second[i]) != variables.end()) { // variable
      if(inst.find(rl.second[i]) != inst.end()) {
        std::cerr << "Rule error, variable '" << rl.second[i] << "' instantiated twice\n"; // FIXME print rule(?)
        throw false;
      }

      inst.insert(make_pair(rl.second[i], right[i]));
    }
    else if(rl.second[i] != right[i]) return false;
  }

  // rule matches so far...
  // check formula
  const Formula& f = r.second.second;

  for(size_t i = 0; i < f.predicates.size(); i++) {
    if(f.predicates[i].vars.size() == 0) {
      std::cerr << "Empty condition, using relation '" << f.predicates[i].predicate << "'\n";
      throw false;
    }

    // build in == predicate
    if(f.predicates[i].predicate == "==") {
      if(f.predicates[i].vars.size() < 2) {
        std::cerr << "Error: Build-in relation ''=='' has arity of at least 2!\n";
        throw false;
      }
      bool res = true;
      if(inst.find(f.predicates[i].vars[0]) == inst.end()) {
        std::cerr << "Error: variable '" << f.predicates[i].vars[0] << "' not instantiated\n";
        throw false;
      }
      const std::string val = inst[f.predicates[i].vars[0]];
      for(size_t j = 1; j < f.predicates[i].vars.size() && res == true; j++) {
        if(inst.find(f.predicates[i].vars[j]) == inst.end()) {
          std::cerr << "Error: variable '" << f.predicates[i].vars[0] << "' not instantiated\n";
          throw false;
        }
        if(inst[f.predicates[i].vars[j]] != val) res = false;
      }

      if(!f.predicates[i].negate && !res) return false;
      if(f.predicates[i].negate && res) return false;
      continue;
    }

    if(predicates.find(f.predicates[i].predicate) == predicates.end()) {
      std::cerr << "Rule error, unknown predicate " << f.predicates[i].predicate << std::endl;
      throw false;
    }

    const std::vector<std::vector<std::string> >& sets = predicates.find(f.predicates[i].predicate)->second;
    bool one_matched = false;
    for(size_t j = 0; j < sets.size() && !one_matched; j++) {
      if(f.predicates[i].vars.size() != sets[j].size()) {
        std::cout << "Rule error, arity mismatch " << f.predicates[i].predicate << ": ";
	std::cout << "\tFormula predicate arity" << f.predicates[i].vars.size() << " vs. known predicate arity " << sets[j].size() << "\n";
        throw false;
      }

      bool hit = true;
      for(size_t k = 0; k < f.predicates[i].vars.size() && hit == true; k++) {
        if(inst.find(f.predicates[i].vars[k]) == inst.end()) {
          std::cout << "Rule depends on uninstantiated var '" << f.predicates[i].vars[k] << "'\n";
          throw false;
        }
        if(sets[j][k] != inst[f.predicates[i].vars[k]])
          hit = false;
      }
      // Negate AND predicate matches => condition fails
      if(f.predicates[i].negate && hit)
        return false;
      // Not negated AND predicate matches => predicate satisfied
      if(!f.predicates[i].negate && hit)
        one_matched = true;
    }
    // Not negated AND none matched => condition fails
    if(!one_matched && !f.predicates[i].negate)
      return false;
  }

  return true;
}

void CombinedCalculusReader::generateCartesianProduct(Relation& result, std::string assemble, std::vector<Relation>& iresults, const size_t index) const {
    assert(calculi.size() == iresults.size());
    assert(index < calculi.size());

//    result = A,B,C,D + " " + ...

    for (size_t br = 0; br < calculi[index].second->getNumberOfBaseRelations() ; br++) {
	Relation baserel;
	baserel.set(br);
	if (!iresults[index][br])
	    continue;
	const std::string& name = calculi[index].second->getBaseRelationName(br);
	if (index == calculi.size() - 1) {
	    const std::string fullname = assemble+name;
	    std::vector<std::string>::const_iterator it;
	    it = std::find(baseRelations.begin(), baseRelations.end(), fullname);
	    if (it != baseRelations.end()) {
		result.set(it - baseRelations.begin());
	}
	}
	else
	    generateCartesianProduct(result, assemble + name + ",", iresults, index+1);
    }
}

void CombinedCalculusReader::generateCompositionTable() {
    compositionTable.resize(baseRelations.size());
    for (size_t b1 = 0; b1 < baseRelations.size(); ++b1) {
	compositionTable[b1].resize(baseRelations.size());
        for (size_t b2 = 0; b2 < baseRelations.size(); ++b2) {
	    const std::vector<std::string>& left = relationSplitter.find(baseRelations[b1])->second;
	    const std::vector<std::string>& right = relationSplitter.find(baseRelations[b2])->second;

	    assert(left.size() == right.size());

	    std::vector<Relation> results;
	    for (size_t i = 0; i < calculi.size(); i++)
		results.push_back(
		    calculi[i].second->getComposition(calculi[i].second->encodeRelation(left[i]), calculi[i].second->encodeRelation(right[i]))
		);

	    for (size_t i = 0; i < composition_refinements.size(); i++)
		if (matchRule(left, right, composition_refinements[i]))
		    for (size_t j = 0; j < results.size(); j++)
			results[j] = results[j] & composition_refinements[i].second.first[j];


	    Relation result;
	    generateCartesianProduct(result, std::string(), results, 0);

	    compositionTable[b1][b2] = result;
	}
    }
}

void CombinedCalculusReader::generateConverses() {
  converses.resize(baseRelations.size());
  for(size_t b = 0; b < baseRelations.size(); ++b) {
    const std::vector<std::string>& r = relationSplitter[baseRelations[b]];
    std::string conv;
    assert(r.size() == calculi.size());
    for(size_t i = 0; i < r.size(); i++) {
      const std::string s = calculi[i].second->relationToString(calculi[i].second->getConverse(calculi[i].second->encodeRelation(r[i])));
      conv += Stringtools::trim(s.substr(1, s.length()-2));

      if(i+1 < r.size()) conv += ",";
    }

    assert(find(baseRelations.begin(), baseRelations.end(), conv) != baseRelations.end());
    converses[b] = find(baseRelations.begin(), baseRelations.end(), conv) - baseRelations.begin();
  }

  assert(converses.size() == baseRelations.size());
}

void CombinedCalculusReader::generateWeights() {
  assert(weights.empty());
  weights.resize(baseRelations.size());

  for(size_t b = 0; b < baseRelations.size(); ++b) {
    const std::vector<std::string>& r = relationSplitter[baseRelations[b]];
    unsigned w = 0;

    for(size_t i = 0; i < r.size(); i++) {
      w += calculi[i].second->getWeight(calculi[i].second->encodeRelation(r[i]));
    }

    weights[b] = w;
  }

  assert(weights.size() == baseRelations.size());
}

bool CombinedCalculusReader::checkRules() const {
    // check refinement rules
    for (std::map<const size_t, std::vector<std::vector<Relation> > >::const_iterator it = refinements.begin(); it != refinements.end(); it++) {
	const size_t refines = it->first;
	const std::vector<std::vector<Relation> >& rules = it->second;

	assert(refines < calculi.size());

	for (size_t i = 0; i < rules.size(); i++) {
	    for (size_t j = 0; j < rules[i].size(); j++) {
		if (j == refines)
		    continue;
		if (rules[i][j] == calculi[j].second->getUniversalRelation())
		    continue;

		const Relation ref = rules[i][refines];

		for (size_t b = 0; b < calculi[refines].second->getNumberOfBaseRelations(); b++) {
		    if (ref[b])
			continue;

		    Relation baseRel;
		    baseRel.set(b);

		    if (refinements.find(j) == refinements.end()) {
			std::cerr << "Rule that refines " << refines << ":" << calculi[refines].first << " to " << calculi[refines].second->relationToString(rules[i][refines]);
			std::cerr << " has no rule for " << j << ":" << calculi[j].first << "\n";
			std::cerr << "\n";
			return false;
		    }

		    bool has_converse_rule = false;
		    const std::vector<std::vector<Relation> >& convrules = refinements.find(j)->second;
		    for(size_t k = 0; k < convrules.size() && !has_converse_rule; k++) {
			std::vector<Relation> test = rules[i];
			test[refines] = baseRel;
			test[j] = convrules[k][j];
			if (convrules[k] == test && (rules[i][j] & convrules[k][j]).none())
			    has_converse_rule = true;
		    }
		    if (!has_converse_rule) {
			std::cerr << "Rule that refines " << refines << ":" << calculi[refines].first << " to " << calculi[refines].second->relationToString(rules[i][refines]);
			std::cerr << " has no converse rule for " << j << ":" << calculi[j].first << "\n";
			std::cerr << "\n";
//			return false;
		    }
		}
	    }
	}
    }
    // Composition refinement checks can be handled by "check" module

    return true;
}

std::vector<std::vector<std::vector<Relation> > >
CombinedCalculusReader::compileInterdependencies() {
    std::vector<std::vector<std::vector<Relation> > > res;

    if(!readCombinationFile() || calculi.size() < 2) {
	std::cerr << "An error occurred while combining the calculi, check the combination specification file.\n No interdependencies are being used.\n";
	res.resize(calculi.size());
	return res;
    }

    // setup empty table
    res.resize(calculi.size());

    for (size_t calc = 0; calc < calculi.size(); calc++) {
	if (refinements.find(calc) == refinements.end()) {
	    continue;
	}

	// blow-up existing rules
	const std::vector<std::vector<Relation> >& rules = refinements.find(calc)->second;

	bool done = false;
	std::vector<size_t> state(calculi.size(), 0);

	int idx = calculi.size()-1;
	while (!done) {
	    // test if we have a rule for this br tuple
	    bool covered = false;
	    for (size_t rule = 0; rule < rules.size() && !covered; rule++) {
		bool match = true;
		for (size_t nr = 0; nr < calculi.size() && match; nr++)
		    if (nr != calc && !rules[rule][nr][state[nr]])
			match = false;
		if (match) { // rule matches state
		    covered = true;
		}
	    }

	    if (!covered) {
		std::vector<Relation> refine;
		for (size_t nr = 0; nr < calculi.size(); nr++)
		    if (nr == calc)
			refine.push_back(calculi[nr].second->getUniversalRelation());
		    else {
			Relation tmp;
			tmp.set(state[nr]);
			refine.push_back(tmp);
		    }
		res[calc].push_back(refine);
	    }

	    // get next base relation tuple
	    while(true) {
		if ((size_t) idx == calc)
		    idx--;
		if (idx < 0) {
		    done = true;
		    break;
		}

		if (state[idx]+1 > calculi[idx].second->getNumberOfBaseRelations()) {
		    state[idx] = 0;
		    idx--;
		}
		else {
		    state[idx]++;
		    break;
		}
	    }
	}
	// add real rules
	for (size_t rule = 0; rule < rules.size(); rule++)
	    res[calc].push_back(rules[rule]);
    }

    return res;
}
