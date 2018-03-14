#pragma once
#include "acs_def.h"
#define NOMINMAX
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <cmath>
namespace acs {
	namespace vector {
		template <typename T, unsigned int x>
		struct _xd {
			_xd<T, x - 1>cv;
			T v;

			T& operator[](size_t i) {
				T* tp = reinterpret_cast<T*>(this);
				return tp[i];
			}
		};
		template <typename T>
		struct _xd<T, 0> {
			_xd() {
				static_assert(false, "err: _xd x = 0");
			}
		};
		template <typename T>
		struct _xd<T, 1> {
			T v;
			T& operator[](size_t i) {
				T* tp = reinterpret_cast<T*>(this);
				return tp[i];
			}
		};

		template<typename T>
		struct Vector2D {
			T x, y;
			Vector2D() : x(), y() {};
			Vector2D(T _x, T _y) : x(_x), y(_y) {};
			Vector2D(const Vector2D& v) : x(v.x), y(v.y) {};
			inline Vector2D ACS_FCALL operator+(Vector2D v)const noexcept {
				return Vector2D(x + v.x, y + v.y);
			}
			inline Vector2D ACS_FCALL operator-(Vector2D v)const noexcept {
				return Vector2D(x - v.x, y - v.y);
			}
			inline Vector2D ACS_FCALL operator*(T v)const noexcept {
				return Vector2D(x * v, y * v);
			}
			inline Vector2D ACS_FCALL operator/(T v)const noexcept {
				return Vector2D(x / v, y / v);
			}
			
			inline Vector2D& ACS_FCALL operator+=(Vector2D v) noexcept {
				x += v.x;
				y += v.y;
				return *this;
			}
			inline Vector2D& ACS_FCALL operator-=(Vector2D v) noexcept {
				x -= v.x;
				y -= v.y;
				return *this;
			}
			inline Vector2D& ACS_FCALL operator*=(T v) noexcept {
				x *= v;
				y *= v;
				return *this;
			}
			inline Vector2D& ACS_FCALL operator/=(T v) noexcept {
				x /= v;
				y /= v;
				return *this;
			}

			inline Vector2D ACS_FCALL operator-() noexcept {
				return Vector2D(-x, -y);
			}

			inline Vector2D& ACS_FCALL operator=(Vector2D v) noexcept {
				x = v.x; y = v.y;
				return *this;
			}
			inline bool ACS_FCALL operator==(const Vector2D& v)const noexcept {
				return x == v.x && y == v.y;
			}

			inline T& ACS_FCALL operator[](size_t i) noexcept {
				auto p = reinterpret_cast<T*>(this);
				return p[i];
			}
			inline const T& ACS_FCALL operator[](size_t i)const noexcept {
				auto p = reinterpret_cast<const T*>(this);
				return p[i];
			}

