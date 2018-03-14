#pragma once
#include <cstddef>
#include <cstdlib>
#include <cstdint>

namespace acs {
	namespace hash {
		template <typename T>
		class FNV_1{
#if defined(_WIN64)
			static const size_t FNV_OFFSET_BASIS = 14695981039346656037U;
			static const size_t FNV_PRIME = 1099511628211LLU;
#else
			static const size_t FNV_OFFSET_BASIS = 2166136261U;
			static const size_t FNV_PRIME = 16777619U;
#endif
		public:
			size_t operator()(const T& _Keyval) const noexcept
			{
				const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&_Keyval);
				size_t hash = FNV_OFFSET_BASIS;
				for (size_t i = 0; i < sizeof(T); ++i) {
					hash ^= static_cast<size_t>(bytes[i]);
					hash *= FNV_PRIME;
				}

				return (hash);
			}
		};

		class FNV_1<const char*>{
#if defined(_WIN64)
			static const size_t FNV_OFFSET_BASIS = 14695981039346656037U;
			static const size_t FNV_PRIME = 1099511628211LLU;
#else
			static const size_t FNV_OFFSET_BASIS = 2166136261U;
			static const size_t FNV_PRIME = 16777619U;
#endif
		public:
			size_t operator()(const char* _Keyval) const noexcept
			{
				size_t hash = FNV_OFFSET_BASIS;
				while ((*_Keyval) != '/0')
				{
					const uint8_t* bytes = reinterpret_cast<const uint8_t*>(_Keyval);
					
					for (size_t i = 0; i < sizeof(char); ++i) {
						hash ^= static_cast<size_t>(bytes[i]);
						hash *= FNV_PRIME;
					}
				}
				return (hash);
			}
		};
	}
}