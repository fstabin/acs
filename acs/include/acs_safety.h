#pragma once 
#include "acs_def.h"
#include <string.h>

//スコープ終了時の処理簡易化クラス等

namespace acs {
	namespace safety {
		//スマートオブジェクト
		template<typename T>
		struct default_attach {
			void operator() (T&)const noexcept {

			}
		};

		template<typename T>
		struct default_detach {
			void operator() (T&)const noexcept {

			}
		};

		template <typename T, typename _attach = default_attach<T>, typename _detach = default_detach<T>>
		class safe_obj {
			bool b = false;
			union
			{
				acs::byte data[sizeof(T)];
				T raw;
			};
		public:
			using type = T;
			using ref = T&;
			using point = T*;

			safe_obj() noexcept{
			}
			safe_obj(const T& in) noexcept{
				attach(in);
			}
			~safe_obj() noexcept{
				detach();
			}
			
			void attach(const T& in)noexcept {
				b = true;
				memcpy(data, &in, sizeof(T));
				_attach a;
				a(raw);
			}
			inline void detach()noexcept {
				if (b) {
					b = false;
					_detach d;
					d(raw);
					memset(data, 0, sizeof(sizeof(T)));
				}
			}

			inline ref get() noexcept {
				return raw;
			}
			inline operator ref() noexcept {
				return raw;
			}

			safe_obj(safe_obj& l) noexcept {
				*this = l;
			}
			inline safe_obj& operator= (safe_obj&  in) noexcept {
				for (size_t i = 0; i < sizeof(T); i++)
				{
					acs::byte b = data[i];
					data[i] = in.data[i];
					in.data[i] = b;
				}
				return *this;
			}
			inline safe_obj& operator= (const T&  in)noexcept {
				attach(in);
				return *this;
			}

			inline point operator-> () noexcept {
				return &raw;
			}
		};

		//処理ガード
		template<typename T>
		class cleanup_guard {
			T func;
		public:
			cleanup_guard(T _func)noexcept :func(_func){}
			~cleanup_guard() {
				func();
			}
		};

	}
}