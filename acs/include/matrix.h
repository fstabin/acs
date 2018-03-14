#pragma once
#include "vector.h"
#include <cmath>
namespace acs {
	namespace matrix {
		template<typename T = float>
		struct m4x4 {
			typedef vector::Vector4D<T> __t4d;
			typedef m4x4<T> __tm44;
			__t4d _0;
			__t4d _1;
			__t4d _2;
			__t4d _3;
			m4x4() noexcept:_0(), _1() , _2() , _3() {
			}
			m4x4(__t4d& __0, __t4d& __1, __t4d& __2, __t4d& __3)noexcept :_0(__0), _1(__1), _2(__2), _3(__3) {
			}
			m4x4(
				T __00, T __01, T __02, T __03,
				T __10, T __11, T __12, T __13,
				T __20, T __21, T __22, T __23,
				T __30, T __31, T __32, T __33) 
				:_0(__00, __01, __02, __03), _1(__10, __11, __12, __13), _2(__20, __21, __22, __23), _3(__30, __31, __32,__33) {
			}
			m4x4(__tm44& m) = default;
			m4x4& operator=(m4x4&) = default;		
			__t4d& operator[](size_t i) noexcept {
				__t4d* const p = reinterpret_cast<__t4d* const>(this);
				return p[i];
			}
			const __t4d& operator[](size_t i) const noexcept {
				const __t4d* const p = reinterpret_cast<const __t4d* const>(this);
				return p[i];
			}
		};

		template<typename T>
		inline void set_default(m4x4<T>& m44)noexcept {
			m44 = m4x4<T>(
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			);
		}

		template<typename T>
		inline void mul(m4x4<T>& out, const m4x4<T>& ml, const m4x4<T>& mr)noexcept {
			for (char i = 0; i < 4; i++)
			{
				for (char j = 0; j < 4; j++)
				{
					out[i][j] = 0;
					for (char k = 0; k < 4; k++)
					{
						out[i][j] += ml[i][k] * mr[k][j];
					}
				}
			}
		}

		template<typename T>
		inline void set_pos(m4x4<T>& m44, vector::Vector3D<T> pos)noexcept {
			m44 = m4x4<T>(
				1, 0, 0, pos.x,
				0, 1, 0, pos.y,
				0, 0, 1, pos.z,
				0, 0, 0, 1
			);
		}
		template<typename T>
		inline void set_pos_size(m4x4<T>& m44, vector::Vector3D<T> pos, vector::Vector3D<T> size)noexcept {
			m44 = m4x4<T>(
				size.x, 0, 0, pos.x,
				0, size.y, 0, pos.y,
				0, 0, size.z, pos.z,
				0, 0, 0, 1
			);
		}
	
		template<typename T>
		inline void set_pos_size_rotationxyz(m4x4<T>& m44, vector::Vector3D<T> pos, vector::Vector3D<T> size, vector::Vector3D<T> rotation) noexcept {
			constexpr const float pi2 = 3.1415926535897f / 2.f;
			int f;
			T sina, sinb, sinc;
			T cosa, cosb, cosc;
			sina = std::sin(rotation.z);	
			f = static_cast<int>(rotation.z / pi2);
			f &= 0x03;
			cosa = (((f ^ (f >> 1)) & 0x01)?  -1 : 1) * std::sqrt(1.f - sina * sina);

			sinb = std::sin(rotation.y);
			f = static_cast<int>(rotation.y / pi2);
			f &= 0x03;
			cosb = (((f ^ (f >> 1)) & 0x01) ? -1 : 1) * std::sqrt(1.f - sinb * sinb);

			sinc = std::sin(rotation.x);
			f = static_cast<int>(rotation.x / pi2);
			f &= 0x03;
			cosc = (((f ^ (f >> 1)) & 0x01) ? -1 : 1) * std::sqrt(1.f - sinc * sinc);

			T m[9] = { cosa * cosb,				cosb *sina,									-sinb,
				sinb *sinc * cosa - cosc * sina,	sinc * sinb * sina + cosc * cosa,	sinc * cosb,
				sina *sinc + cosc *sinb *cosa ,	-sinc * cosa + cosc * sinb * sina,	cosc * cosb
			};
			m44 = m4x4<T>(
				size.x * m[0], size.y* m[3], size.z* m[6], pos.x,
				size.x* m[1], size.y* m[4], size.z* m[7], pos.y,
				size.x* m[2], size.y* m[5], size.z* m[8], pos.z,
				0, 0, 0, 1
				);
		}

