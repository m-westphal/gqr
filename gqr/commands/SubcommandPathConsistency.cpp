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

#include <iostream>
#include <cassert>
#include <cstdlib>

#include <fstream>

#include "utils/Timer.h"
#include "utils/Logger.h"

#include "SubcommandPathConsistency.h"
#include "Calculus.h"
#include "CSPReader.h"

#include "gqrtl/RelationFixedBitset.h"
#include "gqrtl/CSP.h"
#include "CSPSparse.h"
#include "gqrtl/CalculusOperations.h"
#include "gqrtl/WeightedTripleIterator.h"

std::string SubcommandPathConsistency::helpString = std::string(
	"Path consistency: enforce path consistency on constraint networks described in 'file_i'\n"
	"Usage: gqr \"path consistency\" -C calculus [Options] file_1 [file_2 ...]\n"
	"\"path consistency\" (pc) subcommand options:\n"
	"  -C calc                  choose calculus 'calc'\n"
	"  -n                       show only information about inconsistent ('negative') networks\n"
	"                           (suppresses -p)\n"
	"  -p                       show only information about consistent ('positive') networks\n"
	"                           (suppresses -n)\n"
	"  -S, --solution           display solution (if any) for each network\n"
	"  -q                       return state of last CSP (0 inconsistent, 1 otherwise)\n"
);

SubcommandPathConsistency::SubcommandPathConsistency(const std::vector<std::string>& a) : SubcommandAbstract(a),
unusedArgs(commandLine),
negativeOnly(false), positiveOnly(false),
returnState(false),
//swPrintConvTable(false), swPrintCompTable(false), swPrintBaseRelations(false),
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

SubcommandPathConsistency::~SubcommandPathConsistency() {
	delete calculus;
}

bool SubcommandPathConsistency::parseUnusedArgs() {
	std::vector<std::string> new_unused;

	for (size_t i = 0; i < unusedArgs.size(); i++) {
		if (unusedArgs[i] == "-n") {
			negativeOnly = true;
			positiveOnly = false;
		}
		else if (unusedArgs[i] == "-q") {
			returnState = true;
		}
		else if (unusedArgs[i] == "-p") {
			negativeOnly = false;
			positiveOnly = true;

		}
		else if (unusedArgs[i] == "-S") {
			showSolution = true;
		}
		else {
			new_unused.push_back(unusedArgs[i]);
		}
	}

	unusedArgs = new_unused;

	#ifndef NDEBUG
	if (!unusedArgs.empty()) {
		std::cout << "Remaining (unparsed) arguments:\n";
		for (size_t i = 0; i < unusedArgs.size(); i++)
			std::cout << "\t" << unusedArgs[i] << std::endl;
		std::cout << std::endl;
	}
	#endif

	return true;
}

int SubcommandPathConsistency::run() {
	if (commandLine.empty() || unusedArgs.empty()) {
		std::cout << helpString;
		return 1;
	}
	if (!calculus) // must be set up
		return 1;

	const bool lastState = applyPathConsistency(unusedArgs);

	if (returnState) {
		// return state of last CSP (0 inconsistent, 1 otherwise)
		if (lastState)
			return 1;
		return 0;
	}

	return 0;	// no errors
}

template<class R>
int SubcommandPathConsistency::runCoreTemplate<R>::execute(const std::string& name) {
	assert(calculus);

	std::ifstream i;
	i.open(name.c_str());
	if (!i.is_open()) { // TODO: move somewhere else
		std::cerr << "Failed to open CSP \"" << name << "\"\n";
		return -1;
	}

	CSPReader r(&i, calculus->getCalculus());

	bool path_consistent = true;	// TODO: there is no reasonable default value

	CSPSparse* input;
	while ( (input = r.makeCSP()) != NULL) {
		Logger groundTime("CSP ground time", 0);
		groundTime.start();

		typedef gqrtl::CSP<R, gqrtl::CalculusOperations<R> > GroundedRep;
		GroundedRep csp(*input, *calculus);
		gqrtl::WeightedTripleIterator<R, GroundedRep> propagation;

		groundTime.end();
		groundTime.postLog("", 1, "CSPs");

		path_consistent = (propagation.enforce(csp).empty());

		std::cout << csp.name;
		if (!path_consistent) {
			if (!positiveOnly)
				std::cout << ": 0\n";
		}
		else {
			if (!negativeOnly) {
				std::cout << ": 1\n";
				if (showSolution) {
					std::cout << csp.getSize()-1 << " " << csp.name << std::endl;
					for (size_t i = 0; i < csp.getSize(); ++i)
						for (size_t j = i+1; j < csp.getSize(); ++j) {
							const R& rel = csp.getConstraint(i,j);
							if (rel != calculus->getUniversalRelation()) {
								std::cout << i << " " << j << " ";
								std::cout << calculus->getCalculus().relationToString(rel.getRelation());
								std::cout << std::endl;
							}
						}
					std::cout << ".\n";
				}
			}
		}

		delete input;
	}
	i.close();

	if (path_consistent)
		return 1;
	return 0;
}

template<class R>
bool SubcommandPathConsistency::runCoreTemplate<R>::ground(const Calculus &c) {
	if (R::maxSize() < c.getNumberOfBaseRelations())
		return false;
#ifndef NDEBUG
std::cout << "gqrtl::Calculus build on relations with a maximum of " << R::maxSize() << " base relations\n";
#endif
	Logger groundTime("Calculus ground time", 0);
	groundTime.start();

	R::init();
	calculus = new gqrtl::CalculusOperations<R>(c);

	groundTime.end();
	groundTime.postLog("", 1, "calculi");

	return true;
}

template<class R>
SubcommandPathConsistency::runCoreTemplate<R>::~runCoreTemplate() {
	if (!calculus) return;
	delete calculus;
	R::clean_up();
}

bool SubcommandPathConsistency::applyPathConsistency(const std::vector<std::string>& filenames) const {
	std::vector<runCore*> cores;

	cores.push_back(new runCoreTemplate<gqrtl::Relation8>(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::Relation16>(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::Relation32>(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 1> >(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 2> >(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 4> >(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 5> >(positiveOnly, negativeOnly, showSolution));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 10> >(positiveOnly, negativeOnly, showSolution));
	// Fallback default code
	cores.push_back(new runCoreTemplate<Relation>(positiveOnly, negativeOnly, showSolution));

	size_t core;
	for(core = 0; core < cores.size(); core++)
		if ((cores[core])->ground(*calculus))
			break;

	bool lastState = false;		// inconsistent
	Timer start = Timer();

	for (size_t i = 0; i < filenames.size(); i++) {
		const int ret = (cores[core])->execute(filenames[i]);
		if (ret < 0) { // TODO: need some error handling ...
			for (size_t c = 0; c < cores.size(); c++) delete cores[c];
			return false;
		}
		lastState = (ret == 1);
	}

	long int total_ms = std::labs(start.msec_passed(Timer()));
	std::cout << "CSPs processing time: " << ((double) total_ms) / 1000.0 << " seconds" << std::endl;

	for (size_t c = 0; c < cores.size(); c++) delete cores[c];
	return lastState;
}
