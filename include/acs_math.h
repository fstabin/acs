#pragma once
#include <cmath>

namespace acs {
	namespace math {
		inline double root(double x,double y){
			return std::pow(x, 1. / y);
		}

	}
}