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

#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>

#include "gqrtl/DFSReport.h"
#include "Logger.h"

Logger::Logger(const std::string&n, const long int i) : name(n), logInterval(i), unused(true), finished(false) {
	lastLogTime = startTime = Timer();
}

void Logger::start() {
	assert(unused);
	assert(!finished);

	unused = false;

	// set to current time
	startTime = Timer();
	lastLogTime = startTime;
}

void Logger::end() {
	assert(!unused);
	assert(!finished);

	finished = true;

	endTime = Timer();
}

void Logger::postLog(const std::string& msg, const size_t steps, const std::string& unit) {
	assert(logDue() || finished);

	long int timeSinceStart;
	if (!finished) {
		const Timer current = Timer();
		lastLogTime = current;
		timeSinceStart = current.msec_passed(startTime);
	}
	else {
		timeSinceStart = endTime.msec_passed(startTime);
	}


	std::cout << std::fixed << std::setprecision (3);
	std::cout << "[" << ((double) timeSinceStart/1000.0) << " sec] ";
	std::cout << name << " ";


	if (timeSinceStart != 0) {
		const double speed = (((double) steps) / (double) timeSinceStart) * 1000.0;
		std::cout << "(raw speed " << speed << " " << unit <<"/sec)\n";
	}
	else
		std::cout << std::endl;

	std::cout << msg << std::flush;
}

void Logger::postDFSLog(const gqrtl::DFSReport& dfs) {
	std::ostringstream out;

	out << "\t(in total) positive decisions=" << dfs.number_positive_decisions;
	out << ", negative decisions=" << dfs.number_negative_decisions;
	out << ", implied decisions=" << dfs.number_implied_decisions;
	out << ", propagation calls=" << dfs.propagation_calls;

	out << "\n\t(current search branch)";
	out << " positive decisions=" << dfs.branch_positive_decisions;
	out << ", negative decisions=" << dfs.branch_negative_decisions;
	out << ", implied decisions=" << dfs.branch_implied_decisions;

	out << "\n\tsearch tree depth:";
	out << " current=" << dfs.depth;
	out << ", during last interval [" << dfs.visited_depth_min << ", " << dfs.visited_depth_max << "]";

	out << "\n\tProgress est.: ";
	out << "unassigned variables remaining=" << dfs.unassigned_variables;
	out << std::endl;

	const std::string unit("pos. decisions");
	const size_t steps = dfs.number_positive_decisions;
	postLog(out.str(), steps, unit);
}

void Logger::finalReport(const bool s, const gqrtl::DFSReport& dfs) {
	assert(finished);

	// inspired by the SAT competition output rules
	const long int timeSinceStart = endTime.msec_passed(startTime);

	std::cout << std::fixed << std::setprecision (3);
	std::cout << "c cpu_and_sys_msec=";
	std::cout << ((double) timeSinceStart/1000.0) << std::endl;
	std::cout << "c positive_decisions=" << dfs.number_positive_decisions << std::endl;
	std::cout << "c propagation_calls=" << dfs.propagation_calls << std::endl;

	if (s)
		std::cout << "s SATISFIABLE" << std::endl;
	else
		std::cout << "s UNSATISFIABLE" << std::endl;
	std::cout << std::flush;
}
