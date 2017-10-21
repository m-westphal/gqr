// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
// Copyright (C) 2010 Isgandar Valizada
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

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>

#include <cassert>

#include "FileSplitter.h"

#include "Stringtools.h"
#include "Calculus.h"

#include "gqrtl/ValueHeuristic.h"

FileSplitter::FileSplitter(const Calculus& calculus, std::ifstream* splitSetFile) {
	readSplitSetFromFile(splitSetFile, calculus);
	assert(!splitRelations.empty());
	orderSplitRelations(calculus);
	presortSplitRelations(calculus);
}
FileSplitter::~FileSplitter() {}

void FileSplitter::readSplitSetFromFile(std::ifstream* input, const Calculus& calculus) {
	if (! input->is_open() ) {
		std::cerr << "FileSplitter::readSplitSetFromFile(): Error opening input stream";
		std::cerr << std::endl << std::flush;
		return;
	}

	while (! input->eof() ) {
		std::string line;
		std::getline(*input, line);

		line = Stringtools::trim(line);
		if (line == "") {
			continue;
		}
		if (line.substr(0, 1) == "#") {
			continue;
		}

		size_t start = line.find_first_of('(');
		size_t end = line.find_first_of(')', start);

		if ( (start == std::string::npos) || (end == std::string::npos) ) {
			std::cerr << "FileSplitter::readSplitSetFromFile(): Failed to parse line: " << line << std::endl;
			std::cerr << "... ignoring this line!" << std::endl;
		}

		const std::string s = line.substr(start+1, end-start-1);

		const Relation r = calculus.encodeRelation(s);
		if (!r.none())
			splitRelations.push_back(r);
	}
}

void FileSplitter::orderSplitRelations(const Calculus& calculus) {
	gqrtl::ValueHeuristic order(calculus);

	std::sort(splitRelations.begin(), splitRelations.end(), order);
}

void FileSplitter::presortSplitRelations(const Calculus& calculus) {
	presortedSplitRelationsLookup.resize(calculus.getNumberOfBaseRelations());

	// assume we require the best match for a relation, where b is the lowest bit
	for (size_t b = 0; b < presortedSplitRelationsLookup.size(); b++)
		for (size_t i = 0; i < splitRelations.size(); i++) {
			if (splitRelations[i].none()) continue;

			const size_t lowest_bit = *splitRelations[i].begin();

			// splitRelations[i] must be a subset of r
			if (lowest_bit < b) // cannot be a subset
				continue;
			presortedSplitRelationsLookup[b].push_back(&splitRelations[i]);
		}
}

bool FileSplitter::isSplit(const Relation& r) const {
	if (r.none()) return true; // TODO: we should not need to calculate this (it does not make much sense)

	// TODO: OMG this is extremely slow
	std::vector<Relation>::const_iterator it =
		std::find(splitRelations.begin(), splitRelations.end(), r);

	return !(it == splitRelations.end());
}

Relation FileSplitter::getFirstSplit(const Relation& r) const {
	if (r.none()) return Relation();

	const size_t lowest_bit = *r.begin();
	for (size_t i = 0; i < presortedSplitRelationsLookup[lowest_bit].size(); ++i) {
		const Relation& s = *(presortedSplitRelationsLookup[lowest_bit][i]);
		if (s.isSubsetOf(r))
			return s;
	}
#ifndef NDEBUG
std::cerr << "Cover set is not exhaustive\n";
#endif
	// TODO: error handling

	assert(false);	// at least one must match
	return Relation();
}
