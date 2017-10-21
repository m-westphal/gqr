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

#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <algorithm>
#include <cassert>
#include <limits>

#include <ostream>

/**
 * Class representing a relation.
 */

class Relation {
	private:
		typedef std::vector<size_t> container;

		container elements;
	public:
		typedef container::const_iterator const_iterator;

		inline const_iterator begin() const { return elements.begin(); }
		inline const_iterator end() const { return elements.end(); }

		/** Default Constructor, sets all bits to 0 */
		Relation() {}

		// TODO: can we get rid of this
		Relation getRelation() const {
			Relation a = *this;
			return a;
		}

		/** Returns true iff no bit is set */
		inline bool none(void) const { return elements.empty(); }

		/** Set a bit to 1 */
		inline void set(const size_t p) {
			container::iterator it = std::lower_bound(elements.begin(), elements.end(), p);
			if (it != elements.end() && *it == p) return;
			elements.insert(it, p);
		}

		/** Set a bit to 0 */
		inline void unset(const size_t p) {
			container::iterator it = std::lower_bound(elements.begin(), elements.end(), p);
			if (it != elements.end() && *it == p) elements.erase(it);
		}

		/** Read one bit */
		inline bool operator[](const size_t p) const {
			return std::binary_search(elements.begin(), elements.end(), p);
		}

		inline Relation operator&(const Relation& b) const {
			Relation r;
			r.elements.reserve(std::max(this->count(),b.count()));

			container::const_iterator it1 = elements.begin();
			container::const_iterator it2 = b.elements.begin();
			while(it1 != elements.end() && it2 != b.elements.end()) {
				if (*it1 < *it2) { ++it1; }
				else if (*it1 == *it2) { r.elements.push_back(*it1); ++it1; ++it2; }
				else { assert (*it1 > *it2); ++it2; }
			}
			return r;
		}

		inline Relation& operator|=(const Relation& b) {
			// TODO: improve this (note, with container as vector
			// iterators are probably invalided when altering
			// container)
			Relation r = *this | b;
			elements = r.elements;
			return *this;
		}

		inline Relation operator|(const Relation& b) const {
			Relation r;
			r.elements.reserve(this->count()+b.count());

			container::const_iterator it1 = elements.begin();
			container::const_iterator it2 = b.elements.begin();
			while(it1 != elements.end() && it2 != b.elements.end()) {
				if (*it1 < *it2) { r.elements.push_back(*it1); ++it1; }
				else if (*it1 == *it2) { r.elements.push_back(*it1); ++it1; ++it2; }
				else { assert (*it1 > *it2); r.elements.push_back(*it2); ++it2; }
			}
			// potentially remaining elements (note: at least one is empty)
			for (; it1 != elements.end(); ++it1) r.elements.push_back(*it1);
			for (; it2 != b.elements.end(); ++it2) r.elements.push_back(*it2);
			return r;
		}

		inline bool isSubsetOf(const Relation b) const {
			if (count() > b.count()) return false;	// heuristic
			return std::includes(b.elements.begin(), b.elements.end(), elements.begin(), elements.end());
		}

		inline bool operator==(const Relation& b) const { return (elements == b.elements); }

		inline bool operator!=(const Relation& b) const { return !(*this == b); }

		/** Output stream operator; lists elements of the relation */
		friend std::ostream& operator<<(std::ostream& os, const Relation& b) {
			os << "{";
			for (const_iterator it = b.begin(); it != b.end(); it++)
				os << " " << *it;
			os << " }";

			return os;
		}

		/** Maximum relation size */
		static const size_t maxSize() { return std::numeric_limits<size_t>::max(); }

		/** Count the cardinality of the set */
		inline size_t count() const { return elements.size(); }

		// Compatibility with RelationFixedBitset
		static void init() {}
		static void clean_up() {}
};

#endif // RELATION_H
