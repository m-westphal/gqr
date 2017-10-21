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

#include "RestartsFramework.h"

namespace gqrtl {

template<class R>
RestartingDFS<R>::RestartingDFS(const RestartingDFS::CoreCSP& csp, RestartsFramework& r, ::Logger* l)
	: DFSReport(), current_state(csp),
	lastConflict(InvalidTuple),
	variableHeuristic(csp.getSize()), log(l),
	restartsFramework(r),
	nogoodDB(csp.getSize(), csp.getCalculus().getNumberOfBaseRelations()),
	propagate(nogoodDB) { }

template<class R>
RestartingDFS<R>::~RestartingDFS() { }

template<class R>
const Tuple RestartingDFS<R>::decideVariable() {
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
const R RestartingDFS<R>::decideValue(const R& values) {
	assert(!values.none());

	const R value = current_state.getCalculus().getFirstSplit(values);
	assert(!value.none());

	return value;
}

template<class R>
bool RestartingDFS<R>::decide() {
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
bool RestartingDFS<R>::enforceConsistency() {
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

	#ifndef NDEBUG
	assert(nogoodDB.allNogoodsApplied(current_state));
	#endif

	return true;
}

template<class R>
typename RestartingDFS<R>::Status RestartingDFS<R>::dfs() {
	if (number_negative_decisions == nextRestart)
		return RestartingDFS<R>::indeterminate;

	if (log) {
		visited_depth_min = std::min<size_t>(visited_depth_min, decisions.size());
		visited_depth_max = std::max<size_t>(visited_depth_max, decisions.size());
		if (log->logDue())
			generateLogReport();
	}

	if (!enforceConsistency())	// inconsistent
		return RestartingDFS<R>::unsatisfiable;

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
		return RestartingDFS<R>::satisfiable;

	if (!decide())	// implied decisions
		return dfs();

	// Made a positive decision:
	// test the outcome
	const Status status = dfs();
	if (status == RestartingDFS<R>::satisfiable || status == RestartingDFS<R>::indeterminate) {
		return status;
	}
	else {
		assert(status == unsatisfiable);

		assert(number_negative_decisions < nextRestart);

		number_negative_decisions++; // cutoff might be reached, but we put negative decision on stack anyway

		const bool direct_deadend = (decisions.back().type == Decision::positive);

		while(decisions.back().type != Decision::positive) {
			if (decisions.back().type == Decision::implied)
				variables.push_back(decisions.back().variable);
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
//		std::cout << "Negative decision: " << var.x << ", " << var.y << " set to " << nvalue << "\n";;
		assert(!nvalue.none());

		// replace positive decision with negative one
		decisions.pop_back();
		if (direct_deadend)
			decisions.push_back(Decision(var, nvalue, Decision::negative_direct));
		else
			decisions.push_back(Decision(var, nvalue, Decision::negative));

		return dfs();
	}
}

template<class R>
bool RestartingDFS<R>::extractNogoods() {
	typename NogoodDB<R>::nogood next_ng;
	for (size_t i = 0; i < decisions.size(); i++) {
		if (decisions[i].type == Decision::positive) {
			next_ng.push_back(std::make_pair(decisions[i].variable, decisions[i].value));
		}
		else if (decisions[i].type == Decision::negative || decisions[i].type == Decision::negative_direct) {
			typename NogoodDB<R>::nogood im_ng = next_ng;
			const R negation = current_state.getCalculus().getNegation(decisions[i].value);
			im_ng.push_back(std::make_pair(decisions[i].variable, negation));

			const size_t a = im_ng.size();
			if (restartsFramework.minimizeNogoods &&
				decisions[i].type == Decision::negative_direct) {

				if (a > 1 && !minimizeNogood(im_ng))
					return false;
			}
			const size_t b = im_ng.size();

			if (a > b)
				nogoodDB.nr_nogoods_minimized++;
			else
				nogoodDB.nr_nogoods_notminimized++;

			if (b == 1) {
				nogoodDB.nr_singleton_ng++;
				const Tuple& var = im_ng[0].first;
				const R negation = current_state.getCalculus().getNegation(im_ng[0].second);
				current_state.setValue(var, negation & current_state.getValue(var));
			}
			else {
				nogoodDB.addNogood(im_ng, current_state);
			}
		}
	}

	nogoodDB.manageNogoods();
	return true; // "return false" implies unsat CSP
}

template<class R>
bool RestartingDFS<R>::minimizeNogood(typename NogoodDB<R>::nogood& ng) {
	assert(ng.size() > 1);

#ifndef NDEBUG
std::cout << "Minimize a nogood of size " << ng.size() << ": ";
#endif

	typename NogoodDB<R>::nogood result;
	result.reserve(ng.size());

	// TODO: optimize; put somewhere higher up
	WeightedTripleIteratorNogoods<R, CoreCSPStack> propagate(nogoodDB);
	propagation_calls++;
	if (!propagate.enforce(current_state).empty())
		return false;	// probably never happens

	current_state.backupState();
	// find transition constraint (note: must exist, since we only run on direct dead ends)
	bool done = false;
	while (!done) {
		current_state.backupState();
		#ifndef NDEBUG
		bool loop_finished = true;
		#endif
		for (size_t i = 0; i < ng.size(); ++i) {
			const Tuple& var = ng[i].first;
			const R refinement = ng[i].second & current_state.getValue(var);
			current_state.setValue(var, refinement);
			propagation_calls++;
			if (!propagate.enforce(current_state, var.x, var.y).empty()) {
				#ifndef NDEBUG
				loop_finished = false;
				#endif
				result.push_back(ng[i]);
				current_state.resetToLastState();

				const R new_refinement = ng[i].second & current_state.getValue(var);
				current_state.setValue(var, new_refinement);

				while (ng.size() - i > 0) ng.pop_back();

				propagation_calls++;
				if (!propagate.enforce(current_state, var.x, var.y).empty())
					done = true;
				break;
			}
		}
		#ifndef NDEBUG
		assert(!loop_finished);
		#endif
	}
	current_state.resetToLastState();

	ng = result;
#ifndef NDEBUG
	assert(!ng.empty());
std::cout << "result of size " << ng.size() << "\n";
#endif
	return true;
}

template<class R>
typename RestartingDFS<R>::CoreCSP* RestartingDFS<R>::run() {
	if (log)
		log->start();

	const size_t& size = current_state.getSize();

	variables.clear();
	variables.reserve((size*size)/2);
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

	Status consistency = RestartingDFS<R>::indeterminate;
	while(consistency == RestartingDFS<R>::indeterminate) {
		current_state.resetToInitialState();
		nextRestart = restartsFramework.getNextCutoff();

		if (!decisions.empty()) {
			assert(decisions.back().type == Decision::negative || decisions.back().type == Decision::negative_direct);

			if (!extractNogoods()) {
				consistency = unsatisfiable;
				break;
			}

			// reset variables, clear decisions
			for (size_t i = 0; i < decisions.size(); i++)
				if (decisions[i].type == Decision::positive || decisions[i].type == Decision::implied) {
					const R& value = current_state.getValue(decisions[i].variable);
					if (current_state.getCalculus().isSplit(value))
						number_implied_decisions++;
					else
						variables.push_back(decisions[i].variable);
				}
			decisions.clear();
		}

		lastConflict = InvalidTuple;
		consistency = dfs();
	}

	if (log) {
		log->end();
		generateLogReport();
	}

	if (consistency == satisfiable) {
		return new CoreCSP(current_state.getCSP());
	}

	assert(consistency == unsatisfiable);
	return NULL;
}

template<class R>
void RestartingDFS<R>::generateLogReport(void) {
	assert(log);

	depth = decisions.size();
	unassigned_variables = variables.size();

	branch_positive_decisions = 0;
	branch_negative_decisions = 0;
	branch_implied_decisions = 0;
	for (size_t i = 0; i < decisions.size(); i++)
		switch(decisions[i].type) {
			default:
				assert(false);
			case Decision::positive:
				branch_positive_decisions++; break;
			case Decision::negative:
			case Decision::negative_direct:
				branch_negative_decisions++; break;
			case Decision::implied:
				branch_implied_decisions++; break;
		}


	log->postDFSLog(*this);
	nogoodDB.printStatistics();

//	current_state.getCalculus().printStatistics();
//	std::cout << "/* Collected statistics:";
//	this->printStatistics();
//	std::cout << "*/\n";

	// min-max is relative to the log-interval
	visited_depth_min = std::numeric_limits<size_t>::max();
	visited_depth_max = 0;
}

}
