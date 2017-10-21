// -*- C++ -*-

// Copyright (C) 2011 Thomas Liebetraut
// Copyright (C) 2008,2012 Matthias Westphal
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

#ifndef SUBCOMMANDHELP_H_
#define SUBCOMMANDHELP_H_

#include "commands/SubcommandAbstract.h"

#include <string>
#include <vector>

class SubcommandHelp : public SubcommandAbstract {
	public:
		static std::string helpString;

		SubcommandHelp(const std::vector<std::string>&);

		virtual ~SubcommandHelp() { }

		int run();
};

#endif /* SUBCOMMANDHELP_H_ */
