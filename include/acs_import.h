#pragma once 
#include "acs_def.h"
#include <Windows.h>
namespace acs {
	namespace import {
		class module {
			HMODULE mod = NULL;
			ACS_NO_COPY(module);
		public:
			module()noexcept {
			}
			module(const wchar_t* modpass) noexcept {
				open(modpass);
			}
			~module() noexcept {
				close();
			}
			bool opened()noexcept { return mod != NULL; }

			bool open(const wchar_t* modpass)noexcept {
				close();
				mod = LoadLibraryW(modpass);
				return opened();
			}
			void close() noexcept {
				if(opened())FreeLibrary(mod);
			}

			void* get_func(const char* funcname)noexcept {
				return static_cast<void*>(GetProcAddress(
					mod,    // DLL モジュールのハンドル
					funcname   // 関数名
				));
			}
		};
	}
}