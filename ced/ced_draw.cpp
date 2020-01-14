#include "ced_draw.h"
#include <algorithm>

int								ced::setPixel			(::ced::view_grid<::ced::SColor> pixels, ::ced::SCoord2<int32_t> position, ::ced::SColor color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	)
		pixels[position.y][position.x]	= color;
	return 0;
}

int								ced::drawRectangle		(::ced::view_grid<::ced::SColor> pixels, ::ced::SRectangle<int32_t> rectangle, ::ced::SColor color)	{
	for(int32_t y = 0; y < (int32_t)rectangle.Size.y; ++y)
	for(int32_t x = 0; x < (int32_t)rectangle.Size.x; ++x)
		::ced::setPixel(pixels, {rectangle.Position.x + x, rectangle.Position.y + y}, color);
	return 0;
}

int								ced::drawCircle			(::ced::view_grid<::ced::SColor> pixels, ::ced::SCircle<int32_t> circle, ::ced::SColor color)	{
	for(int32_t y = (int32_t)-circle.Radius; y < (int32_t)circle.Radius; ++y)
	for(int32_t x = (int32_t)-circle.Radius; x < (int32_t)circle.Radius; ++x) {
		::ced::SCoord2<int32_t>						position			= {x, y};
		if(position.Length() <= circle.Radius)
			::ced::setPixel(pixels, {circle.Position.x + x, circle.Position.y + y}, color);
	}
	return 0;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine       	(::ced::view_grid<::ced::SColor> pixels, ::ced::SLine<int32_t> line, ::ced::SColor color)	{
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
			setPixel(pixels, {line.A.x, line.A.y}, color);
		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= sy;
			setPixel(pixels, {line.A.x, line.A.y}, color);
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
double									orient2d				(const ::ced::SLine<int32_t>& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x)*(point.y - segment.A.y) - (segment.B.y - segment.A.y)*(point.x - segment.A.x); }

template <typename _tValue>	_tValue 	max3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
template <typename _tValue>	_tValue 	min3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

int								ced::drawTriangle		(::ced::view_grid<::ced::SColor> pixels, ::ced::STriangle<int32_t> triangle, ::ced::SColor color){
	// Compute triangle bounding box
	int32_t								minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int32_t								maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, 0);
	minY							= ::std::max(minY, 0);
	maxX							= ::std::min(maxX, (int32_t)pixels.metrics().x	- 1);
	maxY							= ::std::min(maxY, (int32_t)pixels.metrics().y	- 1);

	// Rasterize
	::ced::SCoord2<int32_t>				p;
	for (p.y = minY; p.y <= maxY; p.y++) {
		for (p.x = minX; p.x <= maxX; p.x++) {
			// Determine barycentric coordinates
			double									w0						= ::orient2d({triangle.B, triangle.C}, p);
			double									w1						= ::orient2d({triangle.C, triangle.A}, p);
			double									w2						= ::orient2d({triangle.A, triangle.B}, p);
			// If p is on or inside all edges, render pixel.
			if (w0 < 0 || w1 < 0 || w2 < 0)
				continue;
			::ced::setPixel(pixels, p, color);
		}
	}
	return 0;
}


int								ced::drawTriangle		(::ced::SCoord2<uint32_t> targetSize, ::ced::STriangle<int32_t> triangle, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords, ::ced::container<::ced::STriangleWeights<double>> & proportions)	{
	// Compute triangle bounding box
	int32_t								minX					= ::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								minY					= ::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int32_t								maxX					= ::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								maxY					= ::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, 0);
	minY							= ::std::max(minY, 0);
	maxX							= ::std::min(maxX, (int32_t)targetSize.x	- 1);
	maxY							= ::std::min(maxY, (int32_t)targetSize.y	- 1);

	// Rasterize
	::ced::SCoord2<int32_t> p;
	for (p.y = minY; p.y <= maxY; ++p.y)
	for (p.x = minX; p.x <= maxX; ++p.x) {
		// Determine barycentric coordinates
		double								w0						= ::orient2d({triangle.B, triangle.C}, p);
		double								w1						= ::orient2d({triangle.C, triangle.A}, p);
		double								w2						= ::orient2d({triangle.A, triangle.B}, p);
		// If p is on or inside all edges, render pixel.
		if (w0 < 0 || w1 < 0 || w2 < 0)
			continue;

		double								proportionABC			= w0 + w1 + w2;
		double								proportionA				= w0 / proportionABC;
		double								proportionB				= w1 / proportionABC;
		double								proportionC				= 1.0 - (proportionA + proportionB);
		pixelCoords.push_back(p);
		proportions.push_back({proportionA, proportionB, proportionC});
	}
	return 0;
}
