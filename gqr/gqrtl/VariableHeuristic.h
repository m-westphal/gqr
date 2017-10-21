// -*- C++ -*-

// Copyright (C) 2008 Zeno Gantner
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

#ifndef WEIGHTWDEG_H
#define WEIGHTWDEG_H

#include <cassert>
#include <vector>
#include <limits>
#include <utility>

#include "Tuple.h"

#include "gqrtl/CSP.h"
#include "gqrtl/CalculusOperations.h"
#include "gqrtl/HashMap.h"

namespace gqrtl {

/**
 * variable ordering according to the weight heuristic taking into account learned weights.
 * It is essentially domwdeg with weights instead of domain size.
 */
template<class R>
class WeightWDeg {
	private:
		HashMap<size_t> learnedWeights;
		/** maximal encountered depth */
		size_t maxDepth;

		size_t size;

		inline size_t getPos(const size_t i, const size_t j) const {
			assert(i <= j);
			return j+i*size - (i*(i+1))/2;
		}

		void updateWeight(size_t i, size_t j, const size_t importance) {
			if (i > j)
				std::swap(i,j);

			std::pair<size_t*, bool> value = learnedWeights.insert(getPos(i,j));
			if (value.second)
				*(value.first) = 1;
			const size_t val = *(value.first) + maxDepth - importance + 1;

			if (val < (*value.first)) {	// overflow
				*(value.first) = std::numeric_limits<size_t>::max();
			}
			else {
				*(value.first) = val;
			}
		}

	public:
		WeightWDeg(const size_t nodes) : maxDepth(1), size(nodes) {}

		size_t decide(const std::vector<Tuple>& variables, const CSP<R, gqrtl::CalculusOperations<R> >& csp) const {
			assert(!variables.empty());

			size_t pos = 0;
			double min = std::numeric_limits<double>::max();

			for (size_t i = 0; i < variables.size(); i++) {
				const Tuple& current = variables[i];
				const R& value = csp.getConstraint(current.x, current.y);
				if (csp.getCalculus().isSplit(value)) {
					return i;
				}

				size_t learned_value = 1;
				if (learnedWeights.exists(getPos(current.x,current.y)))
					learned_value = learnedWeights[getPos(current.x,current.y)];

				const double weight = ((double) csp.getCalculus().getWeight(value)) /
				                                        (double) learned_value;
				if (weight < min) {
					min = weight;
					pos = i;
				}
			}
			return pos;
		}

		void reportFailure(const std::vector<Tuple>& violated_constraint, const size_t importance) {
			if (importance > maxDepth) {
				maxDepth = importance;
			}

			assert(maxDepth >= importance);

			for (size_t i = 0; i < violated_constraint.size(); ++i)
				updateWeight(violated_constraint[i].x, violated_constraint[i].y, importance);
		}
};

}

#endif // WEIGHTWDEG_H
