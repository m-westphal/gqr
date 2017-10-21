// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
// Copyright (C) 2007, 2008 Matthias Westphal
// Copyright (C) 2006, 2007, 2010 Stefan Woelfl
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

#include <cassert>
#include <iostream> // TODO: make class silent

#include "Calculus.h"
#include "Stringtools.h" // TODO: should we really deal with strings in this class?

#include "Splitter.h"

/**
 * Implementation of the Calculus class template
 *
 * Parts of the code in this file are taken from Bernhard Nebel's reasoner for
 * Allen's interval algebra, which is again based on work by Peter van Beek.
 * The code is used with permission from the authors.
 */

static bool decideBaseRelationsSerial(const std::vector<std::vector<Relation> >& ct) {
	for (size_t i = 0; i < ct.size(); i++)
		for (size_t j = 0; j < ct[i].size(); j++)
			if (ct[i][j].none())
				return false;
	return true;
}

static Relation buildIdentityRelation(const size_t id) {
	Relation rel;
	rel.set(id);
	return rel;
}

static Relation buildUniversalRelation(const size_t br) {
	Relation rel;
	for (size_t i = 0; i < br; i++)
		rel.set(i);
	return rel;
}

Calculus::Calculus(const std::string& name,
	const std::vector<std::string>& br,
	const size_t& identity,
	const std::vector<size_t>& conv,
	const std::vector<std::vector<Relation> >& comp,
	const std::vector<size_t>& weights) :
	calculusName(name), baseRelationNames(br), identity(identity), converseTable(conv),
	compositionTable(comp),
	baseRelationsSerial(decideBaseRelationsSerial(compositionTable)),
	relationWeights(weights),
	universalRelation(buildUniversalRelation(getNumberOfBaseRelations())),
	identityRelation(buildIdentityRelation(identity)),
	splitter(NULL)
	{

	assert(identity < getNumberOfBaseRelations());

	assert(!converseTable.empty());
	assert(!compositionTable.empty());
	assert(converseTable.size() == compositionTable.size());

	#ifndef NDEBUG
	// Check for complete composition table matrix
	for (size_t i = 0; i < converseTable.size(); i++)
		for (size_t j = i+1; j < converseTable.size(); j++)
			assert(compositionTable[i].size() == compositionTable[j].size());
	#endif
}

Calculus::~Calculus() {
	delete splitter;
}

std::string Calculus::relationToString(const Relation& rel) const {
	std::string res = "(";

	for (Relation::const_iterator it = rel.begin(); it != rel.end(); it++)
		res += std::string(" ")+getBaseRelationName(*it);

	return res+" )";
}

bool Calculus::checkConverseTable() const {
	std::cout << "Checking integrity of the converse table ..." << std::endl;
	// conv(Id) = Id
	if (identity != getBaseRelationConverse(identity)) {
		const std::string& name_identity = getBaseRelationName(identity);
		std::cout << "Checking integrity of the converse table ..." << std::endl;
		std::cout << "conv(" << name_identity << ") = "
			<< name_identity << " ?\t  ";
		std::cout << "false" << std::endl << std::endl;
		return false;
	}

	// conv(conv(a)) == a ?
	for (size_t i = 0; i < getNumberOfBaseRelations(); ++i) {
		const size_t c_i = getBaseRelationConverse(i);
		const size_t c_c_i = getBaseRelationConverse(c_i);

		if (i != c_c_i) {
			const std::string& name = getBaseRelationName(i);
			const std::string& cname = getBaseRelationName(c_i);
			const std::string& ccname = getBaseRelationName(c_c_i);

			std::cout << "Checking converses of converses ..." << std::endl;
			std::cout << "conv(conv(" << name << ")) = " << name << " ?  ";
			std::cout << "false" << std::endl;
			std::cout << "\tconv(" << name << ") = " << cname << ", conv(" << cname << ") = " << ccname << std::endl;
			return false;
		}
	}

	return true;
}

Relation Calculus::encodeRelation(const std::string& names) const {
	Relation result;
	assert(result.none());

	const std::vector<std::string> v = Stringtools::split(names, ' ');

	for (std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); it++) {
		size_t br = getNumberOfBaseRelations();
		for (size_t i = 0; i < getNumberOfBaseRelations(); i++)
			if (getBaseRelationName(i) == *it) {
				br = i;
				break;
			}
		if (br == getNumberOfBaseRelations()) {
			#ifndef NDEBUG
			// Stefan: this does not belong here, it should be placed in an interface class
			std::cerr << "Warning: unknown relation '" << (*it) << "'" << std::endl;
			#endif
			continue;
		}

		result.set(br);
	}

	return result;
}

