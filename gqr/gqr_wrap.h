// -*- C++ -*-

// Copyright (C) 2010 Matthias Westphal
// Copyright (C) 2010 Stefan Wölfl
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


#ifndef GQRLIB_H_
#define GQRLIB_H_

#include <fstream>
#include <string>

#include "Relation.h"
#include "Calculus.h"
#include "CalculusReader.h"
#include "FileSplitter.h"
#include "gqrtl/CSP.h"
#include "gqrtl/CalculusOperations.h"
#include "gqrtl/WeightedTripleIterator.h"
#include "gqrtl/DFS.h"

static bool initialized_relation_class_1 = false;
static bool initialized_relation_class_2 = false;
static bool initialized_relation_class_3 = false;
static bool initialized_relation_class_4 = false;

static std::string strip_relation_string(std::string s) {
    if (s.size() < 2)
      return "";

    if (*s.begin() != '(' || *s.rbegin() != ')')
      return "";

    s.erase(s.begin());
    s.erase(--s.end());
    return s;
}

// Calculus representation
class GQR_Calculus {
    public:
    Calculus* calculus;

    GQR_Calculus() : calculus(NULL) {}
    ~GQR_Calculus() { delete calculus; }

    // chain instance to a specific calculus (returns true on success)
    bool set_calculus(const std::string name, const std::string data_dir) {
        if (calculus != NULL)
	    return false;

        const std::string spec = data_dir + "/" + name + ".spec";
        std::ifstream stream;
        stream.open(spec.c_str());
        if(!stream.is_open())
            return false;

        CalculusReader r(name, data_dir, &stream);
        calculus = r.makeCalculus();
        stream.close();

        return true;
    }

    // returns the universal relations
    std::string get_base_relations() const {
        if (!calculus)
            return "";
	    return calculus->relationToString(calculus->getUniversalRelation());
	}
    // calculate composition
    std::string get_composition(std::string a_r, std::string b_r) const {
        if (!calculus)
            return "";

        const std::string a = strip_relation_string(a_r);
        const std::string b = strip_relation_string(b_r);

        return calculus->relationToString(
            calculus->getComposition(calculus->encodeRelation(a), calculus->encodeRelation(b))
        );
    }
    // calculate converse
    std::string get_converse(std::string r_raw) const {
        const std::string r = strip_relation_string(r_raw);
        return calculus->relationToString(
            calculus->getConverse(calculus->encodeRelation(r))
        );
    }
};

// CSP representation
class GQR_CSP {
    public:
    std::string name;
    gqrtl::CSP<Relation, Calculus>* csp;
    GQR_Calculus& calculus;


    GQR_CSP(int size, GQR_Calculus& c) : name(""), csp(new gqrtl::CSP<Relation, Calculus>(size, *c.calculus, name)), calculus(c) {}

    GQR_CSP(const GQR_CSP& i) : calculus(i.calculus) {
        csp = new gqrtl::CSP<Relation, Calculus> (*i.csp);
        name = i.name;
    }

    ~GQR_CSP() { delete csp; }

    std::string getName() const {
        return name;
    }
    void setName(std::string s) {
        name = s;
    }

    int getSize() const {
        return csp->getSize();
    }

    bool check_equals(const GQR_CSP& b) const {
        return (*csp == *b.csp);
    }

    bool check_refines(const GQR_CSP& b) const {
        for (size_t i = 0; i < csp->getSize(); i++)
            for (size_t j = i; j < csp->getSize(); j++) {
                if (csp->getConstraint(i,j) == b.csp->getConstraint(i,j))
                    return false;
                if (! csp->getConstraint(i,j).isSubsetOf(b.csp->getConstraint(i,j)))
                    return false;
            }
        return true;
    }

    // add a constraint
    bool add_constraint(const int i, const int j, const std::string relation) {
	    std::string rel = strip_relation_string(relation);
	    if (i < 0 || j < 0)
	        return false;
	    if (csp->getSize() <= (size_t) i || csp->getSize() <= (size_t) j)
            return false;
        csp->setConstraint((size_t) i,(size_t) j, csp->getConstraint((size_t) i,(size_t)j) & csp->getCalculus().encodeRelation(rel));
        return true;
    }
    // set a constraint (i.e. replace)
    bool set_constraint(const int i, const int j, const std::string relation) {
	    std::string rel = strip_relation_string(relation);
        if (i < 0 || j < 0)
	        return false;
	    if (csp->getSize() <= (size_t) i || csp->getSize() <= (size_t) j)
            return false;
        csp->setConstraint((size_t) i,(size_t) j, csp->getCalculus().encodeRelation(rel));
        return true;
    }
    // read constraint
    std::string get_constraint(const int i, const int j) const {
        if (i < 0 || j < 0)
	        return "";
        if (csp->getSize() <= (size_t) i || csp->getSize() <= (size_t) j)
		return "";
        return csp->getCalculus().relationToString(csp->getConstraint((size_t) i, (size_t) j));
    }
};


// GQR Functionality
class GQR_Solver {
    public:
    Calculus calculus; // TODO: do not copy calculus; change code to admit changing splitter

