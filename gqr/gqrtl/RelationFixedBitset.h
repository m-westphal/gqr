// -*- C++ -*-

// Copyright (C) 2008 Matthias Westphal
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

#ifndef RELATIONFIXEDBITSET_H
#define RELATIONFIXEDBITSET_H

#include <cassert>
#include <iostream>
#include <stdint.h>

#include "Relation.h"

namespace gqrtl {

/**
 * Class representing a relation.
 * An array of type T with size nWords is used as the underlying container. Thus sizeof(T)*8*nWords bits can be represented.
 *
 * Most functions of this class are equivalent to the std::bitset functions. However, using std::bitset as relation class turned
 * out to be too slow, since access to underlying words is not _sanely_ possible. Therefore operator< and hash() have to be
 * defined based only on bits, resulting in bad performance.
 * That is why RelationFixedBitset is essentially a rewrite of std::bitset. Note that the class allows to extract
 * the lowest block of type T regardless of the status of higher bits. In contrast (std::bitset::to_ulong() operator only
 * returns lower bits when higher bits are not set).
 * It is further important to have T as a *UNSIGNED integer type* since otherwise the bitshift operators do not work properly.
 */

template<class T, size_t nWords>
class RelationFixedBitset
{
  private:
    /** relation data */
    T data[nWords];

    /** clear data words */
    void zeroData();

    /** table containing the number of set bits for all 16 bit unsigned integers @see count */
    static uint16_t* cardinality_16_bit;
    /** Precompute the table used to accelerate the count() function @see count @see cardinality_16_bit */
    static void precompute_cardinality_16_bit();

  public:
    /** Default Constructor, sets all bits to 0 */
    inline RelationFixedBitset<T, nWords>();

    RelationFixedBitset<T, nWords>(const Relation& r) {
	zeroData();
	assert(this->none());
	for (Relation::const_iterator it = r.begin(); it != r.end(); it++)
		set(*it);
    }

    Relation getRelation() const { // TODO: can we get rid of this?
        Relation res;

	for (size_t i = 0; i < maxSize(); i++)
		if ((*this)[i])
			res.set(i);
	return res;
    }

    class const_iterator {
        private:
            const RelationFixedBitset<T, nWords>* bitset;
            size_t bit;

        public:
            const_iterator& operator++() { // infix ++ operator
                for (bit++; bit < RelationFixedBitset<T, nWords>::maxSize(); bit++)
                    if ((*bitset)[bit])
                        break;

                return *this;
            }
            size_t operator*() const { return bit; }

            bool operator==(const const_iterator& it) const { return bitset == it.bitset && bit == it.bit; }
            bool operator!=(const const_iterator& it) const { return !(*this == it); }

            const_iterator(const RelationFixedBitset<T, nWords>* r, const size_t b) : bitset(r), bit(b) {}
    };

    inline const_iterator begin() const {
        for (size_t i = 0; i < maxSize(); i++)
            if ((*this)[i])
                return const_iterator(this, i);
        return end();
    }
    inline const_iterator end() const { return const_iterator(this, RelationFixedBitset<T, nWords>::maxSize()); }

    /** Converts size_t to a relation. The lowest size_t bits are used to store the number. All other bits are set to 0 */
    inline RelationFixedBitset<T, nWords>(const size_t l);

    /** Initialize static data of the class. @see clean_up*/
    static void init() {
        precompute_cardinality_16_bit();
    }
    /** Get rid of the static data once the data type is not needed any more. @see init */
    static void clean_up() {
	delete[] cardinality_16_bit;
	cardinality_16_bit = NULL;
    }

    /** Return specified word of bitset representation */
    T getWord(const size_t i) const {
	return data[i];
    }

    /** Read the lowest word. This always works, regardless of the state of higher bits */
    inline const size_t lowestWord(void) const;

