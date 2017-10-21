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

#include <iostream>
#include <cassert>

#include "SubcommandHelp.h"
#include "SubcommandVersion.h"
#include "SubcommandCheck.h"
#include "SubcommandPathConsistency.h"
#include "SubcommandConsistency.h"

std::string SubcommandHelp::helpString = std::string(
	"Usage: gqr <subcommand> -C calculus [Options] "
	"file_1 [file_2 ...] \n"
	"Available subcommands:\n"
	"       consistency (c)                 "
	"Solve constraint networks described in 'file_i'\n"
	"       path consistency (pc)           "
	"Enforce path consistency on constraint networks described in 'file_i'\n"
	"       check                           "
	"Check algebraic properties of given calculus\n"
	"       version                         "
	"Show version information and exit\n"
	"       help                            "
	"Show this usage information and exit\n"
	"\n"
	"Type: \"gqr help <subcommand>\" for subcommand specific help\n"
);

/*
//	Disabled features
	"       min csp (mincsp)                "
	"Calculate minimal CSP for each 'file_i'\n"
	"       multi consistency (mc)          "
	"Solve a series of constraint network tuples described in files 'file_1' ... 'file_n'\n"
	"       multi path consistency (mpc)    "
	"Enforce path consistency on constraint network tuples described in files\n"
	"                                       'file_1' ... 'file_n'\n"
*/

SubcommandHelp::SubcommandHelp(const std::vector<std::string>& argv) : SubcommandAbstract(argv) { }

int SubcommandHelp::run() {
	if (requestedSubcommandName == "" && commandLine.empty()) {
		std::cout << SubcommandVersion::helpString;
		std::cout << SubcommandHelp::helpString;
		return 0;
	}

	if (requestedSubcommandName != "help") {
		// not explicitly requested "help"
		std::cout << SubcommandHelp::helpString;
		return 1;
	}

	assert(requestedSubcommandName == "help");

	if (commandLine.empty()) {
		std::cout << SubcommandHelp::helpString;
		return 0;
	}
	if (commandLine.size() > 1) {	// more than one argument? Unclear what is meant by the user!
		std::cout << SubcommandHelp::helpString;
		return 1;
	}
	const std::string& helpFor = commandLine[0];

	if (helpFor == "version") {
		std::cout << SubcommandVersion::helpString;
		return 0;
	}
	if (helpFor == "help") {
		std::cout << SubcommandHelp::helpString;
		return 0;
	}
	if (helpFor == "check") {
		std::cout << SubcommandCheck::helpString;
		return 0;
	}
	if (helpFor == "pc" || helpFor == "path consistency") {
		std::cout << SubcommandPathConsistency::helpString;
		return 0;
	}
	if (helpFor == "c" || helpFor == "consistency") {
		std::cout << SubcommandConsistency::helpString;
		return 0;
	}

/*    if (argc > 0)
    {
        std::string s(argv[0]);
        // the first argument to the help command specifies what command
        // the help text should be displayed for.
        } else if (s == "mincsp" || s == "min csp")
        {
            this->helpString = SubcommandMinCsp::help();
        } else if (s == "mpc" || s == "multi path consistency")
        {
            this->helpString = SubcommandMultiPathConsistency::help();
        } else if (s == "mc" || s == "multi consistency")
        {
            this->helpString = SubcommandMultiConsistency::help();
        }
    }
}
*/

	std::cerr << "Unknown help section " << helpFor << std::endl;

	return 1;
}