    // TODO update list of grounded calculi
    gqrtl::CalculusOperations<gqrtl::Relation8>* c1;
    gqrtl::CalculusOperations<gqrtl::Relation16>* c2;
    gqrtl::CalculusOperations<gqrtl::Relation32>* c3;
    gqrtl::CalculusOperations<gqrtl::RelationFixedBitset<size_t, 10> >* c4;

    GQR_Solver(GQR_Calculus& calc) : calculus(*calc.calculus),
    c1(NULL), c2(NULL), c3(NULL), c4(NULL) {}

    void ground_calculus() {
        if (c1 != NULL) return;
        if (c2 != NULL) return;
        if (c3 != NULL) return;
        if (c4 != NULL) return;

        if (calculus.getNumberOfBaseRelations() <= 8) {
            if (!initialized_relation_class_1) {
                initialized_relation_class_1 = true;
                gqrtl::Relation8::init();
            }
            c1 = new gqrtl::CalculusOperations<gqrtl::Relation8>(calculus);
        }
        else if (calculus.getNumberOfBaseRelations() <= 16) {
            if (!initialized_relation_class_2) {
                initialized_relation_class_2 = true;
                gqrtl::Relation16::init();
            }
            c2 = new gqrtl::CalculusOperations<gqrtl::Relation16>(calculus);
        }
        else if (calculus.getNumberOfBaseRelations() <= 32) {
            if (!initialized_relation_class_3) {
                initialized_relation_class_3 = true;
                gqrtl::Relation32::init();
            }
            c3 = new gqrtl::CalculusOperations<gqrtl::Relation32>(calculus);
        }
        else if (calculus.getNumberOfBaseRelations() <= 10*gqrtl::RelationFixedBitset<size_t, 10>::maxSize()) {
            if (!initialized_relation_class_4) {
                initialized_relation_class_4 = true;
                gqrtl::RelationFixedBitset<size_t, 10>::init();
            }
            c4 = new gqrtl::CalculusOperations<gqrtl::RelationFixedBitset<size_t, 10> >(calculus);
        }
    }
    ~GQR_Solver() { delete c1; delete c2; delete c3; delete c4; }

    // chain instance to a specific tractable subclass (returns true on success)
    bool set_tractable_subclass(const std::string algFilename) {
        if (calculus.getSplitter() != NULL)
            return false;

	std::ifstream* f1 = new std::ifstream();
	f1->open(algFilename.c_str());
	if ( (!f1) || (!f1->is_open()) ) { delete f1; return false; }

        Splitter* s = new FileSplitter(calculus, f1);
        f1->close();
	delete f1;

        if (!s)
            return false;

        calculus.setSplitter(s);

	return true;
    }

    // return false if revised CSP contains empty relations
    bool enforce_algebraic_closure(GQR_CSP& input) {
        ground_calculus();
        bool ret;

        if (c1 != NULL) {
            gqrtl::CSP<gqrtl::Relation8, gqrtl::CalculusOperations<gqrtl::Relation8> > c(*input.csp, *c1);
            gqrtl::WeightedTripleIterator<gqrtl::Relation8, gqrtl::CSP<gqrtl::Relation8, gqrtl::CalculusOperations<gqrtl::Relation8> > >
                prop;
            ret = prop.enforce(c).empty();

            for (size_t i = 0; i < c.getSize(); i++)
                for (size_t j = i; j < c.getSize(); j++)
                    input.csp->setConstraint(i,j, c.getConstraint(i,j).getRelation());
        }
        else if (c2 != NULL) {
            gqrtl::CSP<gqrtl::Relation16, gqrtl::CalculusOperations<gqrtl::Relation16> > c(*input.csp, *c2);
            gqrtl::WeightedTripleIterator<gqrtl::Relation16, gqrtl::CSP<gqrtl::Relation16, gqrtl::CalculusOperations<gqrtl::Relation16> > >
                prop;
            ret = prop.enforce(c).empty();

            for (size_t i = 0; i < c.getSize(); i++)
                for (size_t j = i; j < c.getSize(); j++)
                    input.csp->setConstraint(i,j, c.getConstraint(i,j).getRelation());
        }
        else if (c3 != NULL) {
            gqrtl::CSP<gqrtl::Relation32, gqrtl::CalculusOperations<gqrtl::Relation32> > c(*input.csp, *c3);
            gqrtl::WeightedTripleIterator<gqrtl::Relation32, gqrtl::CSP<gqrtl::Relation32, gqrtl::CalculusOperations<gqrtl::Relation32> > >
                prop;
            ret = prop.enforce(c).empty();

            for (size_t i = 0; i < c.getSize(); i++)
                for (size_t j = i; j < c.getSize(); j++)
                    input.csp->setConstraint(i,j, c.getConstraint(i,j).getRelation());
        }
        else if (c4 != NULL) {
            gqrtl::CSP<gqrtl::RelationFixedBitset<size_t, 10>, gqrtl::CalculusOperations<gqrtl::RelationFixedBitset<size_t, 10> > > c(*input.csp, *c4);
            gqrtl::WeightedTripleIterator<gqrtl::RelationFixedBitset<size_t, 10>, gqrtl::CSP<gqrtl::RelationFixedBitset<size_t, 10>, gqrtl::CalculusOperations<gqrtl::RelationFixedBitset<size_t, 10> > > >
                prop;
            ret = prop.enforce(c).empty();

            for (size_t i = 0; i < c.getSize(); i++)
                for (size_t j = i; j < c.getSize(); j++)
                    input.csp->setConstraint(i,j, c.getConstraint(i,j).getRelation());
        }
        else {
            gqrtl::WeightedTripleIterator<Relation, gqrtl::CSP<Relation, Calculus> > prop;

            ret = prop.enforce(*input.csp).empty();
        }

        return ret;
    }

