// -*- C++ -*-

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

#ifndef SUBCOMMANDCHECK_H_
#define SUBCOMMANDCHECK_H_

#include "SubcommandAbstract.h"

class SubcommandCheck: public SubcommandAbstract {
	private:
		std::vector<std::string> unusedArgs;
		bool parseUnusedArgs();

		static void printBaseRelations(const Calculus& c);
		static void printConverseTable(const Calculus& c);
		static void printCompositionTable(const Calculus& c);

		bool swPrintConvTable;
		bool swPrintCompTable;
		bool swPrintBaseRelations;

		Calculus* calculus;

	public:
		SubcommandCheck(const std::vector<std::string>& a);
		virtual ~SubcommandCheck();

		static std::string helpString;

		int run();
};

#endif /* SUBCOMMANDCHECK_H_ */