    /** Returns true iff no bit is set */
    inline bool none(void) const;
    /** Set a bit to 1 */
    inline void set(const size_t p);
    /** Set a bit to 0 */
    inline void unset(const size_t p);
    /** Flip status of all bits*/
    inline RelationFixedBitset<T, nWords>& flip();
    /** Read one bit */
    inline bool operator[](const size_t p) const;
    /** &= operator */
    inline RelationFixedBitset<T, nWords>& operator&=(const RelationFixedBitset<T, nWords>& b);
    /** AND operator, calculates logical AND for all bits */
    inline RelationFixedBitset<T, nWords> operator&(const RelationFixedBitset<T, nWords>& b) const;
    /** |= operator */
    inline RelationFixedBitset<T, nWords>& operator|=(const RelationFixedBitset<T, nWords>& b);
    /** OR operator, calculates logical OR for all bits */
    inline RelationFixedBitset<T, nWords> operator|(const RelationFixedBitset<T, nWords>& b) const;

    /** Logical left shift (towards highest bit) and assign. Shift parameter must be smaller or equal to RELATION_SIZE */
    inline RelationFixedBitset<T, nWords>& operator<<=(const size_t n);
    /** Logical left shift (towards highest bit). Shift parameter must be smaller or equal to RELATION_SIZE */
    inline RelationFixedBitset<T, nWords> operator<<(const size_t n) const;

    /** Logical right shift (towards lowest bit) and assign. Shift parameter must be smaller or equal to RELATION_SIZE */
    inline RelationFixedBitset<T, nWords>& operator>>=(const size_t n);
    /** Logical right shift (towards lowest bit). Shift parameter must be smaller or equal to RELATION_SIZE */
    inline RelationFixedBitset<T, nWords> operator>>(const size_t n) const;

    /** Prefix increment operator. */
    inline RelationFixedBitset<T, nWords>& operator++();
    /** Postfix increment operator. */
    inline RelationFixedBitset<T, nWords> operator++(int);
    /** Prefix decrement operator. */
    inline RelationFixedBitset<T, nWords>& operator--();
    /** Postfix decrement operator. */
    inline RelationFixedBitset<T, nWords> operator--(int);

    /** Equality operator for relations. Two relations are equal iff all bits in both relations have the same status */
    template<class T2, size_t nWords2>
    friend inline bool operator== (const RelationFixedBitset<T2, nWords2>& a, const RelationFixedBitset<T2, nWords2>& b);
    /** Inequality operator */
    template<class T2, size_t nWords2>
    friend inline bool operator!= (const RelationFixedBitset<T2, nWords2>& a, const RelationFixedBitset<T2, nWords2>& b);
    /** Comparison operator, defines an order on relations. */
    template<class T2, size_t nWords2>
    friend inline bool operator< (const RelationFixedBitset<T2, nWords2>& a, const RelationFixedBitset<T2, nWords2>& b);
    /** Comparison operator, defines an order on relations. */
    template<class T2, size_t nWords2>
    friend inline bool operator<= (const RelationFixedBitset<T2, nWords2>& a, const RelationFixedBitset<T2, nWords2>& b);

    /** hash function for relations @return hash value of this instance */
    inline size_t hash(void) const;

    inline bool isSubsetOf(const RelationFixedBitset<T, nWords>& b) const {
        RelationFixedBitset<T, nWords> temp = b;
        temp.flip();
        return (*this & temp).none();
    }

    /** Output stream operator. Print the relation as a sequence of 0, 1 starting with the highest bit. */
    friend std::ostream& operator<<(std::ostream& os, const RelationFixedBitset<T, nWords>& b) {
        for (size_t i = maxSize(); i > 0; i--) {
            if (b[i-1]) {
                os << 1;
            } else {
                os << 0;
            }
        }
        return os;
    }

    /** Count the cardinality of the set of base relations. */
    inline size_t count() const;

    /** get required memory for one instance of RelationFixedBitset. @return memory needed to store one relation */
    static inline size_t memSize(void) { return sizeof(T)*nWords; }

    /** get number of supported base relations. @return number of supported base relations */
    static inline size_t maxSize(void) {
      return sizeof(T)*nWords*8;
    }

    /** get the type and the size of the bitset via classname, for conviniency purposes only */
    typedef T getType;

    static inline size_t getNWords(void)
    {
       return nWords;
    }
};

}

#include "RelationFixedBitset.tcc"

#include <stdint.h>

namespace gqrtl {
	typedef RelationFixedBitset<uint8_t, 1> Relation8;
	typedef RelationFixedBitset<uint16_t, 1> Relation16;
	typedef RelationFixedBitset<uint32_t, 1> Relation32;
}

#endif // RELATIONFIXEDBITSET_H
