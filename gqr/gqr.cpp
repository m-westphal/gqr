// -*- C++ -*-

// Copyright (C) 2006, 2007, 2008 Zeno Gantner, Stefan Woelfl
// Copyright (C) 2006, 2007 Micha Altmeyer
// Copyright (C) 2007-2012 Matthias Westphal
// Copyright (C) 2011 Thomas Liebetraut
//
// This file is part of the Generic Qualitative Reasoner GQR.  This
// program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to
// the Free Software Foundation, 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.

#include <cassert>
#include <iostream>
#include <string>
#include <limits.h>

#include <cstdlib>



//#include "commands/SubcommandConsistency.h"
//#include "commands/SubcommandMinCsp.h"
//#include "commands/SubcommandMultiConsistency.h"
//#include "commands/SubcommandMultiPathConsistency.h"

#include "gqr.h"

#include "commands/SubcommandAbstract.h"
#include "commands/SubcommandHelp.h"
#include "commands/SubcommandVersion.h"
#include "commands/SubcommandCheck.h"
#include "commands/SubcommandPathConsistency.h"
#include "commands/SubcommandConsistency.h"

/**
 * Generic solver for binary constraint networks.
 *
 * @see USAGE_STRING
 */

static SubcommandAbstract* getSubcommand(const int argc, char *argv[]) {
	// if there was no command specified, we want the help command to be
	// executed

	std::vector<std::string> args;
	for (int i = 0; i < argc; i++)
		args.push_back(std::string(argv[i]));

	std::string s;
	if (args.size() >= 2) // options given
		s = args[1];
	// The first argument determines the type of the subcommand
	// Then delegate the parameter parsing to the specific command


	if (s == "help")
		return new SubcommandHelp(args);
	if (s == "version")
		return new SubcommandVersion(args);
	if (s == "check")
		return new SubcommandCheck(args);
	if (s == "pc" || s == "path consistency")
		return new SubcommandPathConsistency(args);
	if (s == "c" || s == "consistency")
		return new SubcommandConsistency(args);

/*
//	Disabled for now

	if (s == "mincsp" || s == "min csp")
		return new SubcommandMinCsp(args);

	if (s == "mpc" || s == "multi path consistency")
		return new SubcommandMultiPathConsistency(args);

	if (s == "mc" || s == "multi consistency")
		return new SubcommandMultiConsistency(args);
*/

	return new SubcommandHelp(args);
}

int main(int argc, char* argv[])
{
	SubcommandAbstract *cmd = getSubcommand(argc, argv);
	const int ret = cmd->run();
	delete cmd;

	return ret;
}