			template <typename U>
			operator Vector2D<U>() noexcept {
				return Vector2D<U>(static_cast<U>(x), static_cast<U>(y));
			}
		};
		template<typename T>
		inline Vector2D<T>* t2_to_t2d(T v[2]) noexcept {
			return reinterpret_cast<Vector2D<T>*>(v);
		}
		template<typename T>
		struct Vector3D {
			T x, y, z;
			Vector3D() : x(), y(), z() {};
			Vector3D(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {};
			Vector3D(Vector2D<T> _xy, T _z) : x(_xy.x), y(_xy.y), z(_z) {};
			Vector3D(T _x, Vector2D<T> _yz) : x(_x), y(_yz.x), z(_yz.y) {};
			Vector3D(const Vector3D& v) : x(v.x), y(v.y), z(v.z) {};

			Vector2D<T> ACS_FCALL xy() {
				return Vector2D<T>(x, y);
			}
			Vector2D<T> ACS_FCALL yz() {
				return Vector2D<T>(y, z);
			}

			inline Vector3D ACS_FCALL operator+(Vector3D v)const noexcept {
				return Vector3D(x + v.x, y + v.y, z + v.z);
			}
			inline Vector3D ACS_FCALL operator-(Vector3D v)const noexcept {
				return Vector3D(x - v.x, y - v.y, z - v.z);
			}
			inline Vector3D ACS_FCALL operator*(T v)const noexcept {
				return Vector3D(x * v, y * v, z * v);
			}
			inline Vector3D ACS_FCALL operator/(T v)const noexcept {
				return Vector3D(x / v, y / v, z / v);
			}

			inline Vector3D& ACS_FCALL operator+=(Vector3D v) noexcept {
				x += v.x;
				y += v.y;
				z += v.z;
				return *this;
			}
			inline Vector3D& ACS_FCALL operator-=(Vector3D v) noexcept {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				return *this;
			}
			inline Vector3D& ACS_FCALL operator*=(T v) noexcept {
				x *= v;
				y *= v;
				z *= v;
				return *this;
			}
			inline Vector3D& ACS_FCALL operator/=(T v)noexcept {
				x /= v;
				y /= v;
				z /= v;
				return *this;
			}

			inline Vector3D ACS_FCALL operator-() noexcept {
				return Vector3D(-x, -y, -z);
			}

			inline Vector3D& ACS_FCALL operator=(Vector3D v) noexcept {
				x = v.x; y = v.y;z = v.z;
				return *this;
			}
			inline bool ACS_FCALL operator==(Vector3D v) noexcept {
				return x == v.x && y == v.y && z == v.z;
			}
			inline T& ACS_FCALL operator[](size_t i)noexcept {
				auto p = reinterpret_cast<T*>(this);
				return p[i];
			}
			inline const T& ACS_FCALL operator[](size_t i)const noexcept {
				auto p = reinterpret_cast<const T*>(this);
				return p[i];
			}

			template <typename U>
			operator Vector3D<U>() noexcept {
				return vector3d<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z));
			}
		};
		template<typename T>
		inline Vector3D<T>& t3_to_t3d(T v[3]) noexcept {
			return reinterpret_cast<Vector3D<T>&>(*v);
		}
		template<typename T>
		struct Vector4D {
			T x, y, z, w;
			Vector4D()noexcept : x(), y(), z(), w() {};
			Vector4D(T _x, T _y, T _z, T _w) noexcept: x(_x), y(_y), z(_z), w(_w) {};
			Vector4D(const Vector4D& v)noexcept : x(v.x), y(v.y), z(v.z), w(v.w) {};
			inline Vector4D ACS_FCALL operator+(Vector4D v)const noexcept {
				return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
			}
			inline Vector4D ACS_FCALL operator-(Vector4D v)const noexcept {
				return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
			}
			inline Vector4D ACS_FCALL operator*(T v)const noexcept {
				return Vector4D(x * v, y * v, z * v,w *v);
			}
			inline Vector4D ACS_FCALL operator/(T v)const noexcept {
				return Vector4D(x / v, y / v, z / v, w / v);
			}

			inline Vector4D& ACS_FCALL operator+=(Vector4D v) noexcept {
				x += v.x;
				y += v.y;
				z += v.z;
				w += v.w;
				return *this;
			}
			inline Vector4D& ACS_FCALL operator-=(Vector4D v)noexcept {
				x -= v.x;
				y -= v.y;
				z -= v.z;
				w -= v.w;
				return *this;
			}
			inline Vector4D& ACS_FCALL operator*=(T v)noexcept {
				x *= v;
				y *= v;
				z *= v;
				w *= v;
				return *this;
			}
			inline Vector4D& ACS_FCALL operator/=(T v) noexcept {
				x /= v;
				y /= v;
				z /= v;
				w /= v;
				return *this;
			}

			inline Vector4D ACS_FCALL operator-()noexcept {
				return Vector4D(-x, -y, -z, -w);
			}

			inline Vector4D& ACS_FCALL operator=(Vector4D v) noexcept {
				x = v.x; y = v.y;z = v.z;w = v.w;
				return *this;
			}
			inline bool ACS_FCALL operator==(Vector4D v) noexcept {
				return x == v.x && y == v.y && z == v.z && w == v.w;
			}
			inline T& ACS_FCALL operator[](size_t i) noexcept {
				auto p = reinterpret_cast<T*>(this);
				return p[i];
			}
			inline const T& ACS_FCALL operator[](size_t i)const noexcept {
				auto p = reinterpret_cast<const T*>(this);
				return p[i];
			}

