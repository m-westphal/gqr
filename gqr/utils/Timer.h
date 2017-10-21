// -*- C++ -*-

// Copyright (C) 2010 Matthias Westphal
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

#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <sys/time.h>

/** Class that stores a point in time */
class Timer {
    private:
	timeval getCurrentTime() const;
	/** the stored time */
	timeval time;
    public:
	const timeval& getTime() const { return time; }

	Timer() : time(getCurrentTime()) {}

	/** returns milliseconds passed between this instance and t */
	long int msec_passed(const Timer& t) const;
};

#endif
