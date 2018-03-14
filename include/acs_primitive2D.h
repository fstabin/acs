#pragma once
#include "acs_def.h"
#include "acs_vector.h"
namespace acs{
	namespace geometory {
		namespace primitive {
				using acs::vector::f2d;

				typedef acs::vector::f2d Point2D;//ç¿ïW

				typedef acs::vector::f2d Vector2D;//å¸Ç´

				struct Line2D {
					Point2D pos;
					Vector2D vec;
					Line2D(const Point2D& position, const Vector2D& vector):pos(position), vec(vector) {}
					~Line2D() {};
				};

				typedef Line2D Segment2D;

				struct Circle2D {
					Point2D pos;
					float r;
					Circle2D(const Point2D &position, float radius) : pos(position), r(radius){	}
				};

				struct Cupsule {
					Line2D l;
					float r;
					Cupsule(const Line2D& line, float radius) :l(line), r(radius) {}
				};

				struct Triangle {
					Point2D p[3];
				};
		}
	}
}