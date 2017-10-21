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

#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include "Timer.h"

namespace gqrtl {
	class DFSReport;
}

class Logger {
	private:
		const std::string name;

		 /** time when start() was called */
		Timer startTime;

		 /** time when end() was called */
		Timer endTime;

		/** last time progress was updated */
		Timer lastLogTime;

		/** interval for progress reports (in milliseconds)*/
		const long int logInterval;

		// was start() called?
		bool unused;

		// was end() called?
		bool finished;
	public:
		void start();
		void end();

		void postLog(const std::string& msg, const size_t steps, const std::string& unit);

		void postDFSLog(const gqrtl::DFSReport&);

		// Print name, value pairs in an regexp friendly way
		// This function may only be called after end()
		void finalReport(const bool satisfiable, const gqrtl::DFSReport&);

		inline bool logDue() const {
			if (logInterval < 0 || unused)
				return false;

			const Timer current = Timer();
			if (current.msec_passed(lastLogTime) >= logInterval)
				return true;
			return false;
		}

		// Logger with associated interval
		Logger(const std::string& n, const long int);
};

#endif
