// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
// Copyright (C) 2012 Matthias Westphal
//
// This file is part of the Generic Qualitative Reasoner GQR.
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#include <iostream> // TODO: Make silent
#include <cassert>

#include <sstream>
#include <algorithm>

#include "CalculusReader.h"

//#include <stdlib.h>

#include "Calculus.h"

#include "Stringtools.h"

CalculusReader::CalculusReader(const std::string& name, const std::string& dir, std::ifstream* s)
	: stream(s), dataDir(dir), calculusName(name) { }

// ignore empty lines and lines starting with #
static bool ignoreLine(const std::string& s) { return s.empty() || s.substr(0, 1) == std::string("#"); }

const std::map<std::string, size_t>
CalculusReader::readWeights(std::ifstream& input) const {
	std::map<std::string, size_t> weights;

	while (!input.eof() ) {
		std::string line;
		std::getline(input, line);
		line = Stringtools::trim(line);
		if (ignoreLine(line))
			continue;

		size_t pos = line.find_first_of(' ');
		if (pos != std::string::npos) {
			const std::string relation_str = line.substr(0, pos);
			const std::string weight_str = line.substr(pos+1);

			std::stringstream ss;
			ss << weight_str;
			size_t weight;
			ss >> weight;
			assert(weights.find(relation_str) == weights.end());
			weights[relation_str] = weight;
		} else {
			std::cerr << "Error while reading weight file. Could not parse '" << line << "'.";
			std::cerr << std::endl << std::flush;
		}
	}

	return weights;
}


// TODO: Improve string parsing/error robustness etc
const std::map<std::pair<std::string, std::string>, std::set<std::string> >
CalculusReader::readCompositionTable(std::ifstream& input) const {
	std::map<std::pair<std::string, std::string>, std::set<std::string> > compTable;

	while (!input.eof() ) {
		std::string line;
		std::getline(input, line);
		line = Stringtools::trim(line);
		if (ignoreLine(line))
			continue;

		const size_t compPos = line.find(":");
		const size_t isPos = line.find("::");
		if ((compPos != std::string::npos) && (isPos != std::string::npos) ) {
			const std::string r1 = Stringtools::trim(line.substr(0, compPos));
			const std::string r2 = Stringtools::trim(line.substr(compPos + 1, isPos-compPos-1));
			std::string comp = Stringtools::trim(line.substr(isPos + 2));
			comp = Stringtools::trim(comp.substr(1, comp.length()-2));

			const std::pair<std::string, std::string> p = std::make_pair(Stringtools::trim(r1), Stringtools::trim(r2));

			const std::vector<std::string> c = Stringtools::split(comp, ' ');

			std::set<std::string> result;
			for (size_t i = 0; i < c.size(); i++)
				result.insert(c[i]);

			assert(compTable.find(p) == compTable.end());
			compTable[p] = result;
		}
	}

	return compTable;
}

const std::map<std::string, std::string>
CalculusReader::readConverseTable(std::ifstream& input) {
	std::map<std::string, std::string> converse;

	while (!input.eof() ) {
		std::string line;
		std::getline(input, line);
		line = Stringtools::trim(line);
		if (ignoreLine(line))
			continue;

		size_t pos = line.find("::");
		if (pos != std::string::npos) {
			// part before ::
			const std::string relation = Stringtools::trim(line.substr(0, pos - 1));

			// part after ::
			const std::string conv = Stringtools::trim(line.substr(pos + 2, line.length() - pos - 1));

			assert(converse.find(relation) == converse.end());
			converse[relation] = conv;
		}
	}

	return converse;
}

const std::map<std::string, std::string>
CalculusReader::readConfigFile(std::ifstream& input) {
	std::map<std::string, std::string> config;

	while (!input.eof() ) {
		std::string line;
		std::getline(input, line);
		line = Stringtools::trim(line);
		if (ignoreLine(line))
			continue;

		size_t pos = line.find_first_of(' ');
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			std::string value = line.substr(pos + 1, line.length() - pos - 1);
			assert(config.find(key) == config.end());
			config[key] = value;
		}
	}

	return config;
}

void CalculusReader::extractBaseRelations(const converseTableType& ct) {
	std::set<std::string> set;

	for (converseTableType::const_iterator it = ct.begin(); it != ct.end(); it++) {
		set.insert(it->first);
		set.insert(it->second);
	}

	baseRelations.reserve(set.size());
	for (std::set<std::string>::const_iterator it = set.begin(); it != set.end(); it++)
		baseRelations.push_back(*it);

	// fix order on base relations for reproducibility
	std::sort(baseRelations.begin(), baseRelations.end());

	for (size_t i = 0; i < baseRelations.size(); i++) {
		assert(baseRelationsIndex.find(baseRelations[i]) == baseRelationsIndex.end());
		baseRelationsIndex[baseRelations[i]] = i;
	}
}

static bool openFile(std::ifstream& i, const std::string dir, const std::string& filename) {
	const std::string name = dir + "/" + filename;
	i.open(name.c_str());
	if (!i.is_open()) {
		std::cerr << "Error opening file " << name << std::endl << std::flush;
		return false;
	}

	return true;
}

static void printError(const std::string& s) {
	std::cerr << "Error while loading calculus: ";
	std::cerr << s << std::endl;
}

