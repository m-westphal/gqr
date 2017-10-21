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

#ifndef DFS_H
#define DFS_H

#include <cassert>
#include <vector>

#include "Tuple.h"

#include "gqrtl/CalculusOperations.h"
#include "gqrtl/CSPStack.h"
#include "gqrtl/VariableHeuristic.h"
#include "gqrtl/DFSReport.h"

#include "gqrtl/WeightedTripleIterator.h"

class Logger;

/**
 * Implements a 2-way dfs
 */

namespace gqrtl {

template<class R>
class DFS : public DFSReport {
	private:
		typedef CSPStack<R, CalculusOperations<R> > CoreCSPStack; // TODO: Move somewhere else
		typedef CSP<R, CalculusOperations<R> > CoreCSP; // TODO: Move somewhere else

		WeightedTripleIterator<R, CoreCSPStack> propagate;

		CoreCSPStack current_state;

		class Decision {
			public:
				Tuple variable;
				R value;
				enum Type { positive, negative, implied } type;
				Decision(const Tuple t, const R v, const Type ty) : variable(t), value(v), type(ty) {};
		};

		std::vector<Decision> decisions;

		// Last Conflict Based Reasoning
		Tuple lastConflict;

		const Tuple decideVariable();
		const R decideValue(const R& values);
		bool decide();
		bool enforceConsistency();
		bool dfs();

		std::vector<Tuple> variables;

		/** print statistics if log interval exceeded */
		gqrtl::WeightWDeg<R> variableHeuristic;

		::Logger* log;

		void generateLogReport();

	public:
		DFS(const CoreCSP& csp, Logger* log);
		~DFS();

		// Run search
		CoreCSP* run();
};

}

#include "gqrtl/DFS.tcc"

#endif
