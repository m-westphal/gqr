// -*- C++ -*-

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

#ifndef SPARSE_CSP_H
#define SPARSE_CSP_H

#include <map>
#include <string>
#include <cassert>
#include <utility>

#include "Relation.h"
#include "Calculus.h"

/**
 * Class representing a sparse binary constraint satisfaction problem (CSP).
 * This is used for reading CSPs. It does not produce complete graphs.
 */
class CSPSparse {
	private:
		/** Number of nodes in the CSP */
		const size_t size;

		/** Reference to underlying calculus representing the qualitative calculus the CSP is formulated in. */
		const Calculus& calculus;

		/** map containing relations between variables. */
		std::map<std::pair<size_t, size_t>, Relation> network;

	public:
		typedef std::map<std::pair<size_t,size_t>, Relation>::const_iterator const_iterator;

		inline const_iterator begin() const { return network.begin(); }
		inline const_iterator end() const { return network.end(); }

		/**
		* String containing the name (e.g, parameters used to generate the CSP)
		*/
		const std::string name;

		/**
		* Create a new CSP. Network is empty except for all equality constraints
		*
		* @param size the number of nodes (must be > 0)
		* @param calculus pointer to the Calculus object
		*/
		CSPSparse(const size_t s, const Calculus& c, const std::string& n) : size(s), calculus(c), name(n) {
			assert(size > 1);

			// initialize diagonal label (edge) values:
			for (size_t i = 0; i < size; ++i) {
				/// labels between the same node contain the identity relation:
				network.insert(std::make_pair(std::make_pair(i,i), calculus.getIdentityRelation()));
			}
		}

		/**
		*  add a constraint
		*  Takes care of converse relations
		*/
		void addConstraint(size_t x, size_t y, Relation r) {
			if (y < x) {
				std::swap(x,y);
				r = calculus.getConverse(r);
			}
			assert(x < y);
			const std::pair<size_t,size_t> t = std::make_pair(x,y);

			std::map<std::pair<size_t,size_t>, Relation>::iterator it = network.find(t);
			if (it == network.end())
				network.insert( std::make_pair(t, r) );
			else
				it->second = it->second & r;
		}

		/** Get the size of the CSP @return the number of nodes in the CSP */
		const size_t getSize() const { return size; }
};

#endif // SPARSE_CSP_H
