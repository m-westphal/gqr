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

#include "Stringtools.h"

std::string Stringtools::trim(const std::string& s) {
	const size_t start = s.find_first_not_of(" \t\n");
	const size_t end = s.find_last_not_of(" \t\n");

	if (start == std::string::npos) {
		return std::string("");
	}

	return s.substr(start, end - start + 1);
}

/**
 * Split string into several pieces
 *
 * @param s - the string
 * @param c - the character to be used to split the string
 * @return a pointer to vector of strings containing the pieces
 */
std::vector<std::string> Stringtools::split(const std::string& s, char c) {
	std::vector<std::string> result;

	size_t pos = 0;
	while ( (pos = s.find_first_not_of(c, pos)) != std::string::npos) {
		const size_t end = s.find_first_of(c, pos);

		std::string partial_result = s.substr(pos, end - pos);
		result.push_back(partial_result);

		pos = end;
	}

	return result;
}
