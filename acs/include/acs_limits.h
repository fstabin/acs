#pragma once
#include <climits>
#include <cfloat>
namespace acs {

	template<typename T>
	T MaxValue() {
		static_assert("not defined max value!!");
	}

	template<> constexpr 
		char MaxValue<char>() {
		return CHAR_MAX;
	}

	template<> constexpr
		short MaxValue<short>() {
		return SHRT_MAX;
	}

	template<> constexpr
		int MaxValue<int>() {
		return INT_MAX;
	}

	template<> constexpr
		long MaxValue<long>() {
		return LONG_MAX;
	}

	template<> constexpr
		long long MaxValue<long long>() {
		return LLONG_MAX;
	}

	template<> constexpr
		float MaxValue<float>() {
		return FLT_MAX;
	}

	template<> constexpr
		double MaxValue<double>() {
		return DBL_MAX;
	}

	template<> constexpr
		long double MaxValue<long double>() {
		return LDBL_MAX;
	}

	template<> constexpr
		unsigned char MaxValue<unsigned char>() {
		return UCHAR_MAX;
	}

	template<> constexpr
		unsigned short MaxValue<unsigned short>() {
		return USHRT_MAX;
	}

	template<> constexpr
		unsigned int MaxValue<unsigned int>() {
		return UINT_MAX;
	}

	template<> constexpr
		unsigned long MaxValue<unsigned long>() {
		return ULONG_MAX;
	}

	template<> constexpr
		unsigned long long MaxValue<unsigned long long>() {
		return ULLONG_MAX;
	}

	template<typename T>
	T MinValue() {
		static_assert("not defined min value!!");
	}

	template<> constexpr
		char MinValue<char>() {
		return CHAR_MIN;
	}

	template<> constexpr
		short MinValue<short>() {
		return SHRT_MIN;
	}

	template<> constexpr
		int MinValue<int>() {
		return INT_MIN;
	}

	template<> constexpr
		long MinValue<long>() {
		return LONG_MIN;
	}

	template<> constexpr
		long long MinValue<long long>() {
		return LLONG_MIN;
	}

	template<> constexpr
		float MinValue<float>() {
		return FLT_MIN;
	}

	template<> constexpr
		double MinValue<double>() {
		return DBL_MIN;
	}

	template<> constexpr
		long double MinValue<long double>() {
		return LDBL_MIN;
	}
}