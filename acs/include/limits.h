#pragma once
#include <climits>
#include <cfloat>
namespace acs {

	template<typename T>
	[[deprecated("use std::numeric_limits")]] T MaxValue() {
	}
}