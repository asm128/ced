#include "ced_math.h"

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
	int								setPixel			(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SCoord		position	, ::ced::SColor color);
	int								drawRectangle		(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SRectangle	rectangle	, ::ced::SColor color);
	int								drawCircle			(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SCircle		circle		, ::ced::SColor color);
	int								drawLine			(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SLine			line		, ::ced::SColor color);
	int								drawTriangle		(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::STriangle		triangle	, ::ced::SColor color);
} // namespace

#endif // CED_DRAW_H_29837429837
