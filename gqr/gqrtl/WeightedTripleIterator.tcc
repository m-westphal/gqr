// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
// Copyright (C) 2008 Matthias Westphal
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
#include <algorithm>

namespace gqrtl {

template<class R, class N>
inline
bool WeightedTripleIterator<R,N>::revise(N& csp, const size_t i, const size_t j, const size_t k) {
    const R& oldEdge = csp.getConstraint(i, k);
    const R newEdge = oldEdge & csp.getCalculus().getComposition(csp.getConstraint(i, j), csp.getConstraint(j, k));

    if (oldEdge != newEdge) {
	csp.setConstraint(i, k, newEdge);
	return true;
    }

    return false;
}

template<class R, class N>
inline
std::vector<Tuple> WeightedTripleIterator<R,N>::describeTriple(size_t i, size_t j, size_t k) const {
	std::vector<Tuple> res;
	res.reserve(3);
	res.push_back(Tuple(i,j));
	res.push_back(Tuple(i,k));
	res.push_back(Tuple(j,k));
	return res;
}

template<class R, class N>
inline
void WeightedTripleIterator<R,N>::add_to_queue(size_t i, size_t j, const N& csp) {
    assert (i != j); // adding i,i would imply a change happened to ( = ); which could only be R(0U)
    if (j < i)
        std::swap(i, j);

    const size_t nindex = i*csp.getSize()+j;
    queue.insert(nindex, csp.getCalculus().getWeight(csp.getConstraint(i, j)));
}

template<class R, class N>
inline
std::vector<Tuple> WeightedTripleIterator<R,N>::pc1(N& csp) {
    const size_t& size = csp.getSize();

    while (!queue.empty()) {
        const size_t index = queue.peekMin().first;
        queue.popMin();

        const size_t i = index / size;
        const size_t j = index % size;

        assert(i < j);

        for (size_t k = 0; k < size; ++k) {
            if (revise(csp, i, j, k)) {
                if (csp.getConstraint(i, k).none()) {
                    return describeTriple(i,j,k);
                }
                add_to_queue(i, k, csp);
            }

            if (revise(csp, k, i, j)) {
                if (csp.getConstraint(k, j).none()) {
                    return describeTriple(i,j,k);
                }
                add_to_queue(k, j, csp);
            }
        }
    }

    return std::vector<Tuple>();
}

template<class R, class N>
inline
std::vector<Tuple> WeightedTripleIterator<R,N>::enforce(N& csp) {
    queue.clear();

    const size_t& size = csp.getSize();

    if (size < 2) return std::vector<Tuple>();

    if (csp.getConstraint(0, 1).none())
        return describeTriple(0,1,2);

    // At least one edge is non-zero, hence revision will fail at some point even if empty relation is present in the input CSP

    PriorityQueue queue;
    for (size_t i = 0; i < size; i++) {
        for (size_t j = i+1; j < size; j++) {
            if (csp.getCalculus().baseRelationsAreSerial() && csp.getConstraint(i, j) == csp.getCalculus().getUniversalRelation())
                continue;
            add_to_queue(i, j, csp);
        }
    }

    return pc1(csp);
}

template<class R, class N>
inline
std::vector<Tuple> WeightedTripleIterator<R,N>::enforce(N& csp, const size_t i, const size_t j) {
    queue.clear();

    const size_t& size = csp.getSize();

    if (size > 0 && csp.getConstraint(i, j).none())
        return describeTriple(0, i, j);

    // At least one edge is non-zero, hence revision will fail at some point even if empty relation is present in the input CSP

    add_to_queue(i, j, csp);

    return pc1(csp);
}

}
