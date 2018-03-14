#pragma once

#ifndef ACS_G_EPSILON
#define ACS_G_EPSILON 0.000001f // �덷
#endif

#define _ACS_G_EPSILON2 ACS_G_EPSILON*ACS_G_EPSILON // �덷

#include "def.h"
#include "primitive2D.h"
#include <algorithm>
namespace acs{
	namespace geometory {
		namespace collision{
			using acs::vector::f2d;


			//�Փ�
			bool Point2DandLine2D(const primitive::Point2D&pos ,const primitive::Line2D& line){
				//�@�������@�_�Ɛ��̋����P.png
				float d = vector::cross(pos - line.pos, f2d(line.vec));
				d *= d;
				return (_ACS_G_EPSILON2 >= (d / vector::dot(line.vec, line.vec)));
			}

			bool Point2DandSegment2D(const primitive::Point2D&pos, const primitive::Segment2D& seg) {
				//�@�������@�_�Ɛ��̋����P.png
				float d = vector::cross(pos - seg.pos, f2d(seg.vec));
				d *= d;
				f2d vs_half = seg.vec / 2;
				f2d m = seg.pos + vs_half;
				f2d mp = pos - m;
				//�����ɋ߂��������̒��_����̋����������Ȃ�
				return ((ACS_G_EPSILON >= (d / vector::dot(seg.vec, seg.vec))) && (vector::dot(mp, mp) <= vector::dot(vs_half, vs_half) + ACS_G_EPSILON) );
			}

			bool Circle2DandLine2D(const primitive::Circle2D&c, const primitive::Line2D& line) {
				//�@�������@�_�Ɛ��̋����P.png
				float d = vector::cross(c.pos - line.pos, f2d(line.vec));
				d *= d;
				return (_ACS_G_EPSILON2 + c.r * c.r >= (d / vector::dot(line.vec, line.vec)));
			}

			bool Circle2DandSegment2D(const primitive::Circle2D&c, const primitive::Segment2D& seg) {
				//�@�������@�~�Ɛ��̋����P.png
				f2d v_a = seg.pos - c.pos;
				f2d v_d = seg.vec;
				float l_a = acs::vector::dot(v_a, v_a);
				float l_d = acs::vector::dot(v_d, v_d);
				float f_ad = acs::vector::dot(v_a, v_d);
			
				if (l_a - (c.r* c.r) <= ACS_G_EPSILON)return true;
				if (l_d + f_ad  + l_a - (c.r* c.r) <= ACS_G_EPSILON)return true;

				float t = -f_ad / (2 * l_d);
				if (-ACS_G_EPSILON <= t && t  <= 1.f + ACS_G_EPSILON) {
					if (- (f_ad * f_ad / (4 * l_d)) + l_a - (c.r* c.r) <= ACS_G_EPSILON)return true;
				}
			}

			//�Փ� + ����
			bool MovingCircle2DandLine2D_time(const primitive::Circle2D&c, const primitive::Vector2D& vc,const primitive::Line2D& line, float& time) {
				//�@�������@�_�Ɛ��̋����P.png
				f2d v_lc = c.pos - line.pos;
				float hb = c.r * acs::vector::length_of(line.vec);
				float t1 = (- hb - acs::vector::dot(v_lc, line.vec)) / (acs::vector::dot(vc, line.vec));
				float t2 = (+ hb - acs::vector::dot(v_lc, line.vec)) / (acs::vector::dot(vc, line.vec));
				bool sign1 = std::signbit(t1), sign2 = std::signbit(t2);
				if ((sign1 && sign2) || (1.f < t1 && 1.f < t2))return false;//���Ԃ�0�ȉ� or ���Ԃ�1�ȏ�
				if (sign1 != sign2)time = 0;//���Ԃ�0���܂���
				else time = std::min(1.f, std::min(t1, t2));//����ȊO
				return true;
			}
		}
	}
}