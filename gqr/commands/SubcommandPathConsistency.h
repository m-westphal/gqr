// -*- C++ -*-

// Copyright (C) 2011 Thomas Liebetraut
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


#ifndef SUBCOMMANDPATHCONSISTENCY_H_
#define SUBCOMMANDPATHCONSISTENCY_H_

#include "SubcommandAbstract.h"

class Relation;

namespace gqrtl {
	template<class R>
	class CalculusOperations;
}

class SubcommandPathConsistency: public SubcommandAbstract {
	private:
		std::vector<std::string> unusedArgs;
		bool parseUnusedArgs();

		bool negativeOnly;
		bool positiveOnly;

		bool showSolution;

		bool returnState;

		Calculus* calculus;

		class runCore {
			protected:
				bool positiveOnly;
				bool negativeOnly;
				bool showSolution;
			public:
				runCore(const bool p, const bool n, const bool s) : positiveOnly(p), negativeOnly(n), showSolution(s) {}
				virtual ~runCore() {}
				virtual int execute(const std::string&) = 0;
				virtual bool ground(const Calculus& c) = 0;
		};

		template<class R>
		class runCoreTemplate : public runCore {
			private:
				gqrtl::CalculusOperations<R>* calculus;
			public:
				runCoreTemplate(const bool a, const bool b, const bool s) : runCore(a,b,s), calculus(NULL) {};
				virtual ~runCoreTemplate();
				virtual int execute(const std::string&);
				virtual bool ground(const Calculus& c);
		};

		bool applyPathConsistency(const std::vector<std::string>& filenames) const;
	public:
		SubcommandPathConsistency(const std::vector<std::string>&);
		virtual ~SubcommandPathConsistency();

		int run();

		static std::string helpString;
};

#endif /* SUBCOMMANDPATHCONSISTENCY_H_ */
