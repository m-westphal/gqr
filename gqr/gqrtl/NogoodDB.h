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

#ifndef NOGOOD_DB
#define NOGOOD_DB

#include <cassert>
#include <string>
#include <list>
#include <ostream>
#include <utility>

#include "Tuple.h"

#include "gqrtl/HashMap.h"

namespace gqrtl {

template<class R>
class NogoodDB {
	public:
		typedef std::vector<std::pair<Tuple, R> > nogood;
	protected:
		inline size_t getPos(const Tuple& t, const size_t b) const {
			return (t.y+t.x*network_size - (t.x*(t.x+1))/2)*calculus_size+b;
		}

		typedef CSPStack<R, CalculusOperations<R> > CoreCSPStack;
		const size_t network_size;
		const size_t calculus_size;


		class watchedAtom {
			public:
			Tuple tuple;
			size_t bit;
			watchedAtom(const Tuple& t, const size_t b) : tuple(t), bit(b) {};
			friend std::ostream& operator<<(std::ostream& os, const watchedAtom& w) {
				os << "(" << w.tuple << "," << w.bit << ")";
				return os;
			}
		};
		typedef std::pair<watchedAtom, watchedAtom> watchedAtoms;

		class nogoodNode {
			public:
				nogood ng;
				watchedAtoms wa;
				size_t idx_1; // first wa index (in ng)
				size_t idx_2; // second wa index (in ng)
				double used;
				nogoodNode(const nogood& ng, const watchedAtoms at, const size_t i1, const size_t i2)
					: ng(ng), wa(at), idx_1(i1), idx_2(i2), used(0) {};

				friend std::ostream& operator<<(std::ostream& os, const nogoodNode& n) {
					os << "WAtoms: " << n.wa.first << "\t" << n.wa.second << "\n";
					for (size_t i = 0; i < n.ng.size(); i++) {
						const R& r = n.ng[i].second;
						os << n.ng[i].first << "<-" << r << " and ";
					}
					os << "T -> UNSAT\n";
					return os;
				}
		};


		std::list<nogoodNode> nogoods;
		HashMap<std::list<nogoodNode*> > watched_atoms;

		// avoid checking removed values twice
		HashMap<bool> processed_labels; // TODO: could be a hash set
		std::vector<size_t> undo_processed;

	public:
		void startPropagation();

		virtual void addNogood(const typename NogoodDB<R>::nogood& ng, const CoreCSPStack& csp);

		virtual void manageNogoods();

		inline bool check(const Tuple& t, const size_t b) const {
			assert(b < calculus_size);

			const size_t mempos = getPos(t, b);
			return !(watched_atoms[mempos].empty() || processed_labels[mempos]);
		}

		inline std::vector<Tuple> checkNogoods(const Tuple t, const size_t b, CoreCSPStack& csp, PriorityQueue& queue);

        // statistics on usefulness of nogoods
        size_t nr_singleton_ng;
        size_t nr_nogoods_minimized;
        size_t nr_nogoods_notminimized;
    protected:
        size_t nr_reductions; // how often nogoods reduced the network

    public:
        NogoodDB(const size_t nodes, const size_t base_relations) : network_size(nodes), calculus_size(base_relations) {
            nr_reductions = 0;
            nr_singleton_ng = 0;
            nr_nogoods_minimized = 0;
            nr_nogoods_notminimized = 0;
        }

        void printStatistics() const;
        /** update internal statistics (should be called whenever propagation is invoked) */
        void visit_node();

        #ifndef NDEBUG
        friend std::ostream& operator<<(std::ostream& os, const NogoodDB& db) {
            os << "(s) NogoodDB (" << db.nogoods.size() << ")\n";
            typename std::list<nogoodNode>::const_iterator it = db.nogoods.begin();
            for (; it != db.nogoods.end(); it++)
                os << *it;
            os << "(e) NogoodDB\n";
            return os;
        }

        bool allNogoodsApplied(const CoreCSPStack& csp) const;
        #endif
};

}

#include "gqrtl/NogoodDB.tcc"

#endif // NOGOOD_DB
