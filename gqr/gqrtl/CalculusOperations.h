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

#ifndef CALCULUS_OPERATIONS_H
#define CALCULUS_OPERATIONS_H

#include <cassert>
#include <vector>

#include "Calculus.h"

#include "Splitter.h"

#include "gqrtl/RelationFixedBitset.h"
#include "gqrtl/ValueHeuristic.h"

namespace gqrtl {

/**
* The bare minimum of operations on calculus needed for constraint reasoning
* This abstracts from the usual calculus and is mainly used to specialize
* calculi to templates (as implicitly found in all the old reasoners).
*/

template<class R>
class CalculusOperations {
	private:
		// Original calculus
		const ::Calculus& calculus;

		const R identity;
		const R universal;

		size_t precomputationOffset;
		std::vector<R> precomputedCompositionTable;
		inline void precomputeComposition() {}
		std::vector<R> precomputedConverseTable;
		inline void precomputeConverse() {}

		// For Hogge's method
		/**
		* Array holding the split values needed for computing the composition from
		* the precomputed quadrants.
		* Currently splitValues[0] contains the square root of the precomputed composition table's size.
		*/
		size_t splitValues[2];
		/** Array holding the sizes of the quadrants of precomputed compositions */
		size_t quadrantSizes[2];

		std::vector<size_t> precomputedWeights;
		void precomputeWeights() {}

		std::vector<std::vector<R> > compositionTable;

		void buildCompositionTable() {
			assert(compositionTable.empty());
			compositionTable.resize(getNumberOfBaseRelations());
			for (size_t i = 0; i < getNumberOfBaseRelations(); i++) {
				compositionTable[i].resize(getNumberOfBaseRelations());
				for (size_t j = 0; j < getNumberOfBaseRelations(); j++)
					compositionTable[i][j] = R(calculus.getBaseRelationComposition(i, j));
			}
		}

		std::vector<R> precomputedSplit;
		void precomputeSplit() {};
	public:
		CalculusOperations(const ::Calculus& c) : calculus(c),
			identity(c.getIdentityRelation()), universal(c.getUniversalRelation())
			{
				assert(R::maxSize() >= getNumberOfBaseRelations());

				buildCompositionTable();
				precomputeComposition();
				precomputeConverse();
				precomputeWeights();
				precomputeSplit();
			}
		/** Are all base relations of this calculus serial? */
		inline bool baseRelationsAreSerial() const { return calculus.baseRelationsAreSerial(); }

		// Size of the calculus
		inline const size_t getNumberOfBaseRelations() const { return calculus.getNumberOfBaseRelations(); }

		inline R computeConverse(const R& r) const {
			R res;

			for (typename R::const_iterator it = r.begin(); it != r.end(); ++it)
				res.set(calculus.getBaseRelationConverse(*it)); // Assumption: set is no worse than operator|=
			return res;
		}
		inline R getConverse(const R& r) const { return computeConverse(r); }

		inline R computeComposition(const R& a, const R& b) const {
			R result;

			for (typename R::const_iterator it = a.begin(); it != a.end(); ++it)
				for (typename R::const_iterator it2 = b.begin(); it2 != b.end(); ++it2) {
					result |= compositionTable[*it][*it2];
					if (result == universal)
						return universal;
				}
			return result;
		}


		inline R getComposition(const R& a, const R& b) const { return computeComposition(a,b); }

		inline size_t computeWeight(const R& r) const {
			size_t res = 0;
			for (size_t i = 0; i < getNumberOfBaseRelations(); i++)
				if (r[i])
					res += calculus.getWeightBaseRelation(i);
			return res;
		}
		inline size_t getWeight(const R& r) const { return computeWeight(r); }

		inline const R& getIdentityRelation() const { return identity; }
		inline const R& getUniversalRelation() const { return universal; }

		void printStatistics() const { }

		const ::Calculus& getCalculus() const { return calculus; }

		inline R getNegation(const R& r) const { R res = r; res.flip(); return res & universal; }

