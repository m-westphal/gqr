// -*- C++ -*-

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

#ifndef HASH_MAP
#define HASH_MAP

#include <vector>
#include <utility>
#include <limits>
#include <cassert>

#include <cstddef>

namespace gqrtl {

#define HM_INVALID_KEY std::numeric_limits<size_t>::max()
#define HM_MAX_RATIO 3
#define HM_INITIAL_SIZE 10007

template<class P>
class HashMap {
	private:
		typedef std::pair<size_t, P> key_value;
		std::vector<key_value> store;
		size_t elements;

		void increase_store() {
			std::vector<key_value> store2(store.size()*2+1, std::make_pair(HM_INVALID_KEY, P()));

			for (size_t i = 0; i < store.size(); ++i) {
				size_t j = store[i].first % store2.size();
				while(store2[j].first != HM_INVALID_KEY)
					j = (j+1) % store2.size();
				store2[j] = store[i];
			}
			std::swap(store, store2);
		}

		inline size_t find(const size_t& key) const {
			size_t idx = key % store.size();
			while(store[idx].first != key && store[idx].first != HM_INVALID_KEY)
				idx = (idx+1) % store.size();
			return idx;
		}

	public:
		HashMap() { clear(); }

		void clear() {
			elements = 0;
			store.clear();
			store.resize(HM_INITIAL_SIZE, std::make_pair(HM_INVALID_KEY, P()));
		}
		// find key, or insert key with invalid value
		inline std::pair<P*, bool> insert(const size_t& key) {
			key_value& pos = store[find(key)];
			if (pos.first == HM_INVALID_KEY) {
				++elements;
				pos = std::make_pair(key, P());
				if (store.size() <= HM_MAX_RATIO*elements) {
					increase_store();
					key_value& new_pos = store[find(key)];
					return std::make_pair(&(new_pos.second), true);
				}
				return std::make_pair(&(pos.second), true);
			}
			assert(pos.first == key);
			return std::make_pair(&(pos.second), false);
		}
		void erase(const size_t& key) {
			size_t idx = find(key);
			assert(store[idx].first == key);

			store[idx].first = HM_INVALID_KEY;
			elements--;
			size_t p_idx = idx;
			while (true) {
				idx = (idx+1) % store.size();
				if (store[idx].first == HM_INVALID_KEY)
					return;

				const size_t h_idx = store[idx].first % store.size();
				if ( (p_idx<=idx) ? ((p_idx<h_idx)&&(h_idx<=idx)) : ((p_idx<h_idx)||(h_idx<=idx)) )
					continue;

				std::swap(store[p_idx], store[idx]);
				p_idx = idx;
			}
		}

		// get key (if it does not exists returns P&)
		// Careful: do NOT add elements this way; counter is not increased, key is not set
		inline P& operator[](const size_t& key) { return store[find(key)].second; }
		inline const P& operator[](const size_t& key) const { return store[find(key)].second; }

		inline bool exists(const size_t& key) const { return store[find(key)].first != HM_INVALID_KEY; }
};

}

#endif
