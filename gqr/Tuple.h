// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
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

#ifndef TUPLE_H
#define TUPLE_H

#include <ostream>
#include <limits>
#include <cstring>

/** Pair of two index values, representing a label or a path in a CSP.
 *
 *  There is no default constructor, as that could affect performance if
 *  a Tuple is declared, but not explicitly initialized somewhere.
 */

class Tuple {
	public:
		size_t x;
		size_t y;

		/** Function to compute hash value */
		static size_t hash(const Tuple &key) {
			return 17 * key.x + key.y;
		}

		/** Equality operator */
		inline bool operator== (const Tuple& t) const {
			return ((t.x == x) && (t.y == y));
		}
		inline bool operator!= (const Tuple& t) const { return !(*this == t); }

		/** Constructor */
		Tuple(const size_t i, const size_t j) : x(i), y(j) {}

		friend std::ostream& operator <<(std::ostream& stream, const Tuple& t) {
			stream << "( " << t.x << ", " << t.y << ")";
			return stream;
		}
};
/** Used to signal unset Tuple */
static Tuple InvalidTuple = Tuple(std::numeric_limits<std::size_t>::max(), std::numeric_limits<std::size_t>::max());

#endif // TUPLE_H
