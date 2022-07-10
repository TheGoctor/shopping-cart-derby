#pragma once

#include "memory/allocator.h"

#include <list>
#include <map>
#include <queue>
#include <set>
#include <vector>

namespace scd {
template <typename _value, heap_id _heap = heap_id::general>
using list = std::list<_value, scd::allocator<_value, _heap>>;

template <typename _key, typename _value, typename _sort = std::less<_key>,
          heap_id _heap = heap_id::general>
using map = std::map<_key, _value, _sort,
                     scd::allocator<std::pair<const _key, _value>, _heap>>;

template <typename _key, typename _value, typename _sort = std::less<_key>,
          heap_id _heap = heap_id::general>
using multimap
  = std::multimap<_key, _value, _sort,
                  scd::allocator<std::pair<const _key, _value>, _heap>>;

template <typename _value, heap_id _heap = heap_id::general>
using deque = std::deque<_value, scd::allocator<_value, _heap>>;

template <typename _value, heap_id _heap = heap_id::general>
using queue
  = std::queue<_value, scd::deque<_value, scd::allocator<_value, _heap>>>;

template <typename _value, typename _sort = std::less<_value>,
          heap_type _heap = heap_type::general>
using priority_queue = std::priority_queue<
  _value, scd::deque<_value, scd::allocator<_value, _heap>>, _sort>;

template <typename _key, typename _sort = std::less<_key>,
          heap_id _heap = heap_id::general>
using set = std::set<_key, _sort, scd::allocator<_key, _heap>>;

template <typename _value, heap_id _heap = heap_id::general>
using vector = std::vector<_value, scd::allocator<_value, _heap>>;
} // namespace scd
