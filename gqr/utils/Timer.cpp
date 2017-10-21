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

#include <cmath>
#include <cassert>
#include <stdlib.h>
#include <iostream>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#include "Timer.h"

struct timeval Timer::getCurrentTime() const {
    struct timeval current_time;
    struct rusage usage;

    if(getrusage(RUSAGE_SELF, &usage)) {
	std::cerr << "getrusage() failed: ";
	char* s = strerror(errno);
	std::cerr << std::string(s);
	free(s);

	return current_time; // TODO implement error handling
    }

    current_time = usage.ru_utime;
    current_time.tv_sec += usage.ru_stime.tv_sec;
    current_time.tv_usec += usage.ru_stime.tv_usec;

    while (current_time.tv_usec >= 1000*1000) {
	current_time.tv_sec++;
	current_time.tv_usec -= 1000*1000;
    }
    while (current_time.tv_usec < 0) {
	current_time.tv_sec--;
	assert( current_time.tv_sec >= 0);
	current_time.tv_usec += 1000*1000;
    }
    assert(current_time.tv_usec < 1000*1000);

    assert(0 <= current_time.tv_usec);
    assert(0 <= current_time.tv_sec);

    return current_time;
}

long int Timer::msec_passed(const Timer& t) const {
    long int val_ms = (time.tv_sec - t.getTime().tv_sec) * 1000;
    long int val_usec = time.tv_usec - t.getTime().tv_usec;

    if (val_usec < 0) {
	val_ms -= 1000;
	val_usec += 1000*1000;

	assert(val_usec >= 0);
    }
    val_ms += val_usec / 1000;

    return val_ms;
}
