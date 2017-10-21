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

#include <limits>
#include <algorithm>

#include "PriorityQueue.h"

/**
 * A simple priority queue (heap based) with decrease-key functionality
 */

void PriorityQueue::popMin() {
	assert(!empty());

	m_lookup.erase(m_queue[0].first);
	m_queue[0] = m_queue.back();
	m_queue.pop_back();
	if (empty())            // queue is now empty
		return;
	m_lookup[m_queue[0].first] = 0;

	size_t i = 0;
	for (size_t minChild = i;; i = minChild) {
		const size_t left = i * 2 + 1;
		const size_t right = i * 2 + 2;

		if (left < m_queue.size()
			&& m_queue[left].second < m_queue[minChild].second)
			minChild = left;

		if (right < m_queue.size()
			&& m_queue[right].second < m_queue[minChild].second)
			minChild = right;

		if (i == minChild)
			break;          // enforced heap property
		m_lookup[m_queue[i].first] = minChild;
		m_lookup[m_queue[minChild].first] = i;

		std::swap(m_queue[i], m_queue[minChild]);
	}
}