bool Calculus::checkCompositionTable() const {
	std::cout << "Checking integrity of the composition table ..." << std::endl;
	// id o b == b and b o id == b ?
	for (size_t i = 0; i < getNumberOfBaseRelations(); ++i) {
		const Relation res_l = getBaseRelationComposition(i, identity);
		const Relation res_r = getBaseRelationComposition(identity, i);

		const std::string& name = getBaseRelationName(i);
		const std::string& resname_l = relationToString(res_l);
		const std::string& resname_r = relationToString(res_r);

		Relation rel;
		rel.set(i);

		if (res_l != rel) {
			std::cout << "Checking id o b = b ..." << std::endl;
			std::cout << "id o " << name << " = " << name << " ?\t";
			std::cout << "false" << std::endl;
			std::cout << "Composition result: " << resname_l << std::endl;
			return false;
		}

		if (res_r != rel) {
			std::cout << "Checking b o id = b ..." << std::endl;
			std::cout <<  name << " o id" << " = " << name << " ?\t";
			std::cout << "false" << std::endl;
			std::cout << "Composition result: " << resname_r << std::endl;
			return false;
		}
	}

	// conv(a o b) = conv(b) o conv(a) ?
	for (size_t i = 0; i < getNumberOfBaseRelations(); ++i) {
		for (size_t j = 0; j < getNumberOfBaseRelations(); ++j) {
			const Relation resrel_l = getConverse(getBaseRelationComposition(i, j));
			const Relation resrel_r = getBaseRelationComposition(
				getBaseRelationConverse(j), getBaseRelationConverse(i)
			);

			if (resrel_l != resrel_r) {
				const std::string& name_i = getBaseRelationName(i);
				const std::string& name_j = getBaseRelationName(j);
				std::cout << "Checking integrity of the composition table ..." << std::endl;
				std::cout << "Checking conv(a o b) = conv(b) o conv(a) ..." << std::endl;
				std::cout << name_i << " " << name_j << ": ";
				std::cout << "false\n";
				std::cout << name_i << " COMP " << name_j << " IS "
					<< relationToString(getBaseRelationComposition(i,j)) << std::endl;
				std::cout << getBaseRelationName(getBaseRelationConverse(j))
					<< " COMP "
					<< getBaseRelationName(getBaseRelationConverse(i))
					<< " IS " << relationToString(resrel_r) << std::endl;
				std::cout << "conv( " << relationToString(getBaseRelationComposition(i,j))
					<< " ) = " << relationToString(resrel_l) << " != " << relationToString(resrel_r) << std::endl;
				return false;
			}
		}
	}

	// conv(a) \in b COMP c <=> conv(c) \in a COMP b
	for (size_t i = 0; i < getNumberOfBaseRelations(); ++i) {
		const size_t c_i = getBaseRelationConverse(i);

		for (size_t j = 0; j < getNumberOfBaseRelations(); ++j) {
			const Relation& resrel_r = getBaseRelationComposition(i, j);

			for (size_t k = 0; k < getNumberOfBaseRelations(); ++k) {
				const Relation& resrel_l = getBaseRelationComposition(j, k);
				const size_t c_k = getBaseRelationConverse(k);

				if (resrel_l[c_i] != resrel_r[c_k]) {
					Relation rel_i, rel_j, rel_k;

					rel_i.set(i);
					rel_j.set(j);
					rel_k.set(k);

					std::cout << "Checking integrity of the composition table ..." << std::endl;
					std::cout << "Checking conv(a) in b o c iff conv(c) in a o b ..." << std::endl;
					std::cout << relationToString(getConverse(rel_i)) << " in " << relationToString(rel_j)
						<< " o " << relationToString(rel_k) << ": " << resrel_l[c_i] << std::endl;
					std::cout << relationToString(getConverse(rel_k)) << " in " << relationToString(rel_i)
						<< " o " << relationToString(rel_j) << ": " << resrel_r[c_k] << std::endl ;
					std::cout << relationToString(resrel_l) << " cmps to " << relationToString(resrel_r) << ": ";
					std::cout << "false" << std::endl;
					return false;
				}
			}
		}
	}

	return true;
}

void Calculus::setSplitter(Splitter* s) {
	assert(splitter == NULL);
	assert(s != NULL);
	splitter = s;
}
