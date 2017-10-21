// -*- C++ -*-

// Copyright (C) 2006, 2008	Zeno Gantner
// Copyright (C) 2007		Micha Altmeyer
// Copyright (C) 2010		Stefan Woelfl
// Copyright (C) 2012		Matthias Westphal
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

#ifndef SIMPLE_CSP_H
#define SIMPLE_CSP_H

#include <vector>
#include <string>
#include <cassert>

#include "CSPSparse.h"

// for conversion constructor
class Calculus;
class Relation;

namespace gqrtl {

/**
 * Class representing a binary constraint satisfaction problem (CSP).
 * It does not manage any kind of trailing.
 */
template<class R, class C>
class CSP {
	private:
		/** Reference to underlying calculus representing the qualitative calculus the CSP is formulated in. */
		const C& calculus;

		/** Vector representing a two dimensional array containing the edges between variables. */
		std::vector<R> matrix;

		/** Number of nodes in the CSP */
		const size_t size;

		inline size_t getPos(const size_t x, const size_t y) const { return x*size+y; }

	public:
		/**
		* String containing the name (e.g, parameters used to generate the CSP)
		*/
		const std::string name;

		/**
		* Create a CSP object from scratch. The network is initialized with universal relations and equality constraints between the same vars.
		*
		* @param size the number of nodes (must be > 0)
		* @param calculus pointer to the Calculus object
		*/
		CSP(const size_t s, const C& c, const std::string& n) : calculus(c), size(s), name(n) {
			assert(size > 1);

			// labels between different nodes contain the universal relation:
			matrix = std::vector<R>(size*size, calculus.getUniversalRelation());

			// initialize diagonal label (edge) values:
			for (size_t i = 0; i < size; ++i) {
				/// labels between the same node contain the identity relation:
				matrix[getPos(i,i)] = calculus.getIdentityRelation();
			}
		}

		/** build from sparse CSP with general relation representation */
		CSP(const CSPSparse& csp, const C& nc) : calculus(nc), size(csp.getSize()),
			name(csp.name) {

			matrix.resize(size*size, calculus.getUniversalRelation());
			for (CSPSparse::const_iterator it = csp.begin(); it != csp.end(); ++it) {
				const std::pair<size_t, size_t>& var = it->first;
				const Relation& r = it->second;
				setConstraint(var.first, var.second, R(r));
			}
		}

		/** Conversion constructor: allows different Calculus classes */
		CSP(const CSP<Relation, Calculus>& csp, C& nc) : calculus(nc), size(csp.getSize()),
			name(csp.name) {

			matrix.resize(size*size);
			for (size_t i = 0; i < size; ++i)
				for (size_t j = i; j < size; ++j)
					setConstraint(i, j, R(csp.getConstraint(i,j)));
		}

		/**
		*  Set a constraint
		*
		* @param x a node
		* @param y another node
		* @param r the relation between node x and node y
		*/
		void setConstraint(const size_t x, const size_t y, const R r) {
			matrix[getPos(x,y)] = r;
			matrix[getPos(y,x)] = calculus.getConverse(r);
		}

		/** Get a constraint @param x a node @param y another node @return the relation between x and y */
		inline const R& getConstraint(const size_t x, const size_t y) const {
			// network should be normalized
			assert(matrix[getPos(y,x)] == calculus.getConverse(matrix[getPos(x,y)]));
			return matrix[getPos(x,y)];
		}

		/** Get the size of the CSP @return the number of nodes in the CSP */
		const size_t getSize() const { return size; }

		/** Get reference to Calculus object */
		const C& getCalculus() const { return calculus; }

		/** Equality operator. Two CSPs are equal if they have the same size, reference the same calculus and feature the same constraints */
		inline bool operator== (const CSP<R, C>& b) const {
			if (size == b.size
				&& calculus == b.calculus
				&& matrix == b.matrix)
				return true;
			return false;
		}

};

}

#endif // CSP_H
