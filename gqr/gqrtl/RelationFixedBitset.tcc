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

#include <cassert>

namespace gqrtl {

template<class T, size_t nWords>
inline void RelationFixedBitset<T, nWords>::zeroData() {
    for (size_t i = 0; i < nWords; ++i)
        data[i] = 0;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>::RelationFixedBitset()
{
	zeroData();
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>::RelationFixedBitset(const size_t l) {
    data[0] = l;
    for (size_t i = 1; i < nWords; ++i) {
        data[i] = 0;
    }
}

template<class T, size_t nWords>
inline const size_t RelationFixedBitset<T, nWords>::lowestWord(void) const {
    return (size_t) data[0];
}

template<class T, size_t nWords>
inline bool RelationFixedBitset<T, nWords>::none(void) const {
    for (size_t i = 0; i < nWords; ++i) {
        if (data[i] != 0) {
            return false;
        }
    }
    return true;
}

template<class T, size_t nWords>
inline void RelationFixedBitset<T, nWords>::set(const size_t p) {
    assert(p < maxSize());

    if (nWords == 1) {
        data[0] |= ( (T) 1 << p);
    } else {
        const size_t pos = p/(sizeof(T)*8);
        const size_t shift = p%(sizeof(T)*8);

        data[pos] |= ((T) 1 << shift);
    }
    assert((*this)[p]);
}

template<class T, size_t nWords>
inline void RelationFixedBitset<T, nWords>::unset(const size_t p) {
    assert(p < maxSize());

    if (nWords == 1) {
        data[0] &= (T) ~((T) 1 << p);
    } else {
        const size_t pos = p/(sizeof(T)*8);
        const size_t shift = p%(sizeof(T)*8);

        data[pos] &= (T) ~((T) 1 << shift);
    }
    assert(!(*this)[p]);
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::flip() {
    for (size_t i = 0; i < nWords; ++i) {
        data[i] = ~data[i];
    }
    return *this;
}

template<class T, size_t nWords>
inline bool RelationFixedBitset<T, nWords>::operator[](const size_t p) const {
    assert(p < maxSize());

    if (nWords == 1) {
        return ((data[0] >> p) & (T) 1) != 0;
    } else {
        const size_t pos = p/(sizeof(T)*8);
        const size_t shift = p%(sizeof(T)*8);

        return ((data[pos] >> shift) & (T) 1) != 0;
    }
}

template<class T, size_t nWords>
inline bool operator==(const RelationFixedBitset<T, nWords>& a, const RelationFixedBitset<T, nWords>& b) {
    for (size_t i = 0; i < nWords; ++i) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

template<class T, size_t nWords>
inline bool operator!=(const RelationFixedBitset<T, nWords>& a, const RelationFixedBitset<T, nWords>& b) {
    return !(a == b);
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::operator&=(const RelationFixedBitset<T, nWords>& b) {
    for (size_t i = 0; i < nWords; ++i) {
        data[i] &= b.data[i];
    }
    return *this;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords> RelationFixedBitset<T, nWords>::operator&(const RelationFixedBitset<T, nWords>& b) const {
    RelationFixedBitset r = *this;
    r &= b;
    return r;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::operator|=(const RelationFixedBitset<T, nWords>& b) {
    for(size_t i = 0; i < nWords; ++i) {
        data[i] |= b.data[i];
    }
    return *this;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords> RelationFixedBitset<T, nWords>::operator|(const RelationFixedBitset<T, nWords>& b) const {
    RelationFixedBitset<T, nWords> r = *this;
    r |= b;
    return r;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::operator<<=(const size_t n) {
    assert(n <= maxSize());
    if (nWords == 1) {
        data[0] <<= n;
    } else {
        const size_t word_shift = n / (sizeof(T)*8);
        const size_t offset = n % (sizeof(T)*8);

        if (offset == 0) {
            for (size_t i = nWords-1; i > word_shift; --i) {
                data[i] = data[i-word_shift];
            }
            data[word_shift] = data[0];
        } else {
            const size_t inv_offset = sizeof(T)*8 - offset;

            for (size_t i = nWords-1; i > word_shift; --i) {
                data[i] = ((data[i-word_shift] << offset) | (data[i-word_shift-1] >> inv_offset));
            }
            data[word_shift] = data[0] << offset;
        }
        for (size_t i = 0; i < word_shift; ++i) {
            data[i] = 0;
        }
    }

    return *this;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords> RelationFixedBitset<T, nWords>::operator<<(const size_t n) const {
    RelationFixedBitset<T, nWords> r = *this;
    r <<= n;
    return r;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::operator>>=(const size_t n) {
    assert(n <= maxSize());
    if (nWords == 1) {
        data[0] >>= n;
    } else {
        const size_t word_shift = n / (sizeof(T)*8);
        const size_t offset = n % (sizeof(T)*8);
        const size_t last = nWords - word_shift - 1;

        if (offset == 0) {
            for (size_t i = 0; i <= last; ++i) {
                data[i] = data[i+word_shift];
            }
        } else {
            const size_t sub_offset = sizeof(T)*8 - offset;

            for (size_t i = 0; i < last; ++i) {
                data[i] = ((data[i+word_shift] >> offset) | (data[i+word_shift+1] << sub_offset));
            }
            data[last] = data[nWords-1] >> offset;
        }
        for (size_t i = nWords-1; i > last; --i) {
            data[i] = 0;
        }
    }
    return *this;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords> RelationFixedBitset<T, nWords>::operator>>(const size_t n) const {
    RelationFixedBitset<T, nWords> r = *this;
    r >>= n;
    return r;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::operator++() {
    for (size_t i = 0; i < nWords; ++i) {
        if (++data[i] != 0) { // if no overflow
            break;            // ... we are done!
        }
    }
    return *this;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords> RelationFixedBitset<T, nWords>::operator++(int) {
    RelationFixedBitset<T, nWords> temp = *this;
    ++*this;
    return temp;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords>& RelationFixedBitset<T, nWords>::operator--() {
    for (size_t i = 0; i < nWords; ++i) {
        if (data[i]-- != 0) { // if no overflow
            break;            // ... we are done!
        }
    }
    return *this;
}

template<class T, size_t nWords>
inline RelationFixedBitset<T, nWords> RelationFixedBitset<T, nWords>::operator--(int) {
    RelationFixedBitset<T, nWords> temp = *this;
    --*this;
    return temp;
}

template<class T, size_t nWords>
inline bool operator<(const RelationFixedBitset<T, nWords>& a, const RelationFixedBitset<T, nWords>& b) {
    for (size_t i = nWords-1; i > 0; --i) {
        if (a.data[i] < b.data[i]) {
            return true;
        } else if (a.data[i] > b.data[i]) {
            return false;
        }
    }
    return a.data[0] < b.data[0];
}

template<class T, size_t nWords>
inline bool operator<=(const RelationFixedBitset<T, nWords>& a, const RelationFixedBitset<T, nWords>& b) {
    for (size_t i = nWords-1; i > 0; --i) {
        if (a.data[i] < b.data[i]) {
            return true;
        } else if (a.data[i] > b.data[i]) {
            return false;
        }
    }
    return a.data[0] <= b.data[0];
}

template<class T, size_t nWords>
inline size_t RelationFixedBitset<T, nWords>::hash(void) const {
    if (nWords == 1) {
        return data[0];
    }

    size_t hash = 0;

    for (size_t i = 0; i < nWords; ++i) {
        hash ^= (size_t) data[i];
    }

    return hash;
}

/**
 * Precompute 16 method
 *
 * See http://www-db.stanford.edu/~manku/bitcount/bitcount.html
 */
template<class T, size_t nWords>
inline size_t RelationFixedBitset<T, nWords>::count() const {
    size_t numberOfBits = 0;

    for (size_t i = 0; i < nWords; ++i) {
        T rdata = data[i];
	for (size_t j = 0; j <= ((sizeof(T)*8-1) / 16); j++) {
            numberOfBits += cardinality_16_bit [ (int) (rdata & (T) 0x0000ffffU)];
	    rdata = rdata >> 16;
	}
    }
    return numberOfBits;
}

template<class T, size_t nWords>
uint16_t* RelationFixedBitset<T, nWords>::cardinality_16_bit = 0;

template<class T, size_t nWords>
void RelationFixedBitset<T, nWords>::precompute_cardinality_16_bit() {
    RelationFixedBitset<T, nWords>::cardinality_16_bit = new uint16_t[1U << 16]; // deleted in clean_up()

    for (size_t i = 0; i < (1U << 16); ++i) {
        cardinality_16_bit[i] = 0;
        uint16_t n = (uint16_t) i;
        while (n) {
            cardinality_16_bit[i] += n & 1U;
            n >>= 1;
        }
    }
}

}
