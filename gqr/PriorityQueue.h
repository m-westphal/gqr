// -*- C++ -*-

// Copyright (C) 2010 Isgandar Valizada
// Copyright (C) 2010,2012 Matthias Westphal
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

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <cassert>
#include <utility>
#include <vector>

#include "gqrtl/HashMap.h"

/**
 * A simple priority queue (heap based) with decrease-key functionality
 */

class PriorityQueue {
 private:
    std::vector < std::pair < size_t, size_t > > m_queue;
    gqrtl::HashMap<size_t> m_lookup;

 public:
    inline void clear() {
        m_queue.clear();
        m_lookup.clear();
    }
    PriorityQueue() {  /* NOCODE */  }

    inline size_t getSize() const {
        return m_queue.size();
    }

    inline bool empty() const {
        return m_queue.empty();
    }

    void popMin();

    inline const std::pair < size_t, size_t > &peekMin() const {
        assert(!m_queue.empty());
        return m_queue[0];
    }

    inline void insert(const size_t index, const size_t key) {
        std::pair<size_t*, bool> it = m_lookup.insert(index);

        size_t i;
        if (it.second) {
            if ((m_queue.size() + 1) == m_queue.capacity())
                m_queue.reserve(m_queue.capacity() * 2);

            i = m_queue.size();
            m_queue.push_back(std::make_pair(index, key));
        } else {
            i = *it.first;
            assert(m_queue[i].second >= key);   // keys can only decrease!

            if (m_queue[i].second == key)
                return;

            m_queue[i].second = key;
        }

        for (; (i > 0) && (m_queue[(i - 1) / 2].second > key); i = (i - 1) / 2) {
            const size_t parent = (i - 1) / 2;

            m_lookup[m_queue[parent].first] = i;
            std::swap(m_queue[i], m_queue[parent]);
        }

        *it.first = i;
    }
};

#endif                          // PRIORITY_QUEUE_H
