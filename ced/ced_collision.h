#include "ced_math.h"

#ifndef CED_COLLISION_H_2983742983
#define CED_COLLISION_H_2983742983

namespace ced
{
	// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
	// returns t value of intersection and intersection point q
	int32_t				intersectRaySphere		(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, const ::ced::SSphere<float> & sphere, float &t, ::ced::SCoord3<float> &q);
}

#endif // CED_COLLISION_H_2983742983
