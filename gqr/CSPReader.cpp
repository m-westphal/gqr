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

#include <cstdlib>
#include <fstream>
#include <iostream>

#include "CSPReader.h"
#include "Relation.h"
#include "Calculus.h"
#include "CSPSparse.h"

#ifndef NDEBUG
#include "utils/Logger.h"
#endif

CSPReader::CSPReader(std::ifstream* initInput, const Calculus& initCalculus) :
input (initInput), calculus (initCalculus) { }

CSPSparse* CSPReader::makeCSP() throw (CSPReaderException) {
#ifndef NDEBUG
	Logger loadTime("CSP load time", 0);
	loadTime.start();
#endif

	if (! input->is_open()) {
		std::cerr << "CSPReader: Error opening input stream" << std::endl << std::flush;
		return 0;
	}

	size_t size = 0;

	std::string generatorData;

	ReadStartOfCSP:
	if (! input->eof() ) {

		std::string line;
		std::getline(*input, line);

		const size_t start = line.find_first_not_of(' ');
		const size_t end = line.find_first_of(' ', start);
		if (start != std::string::npos) {
			size = strtol(line.substr(start, end - start).c_str(), NULL, 10) + 1;
		} else {
			if (input->eof()) {
				return NULL;	// If we have reached the end of the file,
						// we cannot produce a CSP any more, obviously.
			} else {
				goto ReadStartOfCSP; // hehe, goto ;-)
			}

			std::cerr << "CSPReader: Failed to read CSP size: '" << line;
			std::cerr << "'" << std::endl << std::flush;
			return NULL;
		}

		const size_t generatorDataStart = line.find_first_of('#');
		if (generatorDataStart != std::string::npos) {
			generatorData = line.substr(generatorDataStart);
		} else {
			generatorData = "";
		}

	}

	if (size == 0) {
		std::cerr << "CSPReader: CSP size is 0. Refusing to parse instance.\n";
		return NULL;
	}

	CSPSparse* csp = new CSPSparse(size, calculus, generatorData);
	// this object has to be deleted by the caller

	while (!input->eof()) {
		std::string line;
		std::getline(*input, line);

		// Comment and empty lines
		if (line.substr(0, 1) == "#")
			continue;
		if (line == "")
			continue;
		// A line starting with a dot marks the end of a CSP definition
		if (line.substr(0, 1) == ".")
			break;

		size_t firstNumberStart, firstNumberEnd,
		secondNumberStart, secondNumberEnd,
		leftParanthese, rightParanthese;

		/** @todo handle cases like 1 2(=) */
		if ( ((firstNumberStart = line.find_first_of("0123456789")) == std::string::npos)
			|| ((firstNumberEnd = line.find_first_of(' ', firstNumberStart)) == std::string::npos)
			|| ((secondNumberStart = line.find_first_of("0123456789", firstNumberEnd)) == std::string::npos)
			|| ((secondNumberEnd = line.find_first_of(' ', secondNumberStart)) == std::string::npos)
			|| ((leftParanthese = line.find_first_of('(', secondNumberEnd)) == std::string::npos)
			|| ((rightParanthese = line.find_first_of(')', leftParanthese)) == std::string::npos)
		) {
			std::cerr << "Failed to parse line: " << line << std::endl << std::flush;
			throw CSPReaderException();
		}

		const size_t x = strtol(line.substr(firstNumberStart, firstNumberEnd-firstNumberStart).c_str(), NULL, 10);
		const size_t y = strtol(line.substr(secondNumberStart, secondNumberEnd-secondNumberStart).c_str(), NULL, 10);
		const std::string label = line.substr(leftParanthese + 1, rightParanthese-leftParanthese-1);

	if (!(x >= 0 && x < size) || !(y >= 0 && y < size)) {
	    std::cerr << "Non-existent edge (" << x << "," << y << ") specified\n";
	    throw CSPReaderException();
	}

		const Relation rel = calculus.encodeRelation(label);
		csp->addConstraint(x, y, rel);
	} // while


#ifndef NDEBUG
	loadTime.end();
	loadTime.postLog("", 1, "CSPs");
#endif

	return csp;
}
