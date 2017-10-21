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

#include <limits>
#include <algorithm>

#include "utils/Logger.h"


namespace gqrtl {

template<class R>
DFS<R>::DFS(const DFS::CoreCSP& csp, ::Logger* l) : DFSReport(),
	current_state(csp), lastConflict(InvalidTuple), variableHeuristic(csp.getSize()), log(l) {}

template<class R>
DFS<R>::~DFS() {}

template<class R>
const Tuple DFS<R>::decideVariable() {
	assert(!variables.empty());

	size_t pos;
	if (lastConflict != InvalidTuple) {
		assert(std::find(variables.begin(), variables.end(), lastConflict) != variables.end());

		for (pos = variables.size()-1; variables[pos] != lastConflict; --pos);
		assert(variables[pos] == lastConflict);
	}
	else {
		pos = variableHeuristic.decide(variables, current_state.getCSP());
	}

	std::swap(variables[pos], variables.back());
	Tuple res = variables.back();
	variables.pop_back();

	return res;
}

template<class R>
const R DFS<R>::decideValue(const R& values) {
	assert(!values.none());

	const R value = current_state.getCalculus().getFirstSplit(values);
	assert(!value.none());

	return value;
}

template<class R>
bool DFS<R>::decide() {
	const Tuple var = decideVariable();

	const R& values = current_state.getValue(var);
	assert(!values.none());
	if (current_state.getCalculus().isSplit(values)) {
		decisions.push_back(Decision(var, values, Decision::implied));
		number_implied_decisions++;
		return false;	// no need to assign a new value
	}

	const R value = current_state.getCalculus().getFirstSplit(values);
	assert(!value.none());
	assert(value != current_state.getValue(var));

	decisions.push_back(Decision(var, value, Decision::positive));
	number_positive_decisions++;

	current_state.backupState();			// new depth
	current_state.setValue(var, value);		// enforce decision

	return true;
}

template<class R>
bool DFS<R>::enforceConsistency() {
	if (decisions.empty()) {
		propagation_calls++;

		std::vector<Tuple> t = propagate.enforce(current_state);
		if (!t.empty())
			return false;
	}
	else if (decisions.back().type != Decision::implied) {
		const Tuple& var = decisions.back().variable;

		propagation_calls++;
		std::vector<Tuple> t = propagate.enforce(current_state, var.x, var.y);
		if (!t.empty()) {
			variableHeuristic.reportFailure(t, decisions.size());
			return false;
		}
	}

	return true;
}

template<class R>
bool DFS<R>::dfs() {
	if (log) {
		visited_depth_min = std::min<size_t>(visited_depth_min, decisions.size());
		visited_depth_max = std::max<size_t>(visited_depth_max, decisions.size());
		if (log->logDue())
			generateLogReport();
	}

	if (!enforceConsistency())	// inconsistent
		return false;

	#ifndef NDEBUG
	const CoreCSP DEBUG = current_state.getCSP();
	#endif

	// make new decision
	if (!decisions.empty() && lastConflict != InvalidTuple
		&& (decisions.back().type == Decision::positive || decisions.back().type == Decision::implied)) {
		assert(decisions.back().variable == lastConflict);	// positve/implied decision may only happen on lastConflict
		lastConflict = InvalidTuple;
	}

	if (variables.empty())
		return true;

	if (!decide())	// implied decisions
		return dfs();

	// positive decision
	if (dfs()) {
		return true;
	}
	else {
		number_negative_decisions++;
		while(decisions.back().type != Decision::positive) {
			if (decisions.back().type == Decision::implied) {
				variables.push_back(decisions.back().variable);
			}
			decisions.pop_back();
		}

		const Tuple var = decisions.back().variable;
		if (lastConflict == InvalidTuple)
			lastConflict = var;

		variables.push_back(var);

		current_state.resetToLastState();

		#ifndef NDEBUG
		assert(current_state == DEBUG);
		#endif

		const R nvalue = current_state.getCalculus().getNegation(decisions.back().value);

		current_state.setValue(var, nvalue & current_state.getValue(var));
//		std::cout << "Negative decision: " << var << " set to " << nvalue << "\n";;
		assert(!nvalue.none());

		// replace positive decision with negative one
		decisions.pop_back();
		decisions.push_back(Decision(var, nvalue, Decision::negative));

		return dfs();
	}
}


template<class R>
typename DFS<R>::CoreCSP* DFS<R>::run() {
	if (log)
		log->start();

	const size_t& size = current_state.getSize();

	variables.clear();
	for (size_t i = 0; i < size; i++)
		for (size_t j = i+1; j < size; j++) {
			const Tuple current = Tuple(i,j);
			const R& value = current_state.getValue(current);
			if (!current_state.getCalculus().isSplit(value))
				variables.push_back(current);
			else
				number_implied_decisions++;
		}
#ifndef NDEBUG
std::cout << "Ignoring " << number_implied_decisions << " variables which are already tractable\n";
#endif

	const bool consistent = dfs();
	if (log) {
		log->end();
		generateLogReport();
	}

	if (consistent) {
		return new CoreCSP(current_state.getCSP());
	}

	return NULL;
}

template<class R>
void DFS<R>::generateLogReport(void) {
	assert(log);

	branch_positive_decisions = 0;
	branch_negative_decisions = 0;
	branch_implied_decisions = 0;

	depth = decisions.size();
	unassigned_variables = variables.size();

	for (size_t i = 0; i < decisions.size(); i++)
		switch(decisions[i].type) {
			default:
				assert(false);
			case Decision::positive:
				branch_positive_decisions++; break;
			case Decision::negative:
				branch_negative_decisions++; break;
			case Decision::implied:
				branch_implied_decisions++; break;
		}

	log->postDFSLog(*this);

	// min-max is relative to the log-interval
	visited_depth_min = std::numeric_limits<size_t>::max();
	visited_depth_max = 0;
}

}
