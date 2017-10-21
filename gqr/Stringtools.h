// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
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

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <string>
#include <vector>

class Stringtools {
	public:

		// remove " \t\n" from start/end of string
		static std::string trim(const std::string&);

		/**
		 * Split string into several pieces
		 *
		 * @param s - the string
		 * @param c - the character to be used to split the string
		 * @return a pointer to vector of strings containing the pieces
		 */
		static std::vector<std::string> split(const std::string& s, char c);
};
#endif //STRINGTOOLS_H
