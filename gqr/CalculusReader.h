// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
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

#ifndef CALCULUS_READER_H
#define CALCULUS_READER_H

#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "CalculusMaker.h"
#include "Relation.h"

/** Class for creating a calculus object from specification file */
class CalculusReader : public CalculusMaker {
	private:
		// stream handed to constructor
		std::ifstream* stream;

		// directory where .spec is residing
		const std::string dataDir;

		/** Name of the calculus, e.g. 'allen', 'rcc5', 'rcc8' */
		const std::string calculusName;

		/** String representing the identity relation in the calculus that is created by the object */
		std::string identity;

		typedef std::map<std::string, std::string> converseTableType;

		/** Base relations extracted from calculus */
		std::vector<std::string> baseRelations;

		/** Fill baseRelations */
		void extractBaseRelations(const converseTableType&);

		/** Translation from string to base relation index in baseRelations */
		std::map<std::string, size_t> baseRelationsIndex;

		/** Convert string based converse table to size_t based vector */
		std::vector<size_t> compactConverseTable(const converseTableType& ct) const;

		typedef std::map<std::pair<std::string, std::string>, std::set<std::string> > compositionTableType;

		/** Convert string based composition table to two dimensional vector */
		std::vector<std::vector<Relation> > compactCompositionTable(const compositionTableType& ct) const;

		/** Convert string based weights into vector of size_t */
		std::vector<size_t> compactWeights(const std::map<std::string, size_t>& weights) const;

		/**
		* Read base relation weights from a file
		*
		* @param file an ifstream connected with the weights file
		* @return a map holding a weight for each base relation name
		*/
		const std::map<std::string, size_t> readWeights(std::ifstream& file) const;

		/**
		* Read composition table from file.
		*
		* @param file an ifstream object for the calculus definition file
		* @return a map containing the basic composition table (string representations)
		*/
		const compositionTableType readCompositionTable(std::ifstream& file) const;

		/**
		* Read converse table from file
		*
		* @param file open ifstream object containing the converse table
		* @return the converse table,
		*/
		const converseTableType readConverseTable(std::ifstream& file);

		/**
		* Read calculus configuration (location of the converse and composition table files) from file
		*
		*
		* @param file open ifstream object containing the configuration file
		* @return a key-value map of strings
		*/
		const std::map<std::string, std::string> readConfigFile(std::ifstream& file);

	public:
		/**
		* Create a new Calculus object
		*
		* @return pointer to the Calculus object
		*/
		virtual Calculus* makeCalculus();

		/**
		* Constructor
		*
		* @param initCalculusName the name of the calculus
		* @param initDataDir the directories where the calculus data could be stored
		*/
		CalculusReader(const std::string& name, const std::string& dir, std::ifstream* stream);

		/** Destructor */
		virtual ~CalculusReader() {};
};

#endif // CALCULUS_READER_H
