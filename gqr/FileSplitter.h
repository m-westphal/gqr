// -*- C++ -*-

// Copyright (C) 2006, 2008 Zeno Gantner
// Copyright (C) 2010 Isgandar Valizada
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

#ifndef FILE_SPLITTER_H
#define FILE_SPLITTER_H

#include <vector>
#include <utility>

#include "Splitter.h"

class Calculus;

/** Splitter class that represents (and splits) relations according to a cover
set read from a file. This is mostly used for tractable relations.  */
class FileSplitter : public Splitter {
	private:
		/**
		* Set of split relations (e.g., tractable relations).
		*/

		std::vector<Relation> splitRelations;

		void orderSplitRelations(const Calculus&);


		// For optimizing getFirstSplit, we presort splitRelations
		std::vector<std::vector<Relation*> > presortedSplitRelationsLookup;

		void presortSplitRelations(const Calculus&);

		/**
		* Routine to read the split set from a file.
		*
		* @param input pointer to ifstream object for the split set file
		*/
		void readSplitSetFromFile(std::ifstream* input, const Calculus& calculus);

		public:
			/**
			* Constructor
			* @param initCalculus pointer to a Calculus object
			* @param coverSetFile input pointer to ifstream object for the cover set file
			* @param splitSetFile input pointer to ifstream object for the split set file
			*/
			FileSplitter(const Calculus& calculus, std::ifstream* splitSetFile);
			/** Destructor */
			virtual ~FileSplitter();

			virtual bool isSplit(const Relation& r) const;

			virtual Relation getFirstSplit(const Relation& r) const;
};

#endif // FILE_SPLITTER_H
