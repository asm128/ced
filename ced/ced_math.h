#include <cstdint>
#include <cmath>

#ifndef CED_MATH_H_2983492837498
#define CED_MATH_H_2983492837498

namespace ced
{
	template<typename _tValue>
	struct SCoord2 {
		_tValue							x;
		_tValue							y;

		SCoord2<_tValue>				operator+				(const SCoord2<_tValue>& other)	const noexcept	{ return {x + other.x, y + other.y}; }
		SCoord2<_tValue>				operator-				(const SCoord2<_tValue>& other)	const noexcept	{ return {x - other.x, y - other.y}; }
		SCoord2<_tValue>				operator/				(double scalar)					const			{ return {(_tValue)(x / scalar), (_tValue)(y / scalar)}; }
		SCoord2<_tValue>				operator*				(double scalar)					const noexcept	{ return {(_tValue)(x * scalar), (_tValue)(y * scalar)}; }

		SCoord2<_tValue>&				operator+=				(const SCoord2<_tValue>& other)	noexcept		{ x += other.x; y += other.y; return *this; }
		SCoord2<_tValue>&				operator-=				(const SCoord2<_tValue>& other)	noexcept		{ x -= other.x; y -= other.y; return *this; }
		SCoord2<_tValue>&				operator/=				(double scalar)									{ x = (_tValue)(x / scalar); y = (_tValue)(y / scalar); return *this; }
		SCoord2<_tValue>&				operator*=				(double scalar)					noexcept		{ x = (_tValue)(x * scalar); y = (_tValue)(y * scalar); return *this; }

		template<typename _tOther>
		SCoord2<_tOther>				Cast					()								const			{ return {(_tOther)x, (_tOther)y}; }

		// https://matthew-brett.github.io/teaching/rotation_2d.html
		SCoord2<_tValue>				Rotated					(double theta)									{
			double								dsin					= ::std::sin(theta);
			double								dcos					= ::std::cos(theta);
			return {(int)(dcos * x - dsin * y), (int)(dsin * x + dcos * y)};
		}

		double							Length					()								const noexcept	{
			int									lengthSquared			= x * x + y * y;
			if(0 == lengthSquared)
				return 0;
			return sqrt(lengthSquared);
		}
	};

	template<typename _tValue>
	struct SCoord3 {
		_tValue							x;
		_tValue							y;
		_tValue							z;

		SCoord3<_tValue>				operator+				(const SCoord3<_tValue>& other)		const noexcept	{ return {x + other.x, y + other.y, z + other.z}; }
		SCoord3<_tValue>				operator-				(const SCoord3<_tValue>& other)		const noexcept	{ return {x - other.x, y - other.y, z - other.z}; }
		SCoord3<_tValue>				operator/				(double scalar)						const			{ return {(_tValue)(x / scalar), (_tValue)(y / scalar), (_tValue)(z / scalar)}; }
		SCoord3<_tValue>				operator*				(double scalar)						const noexcept	{ return {(_tValue)(x * scalar), (_tValue)(y * scalar), (_tValue)(z * scalar)}; }

		SCoord3<_tValue>&				operator+=				(const SCoord3<_tValue>& other)		noexcept		{ x += other.x; y += other.y; z += other.z; return *this; }
		SCoord3<_tValue>&				operator-=				(const SCoord3<_tValue>& other)		noexcept		{ x -= other.x; y -= other.y; z -= other.z; return *this; }
		SCoord3<_tValue>&				operator/=				(double scalar)										{ x = (_tValue)(x / scalar); y = (_tValue)(y / scalar); z = (_tValue)(z / scalar); return *this; }
		SCoord3<_tValue>&				operator*=				(double scalar)						noexcept		{ x = (_tValue)(x * scalar); y = (_tValue)(y * scalar); z = (_tValue)(z * scalar); return *this; }

		// https://matthew-brett.github.io/teaching/rotation_2d.html
		SCoord3<_tValue>				RotatedZ				(double theta)						const noexcept	{
			double								dsin					= ::std::sin(theta);
			double								dcos					= ::std::cos(theta);
			return {(int)(dcos * x - dsin * y), (int)(dsin * x + dcos * y), z};
		}

		double							Length					()									const noexcept	{
			int									lengthSquared			= x * x + y * y + z * z;
			if(0 == lengthSquared)
				return 0;
			return sqrt(lengthSquared);
		}
	};

	template<typename _tValue>
	struct SRectangle {
		SCoord2<_tValue>					Position	;
		SCoord2<_tValue>					Size		;
	};

	template<typename _tValue>
	struct SCircle {
		SCoord2<_tValue>					Position	;
		double							Radius		;
	};

	template<typename _tValue>
	struct SLine {
		SCoord2<_tValue>					A;
		SCoord2<_tValue>					B;
	};

	template<typename _tValue>
	struct STriangle {
		SCoord2<_tValue>					A;
		SCoord2<_tValue>					B;
		SCoord2<_tValue>					C;
	};

	template<typename _tValue>
	struct SRectangle3 {
		SCoord3<_tValue>				Position	;
		SCoord3<_tValue>				Size		;
	};

	template<typename _tValue>
	struct SCircle3 {
		SCoord3<_tValue>				Position	;
		double							Radius		;
	};

	template<typename _tValue>
	struct SLine3 {
		SCoord3<_tValue>				A;
		SCoord3<_tValue>				B;
	};

	template<typename _tValue>
	struct STriangle3 {
		SCoord3<_tValue>				A;
		SCoord3<_tValue>				B;
		SCoord3<_tValue>				C;
	};
} // namespace

#endif // CED_MATH_H_2983492837498
