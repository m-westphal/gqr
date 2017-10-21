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

#ifndef CSP_READER_H
#define CSP_READER_H

#include <fstream>
#include <stdexcept>

class Relation;
class Calculus;

class CSPSparse;

/**
 * Exception class of the CSPReader.
 **/
class CSPReaderException : public std::runtime_error {
	public:
		CSPReaderException() : std::runtime_error("CSPReaderException") { }
};

/**
 * Class that reads in a CSP from a file specification
 */
class CSPReader {
	private:
		/** Pointer to the ifstream object for the CSP file */
		std::ifstream* input;
		/** Pointer to the Calculus object */
		const Calculus& calculus;

	public:
		/**
		* Create a new CSP object.
		*
		* @return pointer to the new CSP object
		*/
		CSPSparse* makeCSP() throw (CSPReaderException);

		/**
		* Constructor.
		*
		* @param initInput pointer to ifstream object for the CSP file
		* @param initCalculus pointer to the Calculus object
		*/
		CSPReader(std::ifstream* initInput, const Calculus& initCalculus);
};

#endif // CSP_READER_H
