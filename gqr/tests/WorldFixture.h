// -*- C++ -*-
#ifndef WORLD_FIXTURE_H
#define WORLD_FIXTURE_H

#include <iostream>

#include "GlobalFixture.h"

class WorldFixture : public CxxTest::GlobalFixture {
public:
    bool setUpWorld() {
        std::cout << "<" << std::flush;
        return true;
    }
    
    bool tearDownWorld() {
        std::cout << ">" << std::flush;
        return true;
    }
    
    bool setUp() {
        //std::cout << "(" << std::flush;
        return true;
    }
    
    bool tearDown() {
        //std::cout << ")" << std::flush;
        return true;
    }
};

static WorldFixture worldFixture;

#endif // WORLD_FIXTURE_H