		template<typename T>
		inline void set_camera_direction(m4x4<T>& m44, vector::Vector3D<T> pos, vector::Vector3D<T> direction, vector::Vector3D<T> up) noexcept {
			direction /= vector::length_of(direction);
			vector::Vector3D<T> X = vector::cross(up, direction);
			X /= vector::length_of(X);
			vector::Vector3D<T> Y = vector::cross(direction, X);

			m44 = m4x4<T>(
				X.x, X.y, X.z, -dot(pos, X),
				Y.x, Y.y, Y.z, -dot(pos, Y),
				direction.x, direction.y, direction.z, -dot(pos, direction),
				0, 0, 0, 1
			);
		}
		template<typename T>
		inline void set_camera_focus(m4x4<T>& m44, vector::Vector3D<T> pos, vector::Vector3D<T> focus, vector::Vector3D<T> top) noexcept {
			vector::Vector3D<T> Z = focus - pos;
			Z /= vector::length_of(Z);
			vector::Vector3D<T> X = vector::cross(top, Z);
			X /= vector::length_of(X);
			vector::Vector3D<T> Y = vector::cross(Z, X);
			
			m44 = m4x4<T>(
				X.x, X.y, X.z, -dot(pos, X),
				Y.x, Y.y, Y.z, -dot(pos, Y),
				Z.x, Z.y, Z.z, -dot(pos, Z),
				0, 0, 0, 1
				);
		}

		template<typename T>
		inline void set_orthographicLH(m4x4<T>& m44, vector::Vector2D<T> size, vector::Vector2D<T> limZ)noexcept {
			float range =  limZ.y - limZ.x;
			m44 = m4x4<T>(
				2.f / size.x,0,0,0,
				0, 2.f / size.y,0,0,
				0,0, 1.f / range, -limZ.x / range,
				0, 0, 0, 1
				);
		}
		template<typename T>
		inline void set_orthographicRH(m4x4<T>& m44, vector::Vector2D<T> size, vector::Vector2D<T> limZ) noexcept {
			float range = limZ.y - limZ.x;
			m44 = m4x4<T>(
				2.f / size.x, 0, 0, 0,
				0, 2.f / size.y, 0, 0,
				0, 0, -1.f / range, limZ.x / range,
				0, 0, 0, 1
				);
		}
		template<typename T>
		inline void set_perspectiveLH(m4x4<T>& m44, vector::Vector2D<T> nearsize, vector::Vector2D<T> limZ)noexcept {
			float range = limZ.y - limZ.x;
			m44 = m4x4<T>(
				2.f / nearsize.x, 0, 0, 0,
				0, 2.f / nearsize.y, 0, 0,
				0, 0, 1.f / range, -limZ.x / range,
				0, 0, 1.f / limZ.x, 0
				);
		}
		template<typename T>
		inline void set_perspectiveRH(m4x4<T>& m44, vector::Vector2D<T> nearsize, vector::Vector2D<T> limZ) noexcept {
			float range = limZ.y - limZ.x;
			m44 = m4x4<T>(
				2.f / nearsize.x, 0, 0, 0,
				0, 2.f / nearsize.y, 0, 0,
				0, 0, -1.f / range, limZ.x / range,
				0, 0, -1.f / limZ.x, 0
				);
		}
	}
}