			template <typename U>
			operator Vector4D<U>() noexcept {
				Vector4D<U>(static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w));
			}
		};
		template<typename T>
		inline Vector4D<T>* t4_to_t4d(T v[4]) noexcept {
			return reinterpret_cast<Vector4D<T>*>(v);
		}

		template<typename T>
		inline T sum_of(Vector2D<T> v) noexcept {
			return v.x + v.y;
		}
		template<typename T>
		inline T sum_of(Vector3D<T> v) noexcept {
			return v.x + v.y + v.z;
		}
		template<typename T>
		inline T sum_of(Vector4D<T> v) noexcept {
			return v.x + v.y + v.z + v.w;
		}

		template<typename T>
		inline T length_of(Vector2D<T> v) noexcept {
			return std::sqrt(v.x * v.x + v.y * v.y);
		}
		template<typename T>
		inline T length_of(Vector3D<T> v) noexcept {
			return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		}
		template<typename T>
		inline T length_of(Vector4D<T> v) noexcept {
			return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
		}

		template<typename T>
		inline Vector2D<T> normalize(Vector2D<T> v) noexcept {
			T l = std::sqrt(v.x * v.x + v.y * v.y);
			return (l != 0) ? v / l : Vector2D<T>(0, 0);
		}
		template<typename T>
		inline Vector3D<T> normalize(Vector3D<T> v)noexcept {
			T l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
			return (l != 0) ? v / l : Vector3D<T>(0,0,0);
		}
		template<typename T>
		inline Vector4D<T> normalize(Vector4D<T> v) noexcept {
			T l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
			return (l != 0) ? v / l : Vector4D<T>(0, 0, 0, 0);
		}

		template<typename T>
		inline T cross(Vector2D<T>& a, Vector2D<T>& b) noexcept {
			return (T)(a.x * b.y - a.y * b.x);
		}
		template<typename T>
		inline Vector3D<T> cross(Vector3D<T>& a, Vector3D<T>& b) noexcept {
			return Vector3D<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
		}

		template<typename T>
		inline T dot(Vector2D<T> a, Vector2D<T> b) noexcept {
			return a.x * b.x + a.y * b.y;
		}
		template<typename T>
		inline T dot(Vector3D<T> a, Vector3D<T> b) noexcept {
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		template<typename T>
		inline T max(Vector2D<T> a) noexcept {
			return (a.x < a.y) ? a.y : a.x;
		}
		template<typename T>
		inline T min(Vector2D<T> a) noexcept {
			return (a.x < a.y) ? a.x : a.y;
		}
		template<typename T>
		inline T max(Vector3D<T> a) noexcept {
			T mx = a[0];
			mx = max({ mx,a[1] });
			return max({ mx,a[2] });
		}
		template<typename T>
		inline T min(Vector3D<T> a) noexcept {
			T mn = a[0];
			mn = min({ mn,a[1] });
			return min({ mn,a[2] });
		}

		typedef Vector2D<acs::byte> by2d;
		typedef Vector2D<short> s2d;
		typedef Vector2D<int> i2d;
		typedef Vector2D<long> l2d;
		typedef Vector2D<long long> ll2d;
		typedef Vector2D<ushort> us2d;
		typedef Vector2D<uint> u2d;
		typedef Vector2D<ulong> ul2d;
		typedef Vector2D<ulonglong> ull2d;
		typedef Vector2D<float> f2d;
		typedef Vector2D<double> d2d;

		typedef Vector3D<acs::byte> by3d;
		typedef Vector3D<short> s3d;
		typedef Vector3D<int> i3d;
		typedef Vector3D<long> l3d;
		typedef Vector3D<long long> ll3d;
		typedef Vector3D<ushort> us3d;
		typedef Vector3D<uint> u3d;
		typedef Vector3D<ulong> ul3d;
		typedef Vector3D<ulonglong> ull3d;
		typedef Vector3D<float> f3d;
		typedef Vector3D<double> d3d;

		typedef Vector4D<acs::byte> by4d;
		typedef Vector4D<short> s4d;
		typedef Vector4D<int> i4d;
		typedef Vector4D<long> l4d;
		typedef Vector4D<long long> ll4d;
		typedef Vector4D<ushort> us4d;
		typedef Vector4D<uint> u4d;
		typedef Vector4D<ulong> ul4d;
		typedef Vector4D<ulonglong> ull4d;
		typedef Vector4D<float> f4d;
		typedef Vector4D<double> d4d;
	}
}