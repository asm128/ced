#include <algorithm>

#ifndef CED_COLOR_H_23904872398475
#define CED_COLOR_H_23904872398475

namespace ced
{
	template<typename _tValue>
	_tValue							clamp	(_tValue value, _tValue min, _tValue max)					{ return ::std::min(max, ::std::max(value, min)); };

#pragma pack(push, 1)
	struct SColor {
		uint8_t							b;
		uint8_t							g;
		uint8_t							r;
		uint8_t							a;

		SColor							operator/			(double scalar)	const					{ return {(uint8_t)::ced::clamp(b / scalar, 0.0, 255.0), (uint8_t)::ced::clamp(g / scalar, 0.0, 255.0), (uint8_t)::ced::clamp(r / scalar, 0.0, 255.0), a}; }
		SColor							operator*			(double scalar)	const noexcept			{ return {(uint8_t)::ced::clamp(b * scalar, 0.0, 255.0), (uint8_t)::ced::clamp(g * scalar, 0.0, 255.0), (uint8_t)::ced::clamp(r * scalar, 0.0, 255.0), a}; }
		SColor&							operator/=			(double scalar)							{ r = (uint8_t)::ced::clamp(r / scalar, 0.0, 255.0); g = (uint8_t)::ced::clamp(g / scalar, 0.0, 255.0); b = (uint8_t)::ced::clamp(b / scalar, 0.0, 255.0); return *this; }
		SColor&							operator*=			(double scalar)	noexcept				{ r = (uint8_t)::ced::clamp(r * scalar, 0.0, 255.0); g = (uint8_t)::ced::clamp(g * scalar, 0.0, 255.0); b = (uint8_t)::ced::clamp(b * scalar, 0.0, 255.0); return *this; }

		SColor							operator+			(const SColor& other)	const			{ return {(uint8_t)::ced::clamp(b + other.b, 0, 255), (uint8_t)::ced::clamp(g + other.g, 0, 255), (uint8_t)::ced::clamp(r + other.r, 0, 255), a}; }
		SColor							operator-			(const SColor& other)	const noexcept	{ return {(uint8_t)::ced::clamp(b - other.b, 0, 255), (uint8_t)::ced::clamp(g - other.g, 0, 255), (uint8_t)::ced::clamp(r - other.r, 0, 255), a}; }
		SColor&							operator+=			(const SColor& other)					{ r = (uint8_t)::ced::clamp(r + other.r, 0, 255); g = (uint8_t)::ced::clamp(g + other.g, 0, 255); b =	(uint8_t)::ced::clamp(b + other.b, 0, 255); return *this; }
		SColor&							operator-=			(const SColor& other)	noexcept		{ r = (uint8_t)::ced::clamp(r - other.r, 0, 255); g = (uint8_t)::ced::clamp(g - other.g, 0, 255); b =	(uint8_t)::ced::clamp(b - other.b, 0, 255); return *this; }
	};
#pragma pack(pop)
} // namespace

#endif // CED_COLOR_H_23904872398475
