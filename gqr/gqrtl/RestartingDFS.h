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

#ifndef RESTARTING_DFS_H
#define RESTARTING_DFS_H

#include <cassert>
#include <vector>

#include "Tuple.h"

#include "gqrtl/CalculusOperations.h"
#include "gqrtl/CSPStack.h"
#include "gqrtl/VariableHeuristic.h"
#include "gqrtl/DFSReport.h"

#include "gqrtl/NogoodDB.h"

#include "gqrtl/WeightedTripleIteratorNogoods.h"

class Logger;

class RestartsFramework;

/**
 * Implements a 2-way dfs
 */

namespace gqrtl {

template<class R>
class RestartingDFS : public DFSReport {
	private:
		typedef CSPStack<R, CalculusOperations<R> > CoreCSPStack; // TODO: Move somewhere else
		typedef CSP<R, CalculusOperations<R> > CoreCSP; // TODO: Move somewhere else

		CoreCSPStack current_state;

		class Decision {
			public:
				Tuple variable;
				R value;
				enum Type { positive, negative, negative_direct, implied } type;
				Decision(const Tuple t, const R v, const Type ty) : variable(t), value(v), type(ty) {};
		};

		std::vector<Decision> decisions;

		// Last Conflict Based Reasoning
		Tuple lastConflict;

		const Tuple decideVariable();
		const R decideValue(const R& values);
		bool decide();
		bool enforceConsistency();

		enum Status { satisfiable, unsatisfiable, indeterminate };
		Status dfs();

		std::vector<Tuple> variables;

		/** print statistics if log interval exceeded */
		gqrtl::WeightWDeg<R> variableHeuristic;

		::Logger* log;

		void generateLogReport();

		RestartsFramework& restartsFramework;

		size_t nextRestart;

		bool extractNogoods();
		bool minimizeNogood(typename NogoodDB<R>::nogood&);

		NogoodDB<R> nogoodDB;

		WeightedTripleIteratorNogoods<R, CoreCSPStack> propagate;

	public:
		RestartingDFS(const CoreCSP& csp, RestartsFramework& r, Logger* log);
		~RestartingDFS();

		// Run search
		CoreCSP* run();
};

}

#include "gqrtl/RestartingDFS.tcc"

#endif
