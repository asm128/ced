#include "ced_math.h"
#include "ced_view.h"
#include <algorithm>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace ced
{
	template<typename _tValue>
	_tValue							clamp	(_tValue value, _tValue min, _tValue max)					{ return ::std::min(max, ::std::max(value, min)); };

	struct SColor {
		uint8_t							r;
		uint8_t							g;
		uint8_t							b;
		uint8_t							a;

		SColor							operator/			(double scalar)	const			{ return {(uint8_t)::ced::clamp(r / scalar, 0.0, 255.0), (uint8_t)::ced::clamp(g / scalar, 0.0, 255.0), (uint8_t)::ced::clamp(b / scalar, 0.0, 255.0), a}; }
		SColor							operator*			(double scalar)	const noexcept	{ return {(uint8_t)::ced::clamp(r * scalar, 0.0, 255.0), (uint8_t)::ced::clamp(g * scalar, 0.0, 255.0), (uint8_t)::ced::clamp(b * scalar, 0.0, 255.0), a}; }
		SColor&							operator/=			(double scalar)					{ r = (uint8_t)::ced::clamp(r / scalar, 0.0, 255.0); g = (uint8_t)::ced::clamp(g / scalar, 0.0, 255.0); b = (uint8_t)::ced::clamp(b / scalar, 0.0, 255.0); return *this; }
		SColor&							operator*=			(double scalar)	noexcept		{ r = (uint8_t)::ced::clamp(r * scalar, 0.0, 255.0); g = (uint8_t)::ced::clamp(g * scalar, 0.0, 255.0); b = (uint8_t)::ced::clamp(b * scalar, 0.0, 255.0); return *this; }

		SColor							operator+			(const SColor& other)	const			{ return {(uint8_t)::ced::clamp(r + other.r, 0, 255), (uint8_t)::ced::clamp(g + other.g, 0, 255),		(uint8_t)::ced::clamp(b + other.b, 0, 255), a}; }
		SColor							operator-			(const SColor& other)	const noexcept	{ return {(uint8_t)::ced::clamp(r - other.r, 0, 255), (uint8_t)::ced::clamp(g - other.g, 0, 255),		(uint8_t)::ced::clamp(b - other.b, 0, 255), a}; }
		SColor&							operator+=			(const SColor& other)					{ r = (uint8_t)::ced::clamp(r + other.r, 0, 255); g = (uint8_t)::ced::clamp(g + other.g, 0, 255); b =	(uint8_t)::ced::clamp(b + other.b, 0, 255); return *this; }
		SColor&							operator-=			(const SColor& other)	noexcept		{ r = (uint8_t)::ced::clamp(r - other.r, 0, 255); g = (uint8_t)::ced::clamp(g - other.g, 0, 255); b =	(uint8_t)::ced::clamp(b - other.b, 0, 255); return *this; }
	};
	int								setPixel			(::ced::view_grid<::ced::SColor> pixels, ::ced::SCoord2		<int32_t>	position	, ::ced::SColor color);
	int								drawRectangle		(::ced::view_grid<::ced::SColor> pixels, ::ced::SRectangle	<int32_t>	rectangle	, ::ced::SColor color);
	int								drawCircle			(::ced::view_grid<::ced::SColor> pixels, ::ced::SCircle		<int32_t>	circle		, ::ced::SColor color);
	int								drawLine			(::ced::view_grid<::ced::SColor> pixels, ::ced::SLine		<int32_t>	line		, ::ced::SColor color);
	int								drawTriangle		(::ced::view_grid<::ced::SColor> pixels, ::ced::STriangle	<int32_t>	triangle	, ::ced::SColor color);
	int								drawTriangle		(::ced::SCoord2<uint32_t> targetSize, ::ced::STriangle<int32_t> triangle, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords);
} // namespace

#endif // CED_DRAW_H_29837429837
