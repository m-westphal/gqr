// -*- C++ -*-

// Copyright (C) 2011 Thomas Liebetraut
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

#include <cassert>
#include <iostream>
#include <sstream>

#include "commands/SubcommandVersion.h"

int SubcommandVersion::run() {
	assert(requestedSubcommandName == "version");

	std::cout << helpString;
	if (commandLine.empty())
		return 0;
	return 1;
}

std::string SubcommandVersion::helpString = std::string( "GQR build ")
	+ std::string(GQR_VERSION)
#ifndef NDEBUG
	+ std::string(" (slow debug version)")
#else
	+ std::string("-NDEBUG (release version)")
#endif
	+ std::string(", compiled with gcc " __VERSION__ ". ")
	+ std::string(__DATE__ " " __TIME__)
	+ std::string("\n");
