#include "ced_math.h"
#include "ced_view.h"

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace ced
{
	struct SColor {
		uint8_t							r;
		uint8_t							g;
		uint8_t							b;
		uint8_t							a;
	};
	int								setPixel			(::ced::view_grid<::ced::SColor> pixels, ::ced::SCoord2		<int32_t>	position	, ::ced::SColor color);
	int								drawRectangle		(::ced::view_grid<::ced::SColor> pixels, ::ced::SRectangle	<int32_t>	rectangle	, ::ced::SColor color);
	int								drawCircle			(::ced::view_grid<::ced::SColor> pixels, ::ced::SCircle		<int32_t>	circle		, ::ced::SColor color);
	int								drawLine			(::ced::view_grid<::ced::SColor> pixels, ::ced::SLine		<int32_t>	line		, ::ced::SColor color);
	int								drawTriangle		(::ced::view_grid<::ced::SColor> pixels, ::ced::STriangle	<int32_t>	triangle	, ::ced::SColor color);
} // namespace

#endif // CED_DRAW_H_29837429837
