#include <cstdint>
#include <cmath>

#ifndef CED_MATH_H_2983492837498
#define CED_MATH_H_2983492837498

namespace ced
{
	struct SCoord {
		int32_t							x;
		int32_t							y;

		SCoord							operator+				(const SCoord & other)	const noexcept	{ return {x + other.x, y + other.y}; }
		SCoord							operator-				(const SCoord & other)	const noexcept	{ return {x - other.x, y - other.y}; }
		SCoord							operator/				(double scalar)			const			{ return {(int32_t)(x / scalar), (int32_t)(y / scalar)}; }
		SCoord							operator*				(double scalar)			const noexcept	{ return {(int32_t)(x * scalar), (int32_t)(y * scalar)}; }

		SCoord&							operator+=				(const SCoord & other)	noexcept	{ x += other.x; y += other.y; return *this; }
		SCoord&							operator-=				(const SCoord & other)	noexcept	{ x -= other.x; y -= other.y; return *this; }
		SCoord&							operator/=				(double scalar)						{ x = (int32_t)(x / scalar); y = (int32_t)(y / scalar); return *this; }
		SCoord&							operator*=				(double scalar)			noexcept	{ x = (int32_t)(x * scalar); y = (int32_t)(y * scalar); return *this; }

		double							Length					()						const noexcept	{
			int									lengthSquared			= x * x + y * y;
			if(0 == lengthSquared)
				return 0;
			return sqrt(lengthSquared);
		}
	};

	struct SRectangle {
		SCoord							Position	;
		SCoord							Size		;
	};

	struct SCircle {
		SCoord							Position	;
		double							Radius		;
	};

	struct SLine {
		SCoord							A;
		SCoord							B;
	};

	struct STriangle {
		SCoord							A;
		SCoord							B;
		SCoord							C;
	};
} // namespace

#endif // CED_MATH_H_2983492837498
