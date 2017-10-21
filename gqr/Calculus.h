// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
// Copyright (C) 2007, 2008, 2012 Matthias Westphal
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

#ifndef CALCULUS_H
#define CALCULUS_H

#include <vector>
#include <string>

#include "Relation.h"

class Splitter;

/** Class representing a relation calculus (relational algebra). */
class Calculus {
	private:
		/** Name of the calculus */
		const std::string calculusName;

		/** Names of the base relations */
		const std::vector<std::string> baseRelationNames;

		/** The identity base relation of the calculus */
		const size_t identity;

		/** converses mapping */
		const std::vector<size_t> converseTable;

		/** Two-dimensional array containing the composition result of base relations. */
		const std::vector<std::vector<Relation> > compositionTable;

		/** Calculus property, base relations are serial; see Ligozat and Renz: "What is a qualitative calculus? A general framework" */
		const bool baseRelationsSerial;

		/** Array of weights of the base relations */
		const std::vector<size_t> relationWeights;

		// Universal relation
		const Relation universalRelation;

		// Identity relation
		const Relation identityRelation;

		// Splitter (e.g., tractability information */
		Splitter* splitter;
	public:
		/**
		* Constructor
		*
		* @param name                  name of the calculus
		* @param relations             list of strings representing the base relations
		* @param converse              converse table as a list of converses
		* @param compositionTable      composition table
		* @param identity              the identity base relation
		* @param weights               list of base relations' weights
		*/
		Calculus(const std::string& name,
			const std::vector<std::string>& relations,
			const size_t& identity,
			const std::vector<size_t>& converse,
			const std::vector<std::vector<Relation> >& compositionTable,
			const std::vector<size_t>& weights
			);

			/** Destructor */
			~Calculus();


		// Get the identity base relation
		size_t getIdentityBaseRelation() const { return identity; }

		const Relation& getIdentityRelation() const { return identityRelation; }

		const Relation& getUniversalRelation() const { return universalRelation; }

		// Get the converse of a base relation
		inline size_t getBaseRelationConverse(const size_t i) const { return converseTable[i]; }

		// Get the converse of an arbitrary relation
		inline Relation getConverse(const Relation& r) const {
			Relation res;
			for (Relation::const_iterator it = r.begin(); it != r.end(); it++)
				res.set(getBaseRelationConverse(*it));
			return res;
		}


		// Get the composition of two base relations
		inline const Relation& getBaseRelationComposition(const size_t i, const size_t j) const { return compositionTable[i][j]; }

		// Get the composition of two (arbitrary) relations
		inline Relation getComposition(const Relation& a, const Relation& b) const {
			Relation res;
			for (Relation::const_iterator it1 = a.begin(); it1 != a.end(); it1++)
				for (Relation::const_iterator it2 = b.begin(); it2 != b.end(); it2++) {
					res |= getBaseRelationComposition(*it1, *it2);
					if (res.count() == getNumberOfBaseRelations()) // universal relation
						return res;	// no need to continue computation
				}
			return res;
		}

		/**
		* Convert a string representation of a relation to its binary representation
		*
		* @param s string containing zero or more relation names
		* @return the bit representation of the relation described by s
		*/
		Relation encodeRelation(const std::string& s) const;

		/**
		* Get the weight of a given relation.
		* Only the empty relation has a weight of 0,
		* and only the universal relation has a weight of maxWeight.
		*
		* @param r a relation
		* @return the relation's weight, between 0 and maxWeight
		*/
		size_t getWeight(const Relation& r) const {
			size_t res = 0;
			for (Relation::const_iterator it = r.begin(); it != r.end(); it++)
				res += relationWeights[*it];
			return res;
		}

		size_t getWeightBaseRelation(const size_t i) const { return relationWeights[i]; }

		/**
		* Return the set of mappings from the base relations' names to
		* their internal representation
		*
		* @return pointer to baseRelationNames
		*/
		const std::string& getBaseRelationName(const size_t i) const { return baseRelationNames[i]; }

		/**
		* Return the number of base relations of this calculus
		*
		* @return the number of base relations
		*/
		inline size_t getNumberOfBaseRelations() const { return converseTable.size(); }

		/**
		* Convert to string representation
		*
		* @param r bit representation of the relation
		* @return a string representing the encoded relation
		*/
		std::string relationToString(const Relation& r) const;

		// check converse function
		// @return true if no error can be found in syntactical definition
		bool checkConverseTable() const;
		// check composition function
		// @return true if no error can be found in syntactical definition
		bool checkCompositionTable() const;

		/** Are all base relations of this calculus serial? */
		inline bool baseRelationsAreSerial() const { return baseRelationsSerial; }

		void setSplitter(Splitter* s);

		const Splitter* getSplitter() const { return splitter; }
		const std::string& getName() const { return calculusName; }

		bool operator==(const Calculus& c) const { return calculusName == c.calculusName; } // TODO: this is quite weak
};

#endif // CALCULUS_H
