#pragma once
#ifdef ACS_HC2D_EXPORTS
#define ACS_HC2D_API __declspec(dllexport)
#else
#define ACS_HC2D_API __declspec(dllimport)
#endif


#include "dimension.h"
#include "safety.h"

namespace acs {
	//HitChcker2d
	namespace hc2d {
		using f2d = acs::dimension::f2d;
		using u2d = acs::dimension::u2d;
		using i2d = acs::dimension::i2d;

		enum SHAPE_ID : uint16_t {
			SID_NONE = 0,
			SID_CIRCLE = 1,
		};

		struct SHAPE_HEADER {
			const SHAPE_ID id;
			void* ex;
			SHAPE_HEADER() :id(SID_NONE), ex(nullptr) {

			}
			SHAPE_HEADER(SHAPE_ID sid, void* _ex) :id(sid), ex(_ex) {

			}
		};

		using PSHAPE_HEADER = SHAPE_HEADER*;
		using PCSHAPE_HEADER = const SHAPE_HEADER*;

		struct SHAPE_CIRCLE {
			SHAPE_HEADER header;
			f2d center;
			float radius;
			SHAPE_CIRCLE() :header(SID_CIRCLE, nullptr), center({ 0,0 }), radius(0) {}
			SHAPE_CIRCLE(void* _ex, f2d _center, float _radius) : header(SID_CIRCLE, _ex), center(_center), radius(_radius) {

			}
		};

		//IShapeÇÕÅ@s1,s2,s3,s4......null
		struct HIT_RESULT {
			PCSHAPE_HEADER shape;
			float distance;//íÜêSÇ©ÇÁÇÃãóó£
		};

		struct HITCHECKER {
			int unuse = 0;
		};

		using HHITCHECKER = HITCHECKER*;

		ACS_HC2D_API int HCCClear(HHITCHECKER hhc)noexcept;
		ACS_HC2D_API int HCCAddShape(HHITCHECKER hhc, PCSHAPE_HEADER _shape)noexcept;
		ACS_HC2D_API int HCCAddShape(HHITCHECKER hhc, PCSHAPE_HEADER* _shapes, size_t cnt) noexcept;
		ACS_HC2D_API HIT_RESULT* HCCHitCheck(HHITCHECKER hhc, PCSHAPE_HEADER _shape)noexcept;

		ACS_HC2D_API int HCCCreate(HHITCHECKER* out, f2d _xrange) noexcept;
		ACS_HC2D_API int HCCDelete(HHITCHECKER hhc)noexcept;


		struct HHITCHECKER_detach {
			void operator() (HHITCHECKER&hhc)const {
				HCCDelete(hhc);
			}
		};
		using CHITCHECKER = acs::safety::safe_obj<HHITCHECKER, acs::safety::default_attach<HHITCHECKER>, HHITCHECKER_detach>;
	};
}



