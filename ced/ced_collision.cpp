#include "ced_collision.h"

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
int32_t												ced::intersectRaySphere		(const ::ced::SCoord3<float> & p, const ::ced::SCoord3<float> & d, const ::ced::SSphere<float> & sphere, float &t, ::ced::SCoord3<float> &q) {
	const ::ced::SCoord3<float>								m						= p - sphere.Position;
	double													b						= m.Dot(d);
	double													c						= m.Dot(m) - sphere.Radius * sphere.Radius;

	if (c > 0.0f && b > 0.0f)	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
		return 0;
	double													discr					= b * b - c;

	if (discr < 0.0f)	// A negative discriminant corresponds to ray missing sphere
		return 0;

	t													= (float)(-b - sqrt(discr));	// Ray now found to intersect sphere, compute smallest t value of intersection
	if (t < 0.0f)	// If t is negative, ray started inside sphere so clamp t to zero
		t													= 0.0f;

	q													= p + d * t;
	return 1;
}
