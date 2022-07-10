#pragma once

#include "memory/allocator.h"

#include <list>
#include <map>
#include <queue>
#include <set>
#include <vector>

namespace scd {
template <typename _value>
using list = std::list<_value, scd::allocator<_value>>;

template <typename _key, typename _value, typename _sort = std::less<_key>>
using map = std::map<_key, _value, _sort,
                     scd::allocator<std::pair<const _key, _value>>>;

template <typename _value>
using deque = std::deque<_value, scd::allocator<_value>>;

template <typename _value>
using queue = std::queue<_value, scd::deque>;

template <typename _key, typename _sort = std::less<_key>>
using set = std::set<_key, _sort, scd::allocator<_key>>;

template <typename _value>
using vector = std::vector<_value, scd::allocator<_value>>;
} // namespace scd
