// -*- C++ -*-

// Copyright (C) 2012 Matthias Westphal
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

#ifndef VALUE_HEURISTIC
#define VALUE_HEURISTIC

#include "Calculus.h"

namespace gqrtl {

// Standard value heuristic. Prefer non-constraining values to maximize chance of satisfiability.
class ValueHeuristic {
	private:
		const ::Calculus& calculus;

	public:
		inline ValueHeuristic(const ::Calculus& c) : calculus(c) {}

		// order for base relations: a better than b?
		inline bool operator() (const size_t& a, const size_t& b) const {
			// prefer base relations with high weight
			if (calculus.getWeightBaseRelation(a) > calculus.getWeightBaseRelation(b))
				return true;

			// lexicographic order
			return (b > a);
		}

		// order for relations: a better than b?
		inline bool operator() (const Relation& a, const Relation& b) const {
			// prefer relations with high weight
			if (calculus.getWeight(a) > calculus.getWeight(b))
				return true;
			if (calculus.getWeight(a) < calculus.getWeight(b))
				return false;

			// prefer larger relations
			if (a.count() > b.count())
				return true;
			if (a.count() < b.count())
				return false;

			for (size_t i = calculus.getNumberOfBaseRelations()-1; ; i--) {
				if (!a[i] && b[i]) // TODO move into relation class?
					return true;
				if (a[i] && !b[i]) // TODO move into relation class?
					return false;

				if (i == 0) {
					assert(a == b);
					return false;
				}
			}


			// dead code: should never be reached
			assert(false);
			return false;
		}
};

}

#endif
