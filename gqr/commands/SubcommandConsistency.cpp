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

#include <sstream>

#include <fstream>

#include "utils/Timer.h"
#include "utils/Logger.h"

#include "SubcommandConsistency.h"
#include "Calculus.h"
#include "CSPReader.h"

#include "FileSplitter.h"

#include "gqrtl/RelationFixedBitset.h"
#include "gqrtl/CSP.h"
#include "CSPSparse.h"
#include "gqrtl/CalculusOperations.h"
#include "gqrtl/DFS.h"
#include "gqrtl/RestartingDFS.h"

#include "RestartsFramework.h"

std::string SubcommandConsistency::helpString = std::string(
	"Consistency: solve constraint networks described in 'file_i'\n"
	"Usage: gqr consistency -C calculus [Options] file_1 [file_2 ...] \n"
	"\"consistency\" (c) subcommand options:\n"
	"  -C calc                  choose calculus 'calc'\n"
	"  -c cover                 use 'cover' for splitting in the backtracking step\n"
	"  -S, --solution           display solution (if any) for each network\n"
	"  -q                       return state of last CSP (0 inconsistent, 1 otherwise)\n"
//	"  -A                       find all consistent atomic sub networks\n"
	"  -v, --verbose            show number of nodes visited during backtracking for\n"
	"                           each network (suppresses -q)\n"
	"  --2w                     use 2-way DFS without restarts\n"
	"\n"
	"  --restarts-geometric     use 2-way DFS with geometric restarting strategy [default]\n"
	"  --restarts-luby          use 2-way DFS with luby restarting strategy\n"
	"  --cutoff n               initial cutoff value [default 10]\n"
	"  --minimize-nogoods       minimize each learnt nogoods\n"
);

