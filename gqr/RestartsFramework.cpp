// -*- C++ -*-

// Copyright (C) 2010-2012 Matthias Westphal
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

#include <sstream>
#include <cassert>

#include "utils/Logger.h"

#include "RestartsFramework.h"

RestartsFramework::RestartsFramework() :
	log(NULL),
	restartsHappened(0),
	nextCutoffValue(0),
	useRestarts(true), minimizeNogoods(false),
	strategy(Geometric), cutoff(10) {

	initialize();
}

RestartsFramework::~RestartsFramework() {
	delete log;
}

void RestartsFramework::initialize() {
	delete log;
	log = new Logger("Restarts Framework", 30*1000);
	restartsHappened = 0;
	nextCutoffValue = 0;

	luby_seq.clear();
	luby_seq.push_back(1);
	luby_seq_pos = luby_seq.begin();
}

size_t RestartsFramework::getNextCutoff() {
	if (restartsHappened == 0) {
		nextCutoffValue = cutoff;
		log->start();
	}
	restartsHappened++;

	const size_t res = nextCutoffValue;

	switch(strategy) {
		default:
			assert(false);
		case Luby:
			luby_seq_pos++;
			if (luby_seq_pos == luby_seq.end()) { // done with sequence; calculate s(n+1)
				luby_seq_pos--;
				luby_seq.insert(luby_seq.end(), luby_seq.begin(), luby_seq.end()); // s(n), s(n)
				luby_seq.push_back(luby_seq.back() * 2); // s(n), s(n), 2^{n+1}
				luby_seq_pos++; // point to first element of second s(n)
			}
			nextCutoffValue += cutoff*(*luby_seq_pos);
			break;
		case Geometric:
			nextCutoffValue += nextCutoffValue/2;
			break;
	}

	if (log && log->logDue()) {
		std::ostringstream out;
		if (restartsHappened == 1)
			out << "\tInitial start;";
		else
			out << "\tRestart #" << (restartsHappened-1) << ";";
		out << " next restart scheduled for " << res << "\n";

		if (strategy == Luby) {
			out << "\tLuby seq.: ";
			for (std::list<size_t>::iterator it = luby_seq.begin(); it != luby_seq.end(); it++) {
				if (it == luby_seq_pos)
					out << "* ";
				else
					out << " ";
				out << *it;
			}
			out << std::endl;
		}

		log->postLog(out.str(), restartsHappened, "restarts");
	}

	return res;
}