		R computeFirstSplit(const R& r) const {
			if (r.none()) return R();

			if (getCalculus().getSplitter() != NULL)
				return getCalculus().getSplitter()->getFirstSplit(r.getRelation());

			// Split to base relations
			ValueHeuristic h(calculus);
			R value;
			{
				typename R::const_iterator it = r.begin();
				size_t first = *it;
				for (++it; it != r.end(); ++it)
					if (h(*it, first)) first = *it;
				value.set(first);
			}

			return value;
		}

		R getFirstSplit(const R& r) const { return computeFirstSplit(r); }

		bool isSplit(const R& r) const {
			if (getCalculus().getSplitter() != NULL)
				return getCalculus().getSplitter()->isSplit(r.getRelation());
			return (r.count() == 1);
		}

		bool operator==(const CalculusOperations& c) const { return calculus == c.calculus; } // TODO: this is quite weak
};

// Template specialization for optimizations:
//	-> precomputations
//	-> optimizations for specific data structures
#ifndef NDEBUG
#include <iostream>
#endif

#include <stdint.h>

// Relation8
// Full precomputations
template<> inline
void CalculusOperations<Relation8>::precomputeComposition() {
	const size_t relations = 1 << getNumberOfBaseRelations();
	assert(relations);

	precomputationOffset = relations;

#ifndef NDEBUG
std::cout << "Precompute full composition table with ";
std::cout << relations << "^2 entries\n";
std::cout << "Using " << relations*relations*Relation8::memSize() << " byte\n";
#endif

	precomputedCompositionTable.resize(relations*relations);
	for (size_t i = 0; i < relations; i++) {
		const Relation8 a(i);
		for (size_t j = 0; j < relations; j++) {
			const Relation8 b(j);
			const size_t offset = precomputationOffset * a.lowestWord() + b.lowestWord();
			precomputedCompositionTable[offset] = computeComposition(a,b);
		}
	}
}

template<> inline
Relation8 CalculusOperations<Relation8>::getComposition(const Relation8& a, const Relation8& b) const {
	const size_t pos = a.lowestWord()*precomputationOffset + b.lowestWord();
	const Relation8& result = precomputedCompositionTable[pos];
	assert(result == computeComposition(a,b));
	return result;
}

template<> inline
void CalculusOperations<Relation8>::precomputeConverse() {
	const size_t relations = 1 << getNumberOfBaseRelations();
#ifndef NDEBUG
std::cout << "Precompute full converse table with ";
std::cout << relations << " entries\n";
std::cout << "Using " << relations*Relation8::memSize() << " byte\n";
#endif
	precomputedConverseTable.resize(relations);
	for (size_t i = 0; i < relations; i++) {
		Relation8 a(i);
		precomputedConverseTable[a.lowestWord()] = computeConverse(a);
	}
}

template<> inline
Relation8 CalculusOperations<Relation8>::getConverse(const Relation8& r) const {
	const Relation8& result = precomputedConverseTable[r.lowestWord()];
	assert(result == computeConverse(r));
	return result;
}

// Full weight table
template<> inline
void CalculusOperations<Relation8>::precomputeWeights() {
	const size_t relations = 1 << getNumberOfBaseRelations();

#ifndef NDEBUG
std::cout << "Precompute full weight table with ";
std::cout << relations << " entries\n";
std::cout << "Using " << relations*sizeof(size_t) << " byte\n";
#endif
	precomputedWeights.resize(relations);
	for (size_t i = 0; i < relations; i++) {
		Relation8 a(i);
		precomputedWeights[i] = computeWeight(a);
	}
}

template<> inline
size_t CalculusOperations<Relation8>::getWeight(const Relation8& r) const {
	const size_t& result = precomputedWeights[r.lowestWord()];
	assert(result == computeWeight(r));
	return result;
}

template<> inline
void CalculusOperations<Relation8>::precomputeSplit() {
	const size_t relations = 1 << getNumberOfBaseRelations();

#ifndef NDEBUG
std::cout << "Precomputing table of splits\n";
std::cout << "Using " << relations*Relation8::memSize() << " byte\n";
#endif
	precomputedSplit.resize(relations);
	for (size_t i = 0; i < relations; i++) {
		const Relation8 r(i);

		precomputedSplit[r.lowestWord()] = computeFirstSplit(r);
	}
}

template<> inline
bool CalculusOperations<Relation8>::isSplit(const Relation8& r) const {
	return precomputedSplit[r.lowestWord()] == r;
}

template<> inline
Relation8 CalculusOperations<Relation8>::getFirstSplit(const Relation8& r) const {
	return precomputedSplit[r.lowestWord()];
}



// Relation16

// Hogge's method for composition
template<> inline
void CalculusOperations<Relation16>::precomputeComposition() {
	splitValues[1] = getNumberOfBaseRelations()/2 + getNumberOfBaseRelations()%2;
	quadrantSizes[0] = 1 << splitValues[1];
	quadrantSizes[1] = 1 << getNumberOfBaseRelations()/2;
	splitValues[0] = quadrantSizes[0] + quadrantSizes[1];
	const size_t size = quadrantSizes[0] + quadrantSizes[1];
	precomputedCompositionTable.resize(size*size);

#ifndef NDEBUG
std::cout << "Precompute Hogge's method for composition\n";
std::cout << "Using " << precomputedCompositionTable.size()*Relation16::memSize() << " byte\n";
#endif

	// first quadrant:
	for (size_t i = 0; i < quadrantSizes[0]; ++i) {
		const size_t offset = size * i;
		for (size_t j = 0; j < quadrantSizes[0]; ++j) {
			const Relation16 a(i);
			const Relation16 b(j);
			const Relation16 result = computeComposition(a, b);
			precomputedCompositionTable[offset + j] = result;
		}
	}
	// second quadrant:
	for (size_t i = 0; i < quadrantSizes[1]; ++i) {
		const size_t offset = size * i + quadrantSizes[0] * size;
		for (size_t j = 0; j < quadrantSizes[0]; ++j) {
			Relation16 r1(i);
			r1 <<= splitValues[1];
			const Relation16 r2(j);
			precomputedCompositionTable[offset + j] = computeComposition(r1, r2);
		}
	}
	// third quadrant:
	for (size_t i = 0; i < quadrantSizes[0]; ++i) {
		size_t offset = size * i + quadrantSizes[0];
		for (size_t j = 0; j < quadrantSizes[1]; ++j) {
			const Relation16 r1(i);
			Relation16 r2(j);
			r2 <<= splitValues[1];
			precomputedCompositionTable[offset + j] = computeComposition(r1, r2);
		}
	}
	// fourth quadrant:
	for (size_t i = 0; i < quadrantSizes[1]; ++i) {
		const size_t offset = size * i + quadrantSizes[0] * size + quadrantSizes[0];
		for (size_t j = 0; j < quadrantSizes[1]; ++j) {
			Relation16 r1(i);
			r1 <<= splitValues[1];
			Relation16 r2(j);
			r2 <<= splitValues[1];
			precomputedCompositionTable[offset + j] = computeComposition(r1, r2);
		}
	}
}

template<> inline
Relation16 CalculusOperations<Relation16>::getComposition(const Relation16& a, const Relation16& b) const {
	const size_t& shift = splitValues[1];
	const size_t& sv0 = splitValues[0];
	const size_t& qs0 = quadrantSizes[0];

	const size_t low_mask = ((size_t) 1 << shift) - (size_t) 1;

	const size_t higher1 = (a >> shift).lowestWord();
	const size_t lower1 = low_mask & a.lowestWord();
	const size_t higher2 = (b >> shift).lowestWord();
	const size_t lower2 = low_mask & b.lowestWord();
	const size_t offset1 = sv0 * lower1;
	const size_t offset2 = sv0 * (higher1 + qs0);
	const size_t offset3 = offset1 + qs0;
	const size_t offset4 = offset2 + qs0;

	const Relation16 result1 = precomputedCompositionTable[offset1 + lower2]
		| precomputedCompositionTable[offset2 + lower2];
	const Relation16 result2 = precomputedCompositionTable[offset3 + higher2]
		| precomputedCompositionTable[offset4 + higher2];

	const Relation16 result = result1 | result2;
	assert(result == computeComposition(a, b));
	return result;
}

// Full converses
template<> inline
void CalculusOperations<Relation16>::precomputeConverse() {
	const size_t relations = 1 << getNumberOfBaseRelations();

#ifndef NDEBUG
std::cout << "Precompute full converse table with ";
std::cout << relations << " entries\n";
std::cout << "Using " << relations*Relation16::memSize() << " byte\n";
#endif
	precomputedConverseTable.resize(relations);
	for (size_t i = 0; i < relations; i++) {
		Relation16 a(i);
		precomputedConverseTable[a.lowestWord()] = computeConverse(a);
	}
}

template<> inline
Relation16 CalculusOperations<Relation16>::getConverse(const Relation16& r) const {
	const Relation16& result = precomputedConverseTable[r.lowestWord()];
	assert(result == computeConverse(r));
	return result;
}

// Full weight table
template<> inline
void CalculusOperations<Relation16>::precomputeWeights() {
	const size_t relations = 1 << getNumberOfBaseRelations();

#ifndef NDEBUG
std::cout << "Precompute full weight table with ";
std::cout << relations << " entries\n";
std::cout << "Using " << relations*sizeof(size_t) << " byte\n";
#endif
	precomputedWeights.resize(relations);
	for (size_t i = 0; i < relations; i++) {
		Relation16 a(i);
		precomputedWeights[i] = computeWeight(a);
	}
}

template<> inline
size_t CalculusOperations<Relation16>::getWeight(const Relation16& r) const {
	const size_t& result = precomputedWeights[r.lowestWord()];
	assert(result == computeWeight(r));
	return result;
}

template<> inline
void CalculusOperations<Relation16>::precomputeSplit() {
	const size_t relations = 1 << getNumberOfBaseRelations();

#ifndef NDEBUG
std::cout << "Precomputing table of splits\n";
std::cout << "Using " << relations*Relation16::memSize() << " byte\n";
#endif
	precomputedSplit.resize(relations);
	for (size_t i = 0; i < relations; i++) {
		const Relation16 r(i);

		precomputedSplit[r.lowestWord()] = computeFirstSplit(r);
	}
}

template<> inline
bool CalculusOperations<Relation16>::isSplit(const Relation16& r) const {
	return precomputedSplit[r.lowestWord()] == r;
}

template<> inline
Relation16 CalculusOperations<Relation16>::getFirstSplit(const Relation16& r) const {
	return precomputedSplit[r.lowestWord()];
}

// Relation32

// Hogge's converses
template<> inline
void CalculusOperations<Relation32>::precomputeConverse() {
	splitValues[1] = getNumberOfBaseRelations()/2 + getNumberOfBaseRelations()%2;
	quadrantSizes[0] = 1 << splitValues[1];
	quadrantSizes[1] = 1 << getNumberOfBaseRelations()/2;

	splitValues[0] = quadrantSizes[0] + quadrantSizes[1];
	const size_t& size = splitValues[0];
	precomputedConverseTable.resize(size);

#ifndef NDEBUG
std::cout << "Precompute Hogge's converse function.\n";
std::cout << "Using " << size*Relation32::memSize() << " byte\n";
#endif
	precomputedConverseTable.resize(size);
	for (size_t i = 0; i < quadrantSizes[0]; i++) {
		const Relation32 r(i);
		precomputedConverseTable[r.lowestWord()] = computeConverse(r);
	}
	for (size_t i = 0; i < quadrantSizes[1]; i++) {
		Relation32 r(i);
		r <<= splitValues[1];
		precomputedConverseTable[quadrantSizes[0]+i] = computeConverse(r);
	}
}

template<> inline
Relation32 CalculusOperations<Relation32>::getConverse(const Relation32& r) const {
	const size_t low_mask = (1 << splitValues[1]) - 1;
	const size_t lower_addr = low_mask & r.lowestWord();
	const Relation32 higher_part = r >> splitValues[1];

	const Relation32& result1 = precomputedConverseTable[lower_addr];
	const Relation32& result2 = precomputedConverseTable[higher_part.lowestWord()+quadrantSizes[0]];
	const Relation32 result = result1 | result2;
	assert(result == computeConverse(r));
	return result;
}


// General Relation type

template<> inline
Relation CalculusOperations<Relation>::getNegation(const Relation& r) const {
	Relation result;
	for (size_t i = 0; i < getNumberOfBaseRelations(); i++)
		if (!r[i]) // TODO: can we optimize this?
			result.set(i);

	return result;
}

}
#endif // CALCULUS_OPERATIONS_H
