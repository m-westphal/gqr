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

#ifndef RESTARTS_FRAMEWORK
#define RESTARTS_FRAMEWORK

#include <cstddef>
#include <list>

class Logger;

class RestartsFramework {
	private:
		Logger* log;

		size_t restartsHappened;

		// value when next restart happens
		size_t nextCutoffValue;

		// Luby sequence (if required)
		std::list<size_t> luby_seq;
		std::list<size_t>::const_iterator luby_seq_pos;
	public:
		enum Strategy {Geometric, Luby};

		bool useRestarts;
		bool minimizeNogoods;
		Strategy strategy;
		size_t cutoff;

		size_t getNextCutoff();

		void initialize();

		RestartsFramework();
		~RestartsFramework();
};

#endif
