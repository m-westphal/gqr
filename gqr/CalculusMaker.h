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

#ifndef CALCULUS_MAKER_H
#define CALCULUS_MAKER_H

class Calculus;

/**
 * Class that produces Calculus objects with the makeCalculus() function
 */
class CalculusMaker {
	public:
		/**
		* Create a new Calculus object
		*
		* @return pointer to the Calculus object
		*/
		virtual Calculus* makeCalculus() = 0;

		/** Destructor */
		virtual ~CalculusMaker() { }
};
#endif // CALCULUS_MAKER_H