    // calculate and return an a-closed sub-csp (atomic or tractable)
    GQR_CSP* get_scenario(const GQR_CSP& input) {
        ground_calculus();

        if (c1 != NULL) {
            gqrtl::CSP<gqrtl::Relation8, gqrtl::CalculusOperations<gqrtl::Relation8> > csp(*input.csp, *c1);
            gqrtl::DFS<gqrtl::Relation8> MySearch(csp, NULL);
            gqrtl::CSP<gqrtl::Relation8, gqrtl::CalculusOperations<gqrtl::Relation8> >* res = MySearch.run();
            if (!res)
                return NULL;

            GQR_CSP* result = new GQR_CSP(res->getSize(), input.calculus);

            for (size_t i = 0; i < res->getSize(); i++)
                for (size_t j = i; j < res->getSize(); j++)
                    result->csp->setConstraint(i, j, res->getConstraint(i, j).getRelation());
            delete res;
            return result;
        }
        if (c2 != NULL) {
            gqrtl::CSP<gqrtl::Relation16, gqrtl::CalculusOperations<gqrtl::Relation16> > csp(*input.csp, *c2);
            gqrtl::DFS<gqrtl::Relation16> MySearch(csp, NULL);
            gqrtl::CSP<gqrtl::Relation16, gqrtl::CalculusOperations<gqrtl::Relation16> >* res = MySearch.run();
            if (!res)
                return NULL;

            GQR_CSP* result = new GQR_CSP(res->getSize(), input.calculus);

            for (size_t i = 0; i < res->getSize(); i++)
                for (size_t j = i; j < res->getSize(); j++)
                    result->csp->setConstraint(i, j, res->getConstraint(i, j).getRelation());
            delete res;
            return result;
        }
        if (c3 != NULL) {
            gqrtl::CSP<gqrtl::Relation32, gqrtl::CalculusOperations<gqrtl::Relation32> > csp(*input.csp, *c3);
            gqrtl::DFS<gqrtl::Relation32> MySearch(csp, NULL);
            gqrtl::CSP<gqrtl::Relation32, gqrtl::CalculusOperations<gqrtl::Relation32> >* res = MySearch.run();
            if (!res)
                return NULL;

            GQR_CSP* result = new GQR_CSP(res->getSize(), input.calculus);

            for (size_t i = 0; i < res->getSize(); i++)
                for (size_t j = i; j < res->getSize(); j++)
                    result->csp->setConstraint(i, j, res->getConstraint(i, j).getRelation());
            delete res;
            return result;
        }
        if (c4 != NULL) {
            gqrtl::CSP<gqrtl::RelationFixedBitset<size_t, 10>, gqrtl::CalculusOperations<gqrtl::RelationFixedBitset<size_t, 10> > > csp(*input.csp, *c4);
            gqrtl::DFS<gqrtl::RelationFixedBitset<size_t, 10> > MySearch(csp, NULL);
            gqrtl::CSP<gqrtl::RelationFixedBitset<size_t, 10>, gqrtl::CalculusOperations<gqrtl::RelationFixedBitset<size_t, 10> > >* res = MySearch.run();
            if (!res)
                return NULL;

            GQR_CSP* result = new GQR_CSP(res->getSize(), input.calculus);

            for (size_t i = 0; i < res->getSize(); i++)
                for (size_t j = i; j < res->getSize(); j++)
                    result->csp->setConstraint(i, j, res->getConstraint(i, j).getRelation());
            delete res;
            return result;
        }

        gqrtl::CalculusOperations<Relation> c(calculus);
        gqrtl::CSP<Relation, gqrtl::CalculusOperations<Relation> > ground(*input.csp, c);
        gqrtl::DFS<Relation> MySearch(ground, NULL);

        gqrtl::CSP<Relation, gqrtl::CalculusOperations<Relation> >* res = MySearch.run();
        if (!res)
            return NULL;

        GQR_CSP* result = new GQR_CSP(res->getSize(), input.calculus);
            for (size_t i = 0; i < res->getSize(); i++)
                for (size_t j = i; j < res->getSize(); j++)
                    result->csp->setConstraint(i, j, res->getConstraint(i, j).getRelation());
        delete res;
        return result;
    }
};

#endif
