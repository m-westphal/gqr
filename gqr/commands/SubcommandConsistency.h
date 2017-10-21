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


#ifndef SUBCOMMANDCONSISTENCY_H_
#define SUBCOMMANDCONSISTENCY_H_

#include "SubcommandAbstract.h"

class Relation;

namespace gqrtl {
	template<class R>
	class CalculusOperations;
}

class RestartsFramework;

class SubcommandConsistency: public SubcommandAbstract {
	private:
		arguments unusedArgs;
		bool parseUnusedArgs();
		bool setSplitter(const std::string& name);

		bool showSolution;
		bool returnState;

		RestartsFramework* restartOptions;

		Calculus* calculus;

		class runCore {
			protected:
				bool showSolution;
				bool verbose;
				RestartsFramework& restartOptions;
			public:
				runCore(const bool s, const bool v, RestartsFramework& o) : showSolution(s), verbose(v), restartOptions(o) {}
				virtual ~runCore() {}
				virtual int execute(const std::string&) = 0;
				virtual bool ground(const Calculus& c) = 0;
		};

		template<class R>
		class runCoreTemplate : public runCore {
			private:
				gqrtl::CalculusOperations<R>* calculus;
			public:
				runCoreTemplate(const bool a, const bool b, RestartsFramework& o) : runCore(a,b, o), calculus(NULL) {};
				virtual ~runCoreTemplate();
				virtual int execute(const std::string&);
				virtual bool ground(const Calculus& c);
		};

		bool applyConsistency(const std::vector<std::string>& filenames) const;
	public:
		SubcommandConsistency(const std::vector<std::string>&);
		virtual ~SubcommandConsistency();

		int run();

		static std::string helpString;
};

#endif /* SUBCOMMANDCONSISTENCY_H_ */
