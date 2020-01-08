#include "ced_draw.h"
#include <algorithm>

int								ced::setPixel			(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SCoord position, ::ced::SColor color)	{
	if( (position.x >= 0 && position.x < targetSize.x)
	 && (position.y >= 0 && position.y < targetSize.y)
	)
		pixels[position.y * targetSize.x + position.x]	= color;
	return 0;
}

int								ced::drawRectangle		(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SRectangle rectangle, ::ced::SColor color)	{
	for(int32_t y = 0; y < (int32_t)rectangle.Size.y; ++y)
	for(int32_t x = 0; x < (int32_t)rectangle.Size.x; ++x)
		::ced::setPixel(pixels, targetSize, {rectangle.Position.x + x, rectangle.Position.y + y}, color);
	return 0;
}

int								ced::drawCircle			(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SCircle circle, ::ced::SColor color)	{
	for(int32_t y = (int32_t)-circle.Radius; y < (int32_t)circle.Radius; ++y)
	for(int32_t x = (int32_t)-circle.Radius; x < (int32_t)circle.Radius; ++x) {
		::ced::SCoord						position			= {x, y};
		if(position.Length() <= circle.Radius)
			::ced::setPixel(pixels, targetSize, {circle.Position.x + x, circle.Position.y + y}, color);
	}
	return 0;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine       	(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::SLine line, ::ced::SColor color)	{
	int32_t								dx						= (int32_t)abs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-abs(line.B.y - line.A.y );
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			setPixel(pixels, targetSize, {line.A.x, line.A.y}, color);
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= sy;
			setPixel(pixels, targetSize, {line.A.x, line.A.y}, color);
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
int												orient2d					(const ::ced::SCoord& a, const ::ced::SCoord& b, const ::ced::SCoord& c)		{ return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x); }

template <typename _tValue>	_tValue 			max3						(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
template <typename _tValue>	_tValue 			min3						(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

int						ced::drawTriangle			(::ced::SColor * pixels, ::ced::SCoord targetSize, ::ced::STriangle triangle, ::ced::SColor color){
	// Compute triangle bounding box
	int32_t						minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t						minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int32_t						maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t						maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX					= ::std::max(minX, 0);
	minY					= ::std::max(minY, 0);
	maxX					= ::std::min(maxX, targetSize.x	- 1);
	maxY					= ::std::min(maxY, targetSize.y	- 1);

	// Rasterize
	::ced::SCoord p;
	for (p.y = minY; p.y <= maxY; p.y++) {
		for (p.x = minX; p.x <= maxX; p.x++) {
			// Determine barycentric coordinates
			int							w0						= ::orient2d(triangle.B, triangle.C, p);
			int							w1						= ::orient2d(triangle.C, triangle.A, p);
			int							w2						= ::orient2d(triangle.A, triangle.B, p);
			// If p is on or inside all edges, render pixel.
			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				::ced::setPixel(pixels, targetSize, p, color);
		}
	}
	return 0;
}