SubcommandConsistency::SubcommandConsistency(const std::vector<std::string>& args)
	: SubcommandAbstract(args), unusedArgs(commandLine),
	showSolution(false),
	returnState(false),
	restartOptions(new RestartsFramework()),
	calculus(NULL) {

	if (!commandLine.empty()) {
		parseArguments(unusedArgs); // sets "verbose"

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

bool SubcommandConsistency::parseUnusedArgs() {
	std::vector<std::string> new_unused;

	bool skip = false;
	for (size_t i = 0; i < unusedArgs.size(); i++) {
		if (skip) {
			skip = false;
			continue;
		}

		if (unusedArgs[i] == "-S" || unusedArgs[i] == "--solution") {
			showSolution = true;
		}
		else if (unusedArgs[i] == "-q") {
			returnState = true;
		}
		else if (unusedArgs[i] == "-c") {
			if (i+1 == unusedArgs.size()) {
				std::cerr << "Missing argument \"-c\"\n";
				return false;
			}
			if (!calculus) return false; // TODO: not sure when/if this can happen

			if (calculus->getSplitter() != NULL) {
				std::cerr << "More than one cover set specified\n";
				return false;
			}

			skip = true;
			if (!setSplitter(unusedArgs[i+1]))
				return false;
		}
		else if (unusedArgs[i] == "--restarts-geometric") {
			restartOptions->useRestarts = true;
			restartOptions->strategy = RestartsFramework::Geometric;
		}
		else if (unusedArgs[i] == "--restarts-luby") {
			restartOptions->useRestarts = true;
			restartOptions->strategy = RestartsFramework::Luby;
		}
		else if (unusedArgs[i] == "--cutoff") {
			if (i+1 == unusedArgs.size()) {
				std::cerr << "Missing argument \"--cutoff\"\n";
				return false;
			}
			skip = true;

			std::stringstream in;
			in << unusedArgs[i+1];
			in >> restartOptions->cutoff;

			if (restartOptions->cutoff == 0) {
				std::cerr << "Zero cutoff given\n";
				return false;
			}
		}
		else if (unusedArgs[i] == "--minimize-nogoods") {
			restartOptions->minimizeNogoods = true;
		}
		else if (unusedArgs[i] == "--2w") {
			restartOptions->useRestarts = false;
		}
		// TODO: parse/support all options
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

SubcommandConsistency::~SubcommandConsistency() { delete calculus; delete restartOptions; }


bool SubcommandConsistency::setSplitter(const std::string& name) {
	Logger splitterTime("Cover set load time", 0);
	splitterTime.start();

	std::ifstream input;

	for (size_t i = 0; i < dataDirs.size(); i++) {
		const std::string& dataDir = dataDirs[i];
		const std::string algFilename = dataDir + "/" + calculus->getName() + "/calculus/" + name + "alg";
#ifndef NDEBUG
std::cout << "Trying to read cover set \"" << name << "\" from \"" << algFilename << "\"\n";
#endif
		input.open(algFilename.c_str());
		if (input.is_open())
			break;
	}
	if (!input.is_open()) { // TODO: move somewhere else
		std::cerr << "Failed to find splitter \"" << name << "\"\n";
		return false;
	}

	Splitter* splitter = new FileSplitter(*calculus, &input);
	calculus->setSplitter(splitter);

	splitterTime.end();
	if (verbose > 0)
		splitterTime.postLog("", 1, "cover sets");

	return true;
}

int SubcommandConsistency::run() {
	if (commandLine.empty() || unusedArgs.empty()) {
		std::cout << helpString;
		return 0;
	}
	if (!calculus) // must be set up
		return 1;

	const bool lastState = applyConsistency(unusedArgs);

	if (returnState) {
		return lastState;
	}

	return 0;	// no error
}

bool SubcommandConsistency::applyConsistency(const std::vector<std::string>& filenames) const {
	std::vector<runCore*> cores;

	bool b_verbose = false;
	if (verbose > 0)
		b_verbose = true;

	cores.push_back(new runCoreTemplate<gqrtl::Relation8>(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::Relation16>(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::Relation32>(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 1> >(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 2> >(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 4> >(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 5> >(showSolution, b_verbose, *restartOptions));
	cores.push_back(new runCoreTemplate<gqrtl::RelationFixedBitset<size_t, 10> >(showSolution, b_verbose, *restartOptions));
	// Fallback default code*/
	cores.push_back(new runCoreTemplate<Relation>(showSolution, b_verbose, *restartOptions));

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

template<class R>
int SubcommandConsistency::runCoreTemplate<R>::execute(const std::string& name) {
	assert(calculus);

	std::ifstream i;
	i.open(name.c_str());
	if (!i.is_open()) { // TODO: move somewhere else
		std::cerr << "Failed to open CSP \"" << name << "\"\n";
		return -1;
	}

	CSPReader r(&i, calculus->getCalculus());

	int ret = 1; // TODO there is no default value

	CSPSparse* input;
	while ( (input = r.makeCSP()) != NULL) {

		Logger groundTime("CSP ground time", 0);
		groundTime.start();

		typedef gqrtl::CSP<R, gqrtl::CalculusOperations<R> > GroundedRep;
		GroundedRep csp(*input, *calculus);

		groundTime.end();
		if (verbose)
			groundTime.postLog("", 1, "CSPs");

		Logger* log = NULL;
		if (verbose) {
			const std::string name = std::string("DFS on ")+input->name;
			log = new Logger(name, 30*1000);
		}


		GroundedRep* result;
		if (restartOptions.useRestarts) {
			restartOptions.initialize();
			gqrtl::RestartingDFS<R> search(csp, restartOptions, log);
			result = search.run();
			if (log)
				log->finalReport( (result != NULL), search);
		}
		else {
			gqrtl::DFS<R> search(csp, log);
			result = search.run();
			if (log)
				log->finalReport( (result != NULL), search);
		}


		delete log;

		if (result == NULL) {
			std::cout << input->name << ": 0";
//		if (swVerbose) {
//		    consistency->printStatistics();
//		}
			std::cout << std::endl;
			ret = 0;
		}
		else { // if (!consistencyFindAllConsistentSubCSPs) {
//	    if (swDisplayConsistentNetworks) {
			std::cout << input->name << ": 1";
//		if (swVerbose) {
//			consistency->printStatistics();
//		}
			std::cout << std::endl;
			if (showSolution) {
				std::cout << csp.getSize()-1 << " " << csp.name << std::endl;
				for (size_t i = 0; i < csp.getSize(); ++i)
					for (size_t j = i+1; j < csp.getSize(); ++j) {
						const R& rel = result->getConstraint(i,j);
						if (rel != calculus->getUniversalRelation()) {
							std::cout << i << " " << j << " ";
							std::cout << calculus->getCalculus().relationToString(rel.getRelation());
							std::cout << std::endl;
						}
					}
				std::cout << ".\n";
			}
			delete result;
			ret = 1;
		}
		delete input;
	}
	i.close();


	return ret;
}

template<class R>
bool SubcommandConsistency::runCoreTemplate<R>::ground(const Calculus &c) {
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
	if (verbose)
		groundTime.postLog("", 1, "calculi");

	return true;
}

template<class R>
SubcommandConsistency::runCoreTemplate<R>::~runCoreTemplate() {
	if (!calculus) return;
	delete calculus;
	R::clean_up();
}
