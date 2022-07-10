/*******************************************************************************
//	File			:	Heap.h
//	Date			:	3/29/11
//	Mod. Date		:	7/26/11
//	Mod. Initials	:	MR
//	Author			:	Mac Reichelt
//	Purpose			:	Chunks of memory that can be
allocated and used as
//						dynamic memory
*******************************************************************************/

#pragma once

#include <boost/container/pmr/monotonic_buffer_resource.hpp>

namespace scd {
namespace pmr {
using memory_buffer_pool = boost::container::pmr::monotonic_buffer_resource;
} // namespace pmr
} // namespace scd
