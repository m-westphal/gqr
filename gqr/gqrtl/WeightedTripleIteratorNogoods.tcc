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
bool WeightedTripleIteratorNogoods<R,N>::revise(N& csp, const size_t i, const size_t j, const size_t k) {
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
std::vector<Tuple> WeightedTripleIteratorNogoods<R,N>::pc1(N& csp) {
    const size_t& size = csp.getSize();

    while (!this->queue.empty()) {
        const size_t index = this->queue.peekMin().first;
        this->queue.popMin();

        const Tuple t(index / size, index % size);

        for (size_t k = 0; k < size; ++k) {
            if (revise(csp, t.x, t.y, k)) {
                if (csp.getConstraint(t.x, k).none()) {
                    return this->describeTriple(t.x, t.y, k);
                }
                this->add_to_queue(t.x, k, csp);
            }

            if (revise(csp, k, t.x, t.y)) {
                if (csp.getConstraint(k, t.y).none()) {
                    return this->describeTriple(t.x, t.y, k);
                }
                this->add_to_queue(k, t.y, csp);
            }
        }

	// Nogood propagator

	{
	for (size_t b = 0; b < csp.getCalculus().getNumberOfBaseRelations(); b++)
		if (!csp.getValue(t)[b] && nogoodDB.check(t, b)) {
			std::vector<Tuple> f = nogoodDB.checkNogoods(t, b, csp, this->queue);
			if(!f.empty()) return f;
		}
	}
    }

    return std::vector<Tuple>();
}

template<class R, class N>
inline
std::vector<Tuple> WeightedTripleIteratorNogoods<R,N>::enforce(N& csp) {
    const size_t& size = csp.getSize();

    if (size < 2) return std::vector<Tuple>();

    if (csp.getConstraint(0, 1).none())
        return this->describeTriple(0,1,2);

    // At least one edge is non-zero, hence revision will fail at some point even if empty relation is present in the input CSP

    this->queue.clear();
    for (size_t i = 0; i < size; i++) {
        for (size_t j = i+1; j < size; j++) {
            if (csp.getCalculus().baseRelationsAreSerial() && csp.getConstraint(i, j) == csp.getCalculus().getUniversalRelation())
                continue;
            this->add_to_queue(i, j, csp);
        }
    }

    nogoodDB.startPropagation();
    return pc1(csp);
}

template<class R, class N>
inline
std::vector<Tuple> WeightedTripleIteratorNogoods<R,N>::enforce(N& csp, const size_t i, const size_t j) {
    const size_t& size = csp.getSize();

    if (size > 0 && csp.getConstraint(i, j).none())
        return this->describeTriple(0, i, j);

    // At least one edge is non-zero, hence revision will fail at some point even if empty relation is present in the input CSP

    this->queue.clear();

    this->add_to_queue(i, j, csp);

    nogoodDB.startPropagation();
    return pc1(csp);
}

}