Calculus* CalculusReader::makeCalculus() {
	assert(stream->is_open());

	const std::map<std::string, std::string> config = readConfigFile(*stream);

	std::ifstream input;

	// get identity from configuration map
	// (hell, why do we need so many lines for that task!?!)
	std::map<std::string, std::string>::const_iterator it = config.find("identity");
	if ( it == config.end() ) {
		printError("No identity relation defined.");
		return NULL;
	}
	identity = it->second;

	it = config.find("calculus_size");
	if ( it == config.end() ) {
		printError("'calculus_size' not defined.");
		return NULL;
	}
	size_t numberOfBaseRelations;
	{
	std::stringstream ss;
	ss << it->second;
	ss >> numberOfBaseRelations;
	}

	it = config.find("converse_file");
	if ( it == config.end() ) {
		printError("No converse relation table file defined.");
		return NULL;
	}

	std::string filename = it->second;
	if (!openFile(input, dataDir, filename))
		return NULL;
	const converseTableType converseTable = readConverseTable(input);
	input.close();

	if (numberOfBaseRelations != converseTable.size()) {
		printError("Indicated number of base relations in spec file does not match the size of the converse table.");
		std::cerr << "\t" << numberOfBaseRelations << " vs. " << converseTable.size();
		std::cerr << std::endl;
		return NULL;
	}

	extractBaseRelations(converseTable);
	if (numberOfBaseRelations != baseRelations.size()) {
		printError("Indicated number of base relations in spec file does not match the number of base relations found in the converse table.");
		std::cerr << "\t" << numberOfBaseRelations << " vs. " << (baseRelations.size()) << ")";
		std::cerr << std::endl;
		return NULL;
	}
	assert(baseRelationsIndex.find(identity) != baseRelationsIndex.end());

	it = config.find("comp_table_file");
	if ( it == config.end() ) {
		printError("No comp_table_file defined.");
		return NULL;
	}

	filename = it->second;

	if (!openFile(input, dataDir, filename))
		return NULL;
	const std::map< std::pair<std::string, std::string>, std::set<std::string> > compositionTable = readCompositionTable(input);
	input.close();

	if (compositionTable.size() != numberOfBaseRelations * numberOfBaseRelations) {
		printError("Size of the composition table does not match the squared number of base relations.");
		std::cerr << "\t" << compositionTable.size() << " vs. ";
		std::cerr << (numberOfBaseRelations * numberOfBaseRelations) << std::endl;
		return NULL;
	}

	const std::vector<size_t> ccvt = compactConverseTable(converseTable);
	const std::vector<std::vector<Relation> > ccmpt = compactCompositionTable(compositionTable);
	const size_t id = baseRelationsIndex[identity];

	it = config.find("weights");
	if ( it == config.end() ) {
		// TODO: should we auto compute proper weights?
		std::map<std::string, size_t> trivial_weights;

		for (size_t i = 0; i < baseRelations.size(); i++) {
			assert(trivial_weights.find(baseRelations[i]) == trivial_weights.end());
			trivial_weights[baseRelations[i]] = 1;
		}

		const std::vector<size_t> cw = compactWeights(trivial_weights);
		return new Calculus(calculusName, baseRelations, id, ccvt, ccmpt, cw);
	}
	else {
		filename = it->second;
		if (!openFile(input, dataDir, filename))
			return NULL;
		const std::map<std::string, size_t> weights = readWeights(input);
		input.close();

		if (weights.size() != numberOfBaseRelations) {
			printError("Size of weights table does not match the number of base relations.");
			std::cerr << "\t" << weights.size() << " vs. " << numberOfBaseRelations;
			std::cerr << std::endl;
			return NULL;
		}

		const std::vector<size_t> cw = compactWeights(weights);
		return new Calculus(calculusName, baseRelations, id, ccvt, ccmpt, cw);
	}
}

std::vector<size_t>
CalculusReader::compactConverseTable(const converseTableType& ct) const {
	std::vector<size_t> res;
	res.resize(baseRelations.size());

	for (size_t i = 0; i < baseRelations.size(); i++) {
		const std::string& in = baseRelations[i];
		assert(ct.find(in) != ct.end());
		const std::string& out = ct.find(in)->second;

		assert(baseRelationsIndex.find(out) != baseRelationsIndex.end());
		res[i] = baseRelationsIndex.find(out)->second;
	}

	return res;
}

std::vector<std::vector<Relation> >
CalculusReader::compactCompositionTable(const compositionTableType& ct) const {
	std::vector<std::vector<Relation> > table;
	table.resize(baseRelations.size());
	for (size_t i = 0; i < table.size(); i++)
		table[i].resize(baseRelations.size());

	for (size_t i = 0; i < baseRelations.size(); i++) {
		const std::string& name_i = baseRelations[i];
		for (size_t j = 0; j < baseRelations.size(); j++) {
			const std::string& name_j = baseRelations[j];
			compositionTableType::const_iterator it =
				ct.find(std::make_pair(name_i, name_j));
			assert(it != ct.end());
			const std::set<std::string>& strResult = it->second;

			Relation result;
			for (std::set<std::string>::const_iterator it3 = strResult.begin(); it3 != strResult.end(); it3++) {
				assert(baseRelationsIndex.find(*it3) != baseRelationsIndex.end());
				result.set(baseRelationsIndex.find(*it3)->second);
			}
			table[i][j] = result;
		}
	}

	return table;
}

std::vector<size_t>
CalculusReader::compactWeights(const std::map<std::string, size_t>& w) const {
	std::vector<size_t> res;
	res.resize(baseRelations.size());

	for (size_t i = 0; i < baseRelations.size(); i++) {
		const std::string& name = baseRelations[i];
		assert(w.find(name) != w.end());
		const size_t& weight = w.find(name)->second;
		res[i] = weight;
	}

	return res;
}
