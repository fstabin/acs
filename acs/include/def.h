#pragma once
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <exception>

#define ACS_EXCEPTION_DEF(name, text) struct  name :std::exception{name():std::exception(text){}; name(const char* t):std::exception(t){};};
#define ACS_EXCEPTION_DEF1(name, text, args ) struct  name : args {name():args(text){}; name(const char* t):args(t){};};
#define ACS_NO_COPY(name) name& operator=(name&) = delete; name(name&) = delete;
#define ACS_NO_COPY_1(name) name& operator=(name&) = delete;

#if _WIN32 
#define ACS_SCALL __stdcall		//固定長関数　微早や
#define ACS_CCALL __cdecl		//固定長関数　微早や
#define ACS_TCALL __thiscall	//クラス関数　微早や
#define ACS_FCALL __fastcall	//完結している関数　早め
#define ACS_VCALL __vectorcall//完結している関数　もっと早め
#else 
#define ACS_SCALL
#define ACS_TCALL
#define ACS_FCALL
#define ACS_VCALL
#endif
#define _IACS_CHILD private:size_t refCnt = 1;public:virtual void ACS_TCALL Addref()throw() {refCnt++;}	virtual void ACS_TCALL Release()throw() {	refCnt--;if (refCnt == 0) {delete this;}}

namespace acs {
	typedef uint8_t byte;
	typedef unsigned long long longlong;
	typedef unsigned int uint;
	typedef unsigned short ushort;
	typedef unsigned long ulong;
	typedef unsigned long long ulonglong;

	class IACS {
	public:
		virtual void ACS_TCALL Addref()throw() = 0;
		virtual void ACS_TCALL Release()throw() = 0;
	};

	inline void SafeRelease(IACS*p) {
		if (p)p->Release();
	}

	template <typename T>
	class __s_iacs {
		__s_iacs() {
			static_assert(true, "not IACS_BASE class");
		}
	};
	template <>
	class __s_iacs<IACS>{
	};

	template <typename T>
	class SIACS{
		T* p;
	public:
		typedef SIACS<T> type;
		typedef T base;

		constexpr SIACS() noexcept :p(nullptr) {
		
		}
		constexpr SIACS(T*const& ip) noexcept :p(ip) {
			Addref();
		}
		constexpr SIACS(type const& ip) noexcept : p(ip.p) {
			Addref();
		}
		constexpr SIACS(type&& ip) noexcept : p(ip->p) {

		}
		inline ~SIACS() {
			if(p)p->Release();
		}

		inline void Attach(T* ip)noexcept {
			Relase();
			p = ip;
		}
		inline void Detach() noexcept {
			p = nullptr;
		}
		inline void Relase()noexcept {
			if(p)p->Release();
			p = nullptr;
		}
		inline void Addref()noexcept {
			if (p)p->Addref();
		}

		inline T* Ptr()const noexcept  {
			return p;
		}
		inline operator T*()const noexcept {
			return p;
		}
		inline T** operator&() noexcept {
#if _DEBUG
			if (p)assert("Warning!! p != unllptr");
#endif
			return &p;
		}
		inline SIACS& operator= (T*ip)noexcept {
			Relase();
			p = ip;
			Addref();
			return *this;
		}
		inline SIACS& operator= (const type&ip) noexcept {
			Relase();
			p = ip;
			Addref();
			return *this;
		}
		inline SIACS& operator= (type&&ip) noexcept {
			Relase();
			p = ip.p;
			return *this;
		}
		inline T* operator-> ()const noexcept {
			return p;
		}

		inline bool hasPtr()const noexcept {
			return p != nullptr;
		}
	};
}