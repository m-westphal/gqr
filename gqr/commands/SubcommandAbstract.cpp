// -*- C++ -*-

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
#include <cstdlib>
#include <cassert>
#include <fstream>

#include "gqr.h"

#include "commands/SubcommandAbstract.h"

#include "CombinedCalculusReader.h"
#include "CalculusReader.h"

#include "utils/Logger.h"

class Calculus;


std::string SubcommandAbstract::getProgramNameFromArgv(const arguments& argv) {
	assert(!argv.empty());
	if (!argv.empty()) return argv[0];

	return "";
}

std::string SubcommandAbstract::getSubcommandNameFromArgv(const arguments& argv) {
	if (argv.size() >= 2)
		return argv[1];
	return "";
}

SubcommandAbstract::arguments SubcommandAbstract::stripFirstArgument(const arguments& a) {
	if (a.empty()) return SubcommandAbstract::arguments(a);

	SubcommandAbstract::arguments r;
	for (size_t i = 1; i < a.size(); i++)
		r.push_back(a[i]);
	return r;
}

SubcommandAbstract::SubcommandAbstract(const arguments& a)
	: commandLine(stripFirstArgument(stripFirstArgument(a))),
	programName(getProgramNameFromArgv(a)),
	requestedSubcommandName(getSubcommandNameFromArgv(a)),
	dataDirs(), verbose(0), usedDataDir() {

	assert(!a.empty());

	if (getenv("GQR_DATA_DIR") != NULL) {
		const std::string env(getenv("GQR_DATA_DIR"));
		dataDirs.push_back(env);
	}

	dataDirs.push_back("./data");
	if (!gqr_default_data_dir.empty())
		dataDirs.push_back(gqr_default_data_dir);

	assert(!dataDirs.empty());
}

static void printCommandlineError(const std::string& s) {
	std::cerr << "Error parsing command line. ";
	std::cerr << s << std::endl;
}

void SubcommandAbstract::parseArguments(arguments& args) {
	arguments unused;
	for (size_t i = 0; i < args.size(); i++) {
		const std::string& arg = args[i];
		if (arg == "-v" || arg == "--verbose") {
			// TODO: support different levels of verbosity
			verbose = 100;
			continue;
		}
		unused.push_back(arg);
	}

	args = unused;
}

Calculus* SubcommandAbstract::readCalculus(arguments& args) {
	Logger calculusTime("Calculus load time", 0);
	calculusTime.start();

	Calculus* res = NULL;	// indicates failure
	std::vector<std::string> unused;

	const size_t last_element = args.size()-1;

	std::string name("");

	bool skip = false;
	for (size_t i = 0; i < args.size(); i++) {
		if (skip) {
			skip = false;
			continue;
		}

		if (args[i] == "-C") {
			if (i == last_element) {
				printCommandlineError("-C not followed by calculus name");
				return NULL;
			}
			if (!name.empty()) {
				printCommandlineError("More than one calculi given on command line");
				return NULL;
			}
			name = args[i+1];
			skip = true;
			continue;
		}

		// Unknown option
		unused.push_back(args[i]);
	}

	// parsed options
	if (name.empty()) {
		printCommandlineError("No calculus specified (-C calculus_name)");
		return NULL;
	}

	// trim to unused arguments
	args = unused;

	std::ifstream input;
	for (size_t i = 0; i < dataDirs.size(); i++) {
		usedDataDir = dataDirs[i];
		const std::string filename = usedDataDir + "/" + name + ".spec";

		if (verbose > 10)
			std::cout << "Trying to read calculus \"" << name
				<< "\" from \"" << filename << "\"\n";

		input.open(filename.c_str());
		if (input.is_open()) {
			if (verbose > 10)
				std::cout << "Success: \"" << filename << "\"\n";
			break;
		}
	}
	if (!input.is_open()) {
		std::cerr << "Failed to find .spec file\n";

		// try combination
		for (size_t i = 0; i < dataDirs.size(); i++) {
			usedDataDir = dataDirs[i];
			const std::string filename = usedDataDir + "/" + name + ".combination";

			if (verbose > 10)
				std::cout << "Trying to read calculus \"" << name
					<< "\" from \"" << filename << "\"\n";

			input.open(filename.c_str());
			if (input.is_open()) {
				if (verbose > 10)
					std::cout << "Success: \"" << filename << "\"\n";
				break;
			}
		}
		if (!input.is_open()) {
			usedDataDir = "";
			std::cerr << "Failed to find .combination file\n";
			return NULL;
		}
		CombinedCalculusReader reader(name, usedDataDir, &input);
		res = reader.makeCalculus();
		calculusTime.end();
		if (verbose > 0)
			calculusTime.postLog("", 1, "calculi");
		return res;
	}

	CalculusReader reader(name, usedDataDir, &input);
	res = reader.makeCalculus();

	if (res == NULL) {
		// TODO: print some error message
		return NULL;
	}

	calculusTime.end();
	if (verbose > 0)
		calculusTime.postLog("", 1, "calculi");

	return res;
}
