// -*- C++ -*-

// Copyright (C) 2006, 2007, 2010 Stefan Woelfl
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

#include "SubcommandCheck.h"

#include "utils/Logger.h"

#include "Calculus.h"

#include <iostream>
#include <cstdlib>

SubcommandCheck::SubcommandCheck(const std::vector<std::string>& a) : SubcommandAbstract(a),
unusedArgs(commandLine), swPrintConvTable(false), swPrintCompTable(false), swPrintBaseRelations(false),
calculus(NULL) {
	if (!commandLine.empty()) {
		calculus = readCalculus(unusedArgs);
		if (!calculus)
			return;
		if (!parseUnusedArgs()) {
			delete calculus;
			calculus = NULL;
			return;
		}
	}
}

SubcommandCheck::~SubcommandCheck() {
	delete calculus;
}

std::string SubcommandCheck::helpString = std::string(
	"Check: check algebraic properties of given calculus\n"
	"Usage: gqr \"check\" -C calculus [Options]\n"
	"\"check\" subcommand options:\n"
	"  -C calc                  choose calculus 'calc'\n"
	"  --print-comp-table       output the composition table\n"
	"  --print-conv-table       output the converse table\n"
	"  --print-base-relations   output the list of base relations\n"
	"Check returns 0 if no errors have been found in the calculus definition, 1 otherwise\n"
);

bool SubcommandCheck::parseUnusedArgs() {
	std::vector<std::string> new_unused;

	for (size_t i = 0; i < unusedArgs.size(); i++) {
		if (unusedArgs[i] == "--print-comp-table") {
			swPrintCompTable = true;
		} else if (unusedArgs[i] == "--print-conv-table") {
			swPrintConvTable = true;
		} else if (unusedArgs[i] == "--print-base-relations") {
			swPrintBaseRelations = true;
		}
		else {
			new_unused.push_back(unusedArgs[i]);
		}
	}

	unusedArgs = new_unused;

	if (!unusedArgs.empty()) {
		std::cerr << "SubcommandCheck: unknown command line options given\n";
		std::cerr << "Unknown:";
		for (size_t i = 0; i < unusedArgs.size(); i++)
			std::cerr << " " << unusedArgs[i];
		std::cerr << std::endl;

		return false;
	}

	return true;
}

int SubcommandCheck::run() {
	if (commandLine.empty()) {
		std::cout << helpString;
		return 0;
	}
	if (!calculus) // must be set up
		return 1;

	if (swPrintBaseRelations)
		printBaseRelations(*calculus);
	if (swPrintConvTable)
		printConverseTable(*calculus);
	if (swPrintCompTable)
		printCompositionTable(*calculus);

	Logger checkCalculusTime("Processing time", 0);
	checkCalculusTime.start();

	bool found_error = false;
	if (!calculus->checkConverseTable())
		found_error = true;
	if (!calculus->checkCompositionTable())
		found_error = true;

	if (found_error)
		std::cerr << "Calculus specification contains errors\n";
	else {
		std::cout << "No errors found in calculus specification\n";

		// TODO: do we want to check for splitter errors?
		// Currently splitter is auto computed inside GQR, so it is debug, not
		// input checking
/*		bool found_splitter_error = false;
		if (checkSplitter != NULL) {
		found_checkSplitter_error = (checkCheckSplitset() != true);
		if (!found_checkSplitter_error)
			std::cout << "No errors found in splitset specification\n";
		else {
			std::cerr << "Splitset specification contains errors\n";
			found_error = true;
		}
		}*/
	}

	checkCalculusTime.end();
	checkCalculusTime.postLog("", 1, "calculi");

	if (found_error)
		return 1;

	return 0;
}

void SubcommandCheck::printBaseRelations(const Calculus& c) {
	std::cout << "All base relations of this calculus:" << std::endl;
	std::cout << "Index\t" << "Name\t" << "Encoding\n";

	for (size_t i = 0; i < c.getNumberOfBaseRelations(); ++i) {
		const std::string& name = c.getBaseRelationName(i);

		Relation rel;
		rel.set(i);

		std::cout << i << "\t" << name << "\t" << rel << std::endl;
	}

	std::cout << std::endl;
}

void SubcommandCheck::printConverseTable(const Calculus& c) {
	std::cout << "Converse table:" << std::endl;

	for (size_t i = 0; i < c.getNumberOfBaseRelations(); ++i) {
		const std::string& relname1 = c.getBaseRelationName(i);
		const size_t res = c.getBaseRelationConverse(i);

		const std::string& relname2 = c.getBaseRelationName(res);
		std::cout << relname1 << " :: " << relname2 << std::endl;
	}

	std::cout << std::endl;
}

void SubcommandCheck::printCompositionTable(const Calculus& c) {
	std::cout << "Composition table:" << std::endl;

	for (size_t i = 0; i < c.getNumberOfBaseRelations(); ++i) {
		const std::string& relname1 = c.getBaseRelationName(i);

		for (size_t j = 0; j < c.getNumberOfBaseRelations(); ++j) {
			// format: = : = :: ( = )
			const std::string& relname2 = c.getBaseRelationName(j);
			std::cout << relname1 << " : ";
			std::cout << relname2 << " :: ";
			const Relation comp = c.getBaseRelationComposition(i, j);
			std::cout << c.relationToString(comp) << std::endl;
		}
	}

	std::cout << std::endl;
}
