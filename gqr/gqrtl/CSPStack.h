// -*- C++ -*-

// Copyright (C) 2012       Matthias Westphal
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

#ifndef CSPSTACK_H
#define CSPSTACK_H

#include <cassert>
#include <vector>
#include <list>

#include "Tuple.h"
#include "gqrtl/CSP.h"

namespace gqrtl {

/**
 * Template class representing a stack of CSPs.
 */
template<class R, class C>
class CSPStack {
	private:
		CSP<R, C> csp;

		typedef std::pair<Tuple, R> change;
		std::list<std::vector<change> > trail;

		inline void updateTrail(const Tuple t, const R& o, const R& n) {
			if (trail.empty()) return;

			const R diff = o & csp.getCalculus().getNegation(n);
			trail.back().push_back(std::make_pair(t, diff));
		}

	public:
		CSPStack(const CSP<R, C>& input) : csp(input) {}

		// remember current state for trailing
		inline void backupState() {
			trail.push_back(std::vector<change>());
		}

		// set a value
		inline void setValue(const Tuple& var, const R r) {
			updateTrail(var, csp.getConstraint(var.x, var.y), r);

			csp.setConstraint(var.x, var.y, r);
		}
		inline void setConstraint(const size_t i, const size_t j, const R r) { return setValue(Tuple(i,j), r); }

		void resetToLastState() {
			assert(!trail.empty());

			for (typename std::vector<change>::const_iterator it = trail.back().begin();
				it != trail.back().end(); ++it) {
				const Tuple& t = it->first;
				const R& bits = it->second;

				R update = csp.getConstraint(t.x, t.y);
				update |= bits;
				csp.setConstraint(t.x, t.y, update);
			}
			trail.pop_back();
		}

		void resetToInitialState() { while(!trail.empty()) resetToLastState(); }

		// read a value
		inline const R& getValue(const Tuple& t) const { return csp.getConstraint(t.x, t.y); }
		inline const R& getConstraint(const size_t i, const size_t j) const { return csp.getConstraint(i, j); }

		/** Get the size of the CSP @return the number of nodes in the CSP */
		const size_t getSize() const { return csp.getSize(); }
		/** Get pointer to Calculus object */
		const C& getCalculus() const { return csp.getCalculus(); }


		// Check whether current state of stacks is equal
		bool operator== (const CSPStack<R, C>& b) const {
			return csp == b.csp;
		}

		const CSP<R, C>& getCSP() { return csp; } // TODO: really necessary?
};

}

#endif // CSPStack_H
