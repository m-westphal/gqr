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

#ifndef SUBCOMMAND_ABSTRACT_H
#define SUBCOMMAND_ABSTRACT_H

#include <vector>
#include <string>

class Calculus;

// Abstract class that defines an interface for all subcommands
class SubcommandAbstract {
	protected:
		typedef std::vector<std::string> arguments;
		// (Initial) command line arguments (always without argv[0]
		// (program name) and argv[1] (subcommand name))
		const arguments commandLine;

		// Invoked program name (argv[0])
		const std::string programName;

		// Name of the subcommand on the command line, i.e. argv[1]
		// (if it is not given this string is empty)
		const std::string requestedSubcommandName;

		// Set of possible data directories (Calculi definitions etc.)
		std::vector<std::string> dataDirs;

		// Verbose setting (int instead of bool to support different
		// levels of verbosity)
		unsigned int verbose;

		// Extract general options and remove them from arguments list
		void parseArguments(arguments&);

		// data directory where current calculi information was found
		std::string usedDataDir;

		// Set calculus based on args, trim args to unused strings
		// sets usedDataDir appropriately
		// @return pointer to Calculus (may be NULL)
		Calculus* readCalculus(arguments& args);

	private:
		static std::string getProgramNameFromArgv(const arguments&);
		static std::string getSubcommandNameFromArgv(const arguments&);
		static arguments stripFirstArgument(const arguments&);

	public:
		// Setup a subcommand
		SubcommandAbstract(const arguments& a);

		// run the subcommand
		virtual int run() = 0;

		virtual ~SubcommandAbstract() { }
};

#endif
