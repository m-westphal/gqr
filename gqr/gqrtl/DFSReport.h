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

#ifndef DFSREPORT_H
#define DFSREPORT_H

#include <stddef.h>

class Logger;

namespace gqrtl {

// Implements an abstract view of the progress of a DFS for log/statistic
// purposes
class DFSReport {
	protected:
		// search information
		size_t number_positive_decisions;
		size_t number_negative_decisions;
		size_t number_implied_decisions;

		// propagation
		size_t propagation_calls;

		// branch information
		size_t depth;
		size_t unassigned_variables;

		size_t branch_positive_decisions;
		size_t branch_negative_decisions;
		size_t branch_implied_decisions;

		// interval based stats
		size_t visited_depth_min;
		size_t visited_depth_max;

	public:
		DFSReport()
			: number_positive_decisions(0), number_negative_decisions(0),
			  number_implied_decisions(0), propagation_calls(0),
			  depth(0), unassigned_variables(0),
			  visited_depth_min(0), visited_depth_max(0) {}

	friend class ::Logger; // Logger may read data in here
};

}

#endif
