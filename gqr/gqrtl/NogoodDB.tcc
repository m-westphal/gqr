// -*- C++ -*-

// Copyright (C) 2009-2012 Matthias Westphal
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

#include <algorithm>

#include <iostream>

namespace gqrtl {

template<class R> void NogoodDB<R>::addNogood(const typename NogoodDB<R>::nogood& ng, const CSPStack<R, CalculusOperations<R> >& csp) {
	assert(ng.size() > 1);

	#ifndef NDEBUG
	std::cout << "addNogood():\n";
	for (size_t j = 0; j < ng.size(); j++)
		std::cout << "\t\t" << ng[j].first.x << ", " << ng[j].first.y << ": " << ng[j].second << "\n";

	// test useless nogoods
	for (size_t i = 0; i < ng.size(); i++) {
		const Tuple& var = ng[i].first;
		const R& decision = ng[i].second;
		if ( ((decision & csp.getValue(var)).none())
			|| csp.getValue(var).isSubsetOf(decision) ) {

			std::cerr << "Useless nogood!\n";
			std::cerr << var << "<-" << decision << " decision is bad, but we know ";
			std::cerr << csp.getValue(var) << " already.\n";
			assert(false);
		}
	}

	// test malformed nogoods
	for (size_t i = 0; i < ng.size(); i++)
		for (size_t j = i+1; j < ng.size(); j++)
			if (ng[i].first == ng[j].first) {
				std::cerr << "Decision repeated in nogood\n";
			}
	#endif

	const Tuple& v = ng[0].first;
	const R& dom_v = csp.getValue(v);
	const R& D = ng[0].second;

	const Tuple& vp = ng[1].first;
	const R& dom_vp = csp.getValue(vp);
	const R& Dp = ng[1].second;


	size_t value1 = 0;
	for (typename R::const_iterator it = dom_v.begin(); it != dom_v.end(); ++it)
		if (!D[*it]) {
			value1 = *it;
			break;
		}
	assert(dom_v[value1] && !D[value1]);

	size_t value2 = 0;
	for (typename R::const_iterator it = dom_vp.begin(); it != dom_vp.end(); ++it)
		if (!Dp[*it]) {
			value2 = *it;
			break;
		}
	assert(dom_vp[value2] && !Dp[value2]);

	const watchedAtom a(v, value1);
	const watchedAtom b(vp, value2);

	nogoods.push_back(nogoodNode(ng, std::make_pair(a, b), 0, 1));
	nogoods.back().used = 1.0;

	watched_atoms.insert(getPos(v , value1)).first->push_back(&(nogoods.back()));
	watched_atoms.insert(getPos(vp, value2)).first->push_back(&(nogoods.back()));
}


template<class R> void NogoodDB<R>::manageNogoods() {
	const size_t maxNogoods = 4096; // TODO make configurable

	while(nogoods.size() > maxNogoods) {
		double min = std::numeric_limits<double>::max();
		typename std::list<nogoodNode>::iterator c = nogoods.end();
		size_t i = 0;
		for (typename std::list<nogoodNode>::iterator it = nogoods.begin(); it != nogoods.end(); ++it, ++i) {
			if (i == maxNogoods) break;
			if (it->used < min) {
				c = it;
				min = it->used;
			}
		}
		assert(c != nogoods.end());
		watched_atoms[getPos(c->wa.first.tuple, c->wa.first.bit)].remove(&(*c));
		watched_atoms[getPos(c->wa.second.tuple, c->wa.second.bit)].remove(&(*c));
		nogoods.erase(c);
	}
}

template<class R>
std::vector<Tuple> NogoodDB<R>::checkNogoods(const Tuple v, const size_t d, CoreCSPStack& csp, PriorityQueue& queue) {
	assert(!csp.getValue(v)[d]);	// d was removed from dom(v)

	assert(processed_labels[getPos(v,d)] == false);

	*(processed_labels.insert(getPos(v,d)).first) = true;
	undo_processed.push_back(getPos(v,d));

	std::list<nogoodNode*>& list = watched_atoms[getPos(v, d)];
	for (typename std::list<nogoodNode*>::iterator it = list.begin(); it != list.end();) {
		typename std::list<nogoodNode*>::iterator current = it;
		it++;

		// identify watched atom and "the other" watched atom
		watchedAtom* wa = &((*current)->wa.first);
		watchedAtom* o_wa = &((*current)->wa.second);
		size_t* index = &((*current)->idx_1);
		size_t* o_index = &((*current)->idx_2);

		if ((*current)->wa.second.tuple == v) {
			assert((*current)->wa.second.bit == d);

			std::swap(wa, o_wa);
			std::swap(index, o_index);
		}
		else {
			assert((*current)->wa.first.tuple == v && (*current)->wa.first.bit == d);
		}

		const nogood& ng = (*current)->ng;

		// easy access
		const Tuple& vp = o_wa->tuple;
		const R& Dp = ng[*o_index].second;

		const R& dom_vp = csp.getValue(vp);

		if ( (dom_vp & Dp).none() ) // CSP already avoids the Dp decision
			continue;

		// try updating on v before iterating rest of nogood
		{
			const R& dom_v = csp.getValue(v);
			const R& D = ng[*index].second;
			if (!dom_v.isSubsetOf(D)) {
				for (typename R::const_iterator it = dom_v.begin(); it != dom_v.end(); ++it)
					if (!D[*it]) {
						wa->bit = *it;

						watched_atoms.insert(getPos(v, *it)).first->push_back(*current);
						list.erase(current);
						break;
					}
				continue;
			}
		}

		bool updated = false;
		for (size_t l = std::max(*index, *o_index)+1; l < ng.size(); l++) {
			const Tuple& vpp = ng[l].first;
			const R& Dpp = ng[l].second;

			assert(vpp != vp);

			const R& dom_vpp = csp.getValue(vpp);
			if (!dom_vpp.isSubsetOf(Dpp)) {
				for (typename R::const_iterator it = dom_vpp.begin(); it != dom_vpp.end(); ++it)
					if (!Dpp[*it]) {
						wa->tuple = vpp;
						wa->bit = *it;
						*index = l;

						watched_atoms.insert(getPos(vpp, *it)).first->push_back(*current);
						list.erase(current);
						break;
					}

				updated = true;
				break;
			}
		}

		if (updated) {
			// the nogood is (currently) not applicable for GAC

			assert( !(dom_vp & Dp).none() );
			assert( !csp.getValue(wa->tuple).isSubsetOf(ng[*index].second) );
		}
		else {
#ifndef NDEBUG
			size_t sat = 0;
			for (size_t i = 0; i < ng.size(); ++i)
				if (csp.getValue(ng[i].first).isSubsetOf(ng[i].second))
					sat++;

			const size_t unsat = ng.size() - sat;
			if (unsat > 1) {
				std::cerr << "Wrong nogood application\n";
				std::cerr << unsat << " unsat elements\n";
				std::cerr << *index << ", " << *o_index << std::endl;
				for (size_t i = 0; i < ng.size(); ++i) {
					std::cerr << ng[i].first << "<-" << ng[i].second;
					std::cerr << " is currently " << csp.getValue(ng[i].first);
					std::cerr << std::endl;
				}
			}
			assert(unsat <= 1);
#endif
			nr_reductions++;
			(*current)->used += 1.0;

			const R Dp_excluded = csp.getCalculus().getNegation(Dp);
			csp.setValue(vp, csp.getValue(vp) & Dp_excluded);
			queue.insert(vp.x*csp.getSize()+vp.y, csp.getCalculus().getWeight(csp.getValue(vp)));
			if (csp.getValue(vp).none()) {
				std::vector<Tuple> failed_constraint;
				for (size_t l = 0; l < ng.size(); l++)
					failed_constraint.push_back(ng[l].first);
				return failed_constraint;
			}
		}
	}

	return std::vector<Tuple>();
}

template<class R>
void NogoodDB<R>::startPropagation() {
	for (size_t i = 0; i < undo_processed.size(); ++i) {
		processed_labels[undo_processed[i]] = false;
	}
	undo_processed.clear();

	// TODO: think about optimizing
	for (typename std::list<nogoodNode>::iterator it = nogoods.begin(); it != nogoods.end(); ++it) {
		if (it->idx_1 < 2 && it->idx_2 < 2)
			continue;

		std::swap(it->ng[0], it->ng[it->idx_1]);
		it->idx_1 = 0;
		std::swap(it->ng[1], it->ng[it->idx_2]);
		it->idx_2 = 1;
	}
}

template<class R>
void NogoodDB<R>::printStatistics() const {
	std::cout << "\tNumber of nogoods=" << nogoods.size();
	std::cout << ", " << nr_nogoods_minimized << "/" << nr_nogoods_notminimized;
	std::cout << " (minimized/not minimized)";
	if (nr_nogoods_notminimized > 0)
		std::cout << " " << (double) nr_nogoods_minimized / (double) nr_nogoods_notminimized << "\n";
	else
		std::cout << " no ratio\n";
	std::cout << "\tNogoods propagation; ";
	std::cout << "domain reductions=" << nr_reductions;
	std::cout << ", singleton nogoods=" << nr_singleton_ng;
	std::cout << std::endl << std::flush;
}

#ifndef NDEBUG
template<class R>
bool NogoodDB<R>::allNogoodsApplied(const CoreCSPStack& csp) const {
	for (typename std::list<nogoodNode>::const_iterator it = nogoods.begin(); it != nogoods.end(); it++) {
		const nogood& ng = it->ng;

		assert(ng[it->idx_1].first == it->wa.first.tuple);
		assert(ng[it->idx_2].first == it->wa.second.tuple);

		assert(ng.size() > 1);
		size_t sat = 0;
		for (size_t i = 0; i < ng.size(); i++) {
			const Tuple& var = ng[i].first;
			const R& value = ng[i].second;
			const R& current = csp.getValue(var);

			if ( current.isSubsetOf(value) ) {
				sat++;
			}
		}

		if (sat == ng.size()) {
			std::cerr << "Nogood not applied!\n";
			for (size_t i = 0; i < ng.size(); i++)
				std::cerr << ng[i].first.x << ", " << ng[i].first.y << "<-" << ng[i].second << " is currently " << csp.getValue(ng[i].first) << "\n";
			return false;
		}
		if (sat == ng.size()-1) { // maybe some inference is missing
			for (size_t i = 0; i < ng.size(); i++) {
				const Tuple& var = ng[i].first;
				const R& value = ng[i].second;
				const R& current = csp.getValue(var);

				if ( current.isSubsetOf(value) )
					continue;
				if ( (current & value).none() ) // avoid decision
					break;	// only one unsat relation
				std::cerr << "inference not done!\n";
				std::cerr << var << "<-" << value << " is currently " << current << "\n";

				if (csp.getValue(it->wa.first.tuple)[it->wa.first.bit])
					std::cerr << "\twatched atom 1 is still valid\n";
				else
					std::cerr << "\twatched atom 1 is not valid anymore\n";

				if (csp.getValue(it->wa.second.tuple)[it->wa.second.bit])
					std::cerr << "\twatched atom 2 is still valid\n";
				else
					std::cerr << "\twatched atom 2 is not valid anymore\n";

				std::cerr << *it << "\n";
				return false;
			}
		}
	}

	return true;
}
#endif

}
