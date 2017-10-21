// -*- C++ -*-

// Copyright (C) 2008,2012 Matthias Westphal
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

#ifndef WEIGHTED_TRIPLE_ITERATOR_H
#define WEIGHTED_TRIPLE_ITERATOR_H

#include <vector>
#include "Tuple.h"

#include "PriorityQueue.h"

/**
 * Algebraic closure using a weighted queue as described by van Beek and Manchak:
 *
 * Peter van Beek, Dennis W. Manchak:
 * The Design and an Experimental Analysis of Algorithms for Temporal Reasoning.
 * Journal of Artificial Intelligence Research, volume 4, pages 1-18, 1996
 */

namespace gqrtl {

template<class R, class N> class WeightedTripleIterator {
  private:
    /** The revise function. Performs R_{ik} = R_{ik} cap ( R_{ij} comp R_{jk} ). @returns true iff R_{ik} changed. */
    static inline bool revise(N& csp, const size_t i, const size_t j, const size_t k);

  protected:
    PriorityQueue queue;

    inline void add_to_queue(size_t i, size_t j, const N& csp);

    /** pc1 based on a priority queue */
    std::vector<Tuple> pc1(N& csp);

    std::vector<Tuple> describeTriple(size_t i, size_t j, size_t k) const;

  public:
    /** Empty constructor */
    WeightedTripleIterator() {}

    /** Enforce algebraic closure to CSP. */
    std::vector<Tuple> enforce(N& csp);

    /** Enforce algebraic closure to CSP. Assume that csp \ R_{ij} is algebraically closed. */
    std::vector<Tuple> enforce(N& csp, const size_t i, const size_t j);
};

}

#include "gqrtl/WeightedTripleIterator.tcc"

#endif // WEIGHTED_TRIPLE_ITERATOR_H
