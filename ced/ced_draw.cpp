#include "ced_draw.h"
#include <algorithm>

int								ced::setPixel			(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SCoord2<int32_t> position, ::ced::SColorBGRA color)	{
	if( (position.x >= 0 && position.x < (int32_t)pixels.metrics().x)
	 && (position.y >= 0 && position.y < (int32_t)pixels.metrics().y)
	)
		pixels[position.y][position.x]	= color;
	return 0;
}

int								ced::drawRectangle		(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SRectangle<int32_t> rectangle, ::ced::SColorBGRA color)	{
	for(int32_t y = 0; y < (int32_t)rectangle.Size.y; ++y)
	for(int32_t x = 0; x < (int32_t)rectangle.Size.x; ++x)
		::ced::setPixel(pixels, {rectangle.Position.x + x, rectangle.Position.y + y}, color);
	return 0;
}

int								ced::drawCircle			(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SCircle<int32_t> circle, ::ced::SColorBGRA color)	{
	for(int32_t y = (int32_t)-circle.Radius; y < (int32_t)circle.Radius; ++y)
	for(int32_t x = (int32_t)-circle.Radius; x < (int32_t)circle.Radius; ++x) {
		::ced::SCoord2<int32_t>						position			= {x, y};
		if(position.Length() <= circle.Radius)
			::ced::setPixel(pixels, {circle.Position.x + x, circle.Position.y + y}, color);
	}
	return 0;
}

// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine       	(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SLine2<int32_t> line, ::ced::SColorBGRA color)	{
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


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine       	(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SLine2<int32_t> line, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords)	{
	int32_t								dx						= (int32_t)abs(line.B.x - line.A.x);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								dy						= (int32_t)-abs(line.B.y - line.A.y );
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= dx + dy;  /* error value e_xy */
	pixelCoords.push_back({line.A.x, line.A.y});
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= dy) {
			err								+= dy; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			)
				pixelCoords.push_back({line.A.x, line.A.y});

		}
		if (e2 <= dx) { /* e_xy+e_y < 0 */
			err								+= dx;
			line.A.y						+= sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			)
				pixelCoords.push_back({line.A.x, line.A.y});
		}

	}
	return 0;
}


// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
int								ced::drawLine
	( ::ced::view_grid<::ced::SColorBGRA>			pixels
	, const ::ced::SLine3<float>					& lineFloat
	, ::ced::container<::ced::SCoord3<float>>		& pixelCoords
	, ::ced::view_grid<uint32_t>					depthBuffer
	) {
	::ced::SLine2<int32_t>				line					= {{(int32_t)lineFloat.A.x, (int32_t)lineFloat.A.y}, {(int32_t)lineFloat.B.x, (int32_t)lineFloat.B.y}};
	int32_t								xDiff					= (int32_t)abs(line.B.x - line.A.x);
	int32_t								yDiff					= (int32_t)-abs(line.B.y - line.A.y);
	int32_t								sx						= (int32_t)line.A.x < line.B.x ? 1 : -1;
	int32_t								sy						= (int32_t)line.A.y < line.B.y ? 1 : -1;
	int32_t								err						= xDiff + yDiff;  /* error value e_xy */

	bool								yAxis					= abs(yDiff) > xDiff;
	::ced::view<uint32_t>				depthBufferRow			= {};
	if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
	 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
	 && lineFloat.A.z >= 0 && lineFloat.A.z <= 1
	) {
		depthBufferRow					= depthBuffer[line.A.y];
		uint32_t							& depthCell				= depthBufferRow[line.A.x];
		uint32_t							intZ					= uint32_t(0xFFFFFFFFU * lineFloat.A.z);
		if( depthCell > intZ ) {
			depthCell						= intZ;
			//pixelCoords.push_back({(int32_t)line.A.x, (int32_t)line.A.y});
			pixelCoords.push_back(lineFloat.A);
		}
	}

	bool								outside				= true;
	const double						factorUnit			= 1.0 / (yAxis ? abs(yDiff) : xDiff);
	while (true) {   /* loop */
		if (line.A.x == line.B.x && line.A.y == line.B.y)
			break;
		int32_t								e2						= 2 * err;
		if (e2 >= yDiff) {
			err								+= yDiff; /* e_xy+e_x > 0 */
			line.A.x						+= sx;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			) {
				depthBufferRow				= depthBuffer[line.A.y];
				const double						factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const double						finalZ					= ::ced::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const uint32_t						intZ					= uint32_t(0xFFFFFFFFU * finalZ);
				uint32_t							& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell						= intZ;
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
			else
				outside							= true;
		}
		if (e2 <= xDiff) { /* e_xy+e_y < 0 */
			err								+= xDiff;
			line.A.y						+= sy;
			if( line.A.x >= 0 && line.A.x < (int32_t)pixels.metrics().x
			 && line.A.y >= 0 && line.A.y < (int32_t)pixels.metrics().y
			) {
				depthBufferRow					= depthBuffer[line.A.y];
				const double						factor					= 1.0 / (yAxis ? factorUnit * line.A.y : factorUnit * line.A.x);
				const double						finalZ					= ::ced::interpolate_linear(lineFloat.A.z, lineFloat.B.z, factor);// lineFloat.B.z * factor + (lineFloat.A.z * (1.0 - factor));
				if (finalZ <= 0 || finalZ >= 1)
					continue;
				const uint32_t						intZ					= uint32_t(0xFFFFFFFFU * finalZ);
				uint32_t							& depthCell				= depthBufferRow[line.A.x];
				if(depthCell <= intZ)
					continue;

				depthCell						= intZ;
				pixelCoords.push_back({(float)line.A.x, (float)line.A.y, (float)finalZ});
			}
			else
				outside							= true;
		}

	}
	return 0;
}

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
double									orient2d				(const ::ced::SLine2<int32_t>	& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
double									orient2d				(const ::ced::SLine3<int32_t>	& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
double									orient2d				(const ::ced::SLine3<float>		& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }

template <typename _tValue>	_tValue 	max3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
template <typename _tValue>	_tValue 	min3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

int								ced::drawTriangle		(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::STriangle2<int32_t> triangle, ::ced::SColorBGRA color){
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

int								ced::drawTriangle
	( const ::ced::SCoord2<uint32_t>					targetSize
	, const ::ced::STriangle3<float>					triangle
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>> & proportions
	, ::ced::view_grid<uint32_t>						depthBuffer
	)	{
	// Compute triangle bounding box
	int32_t								minX					= (int32_t)::min3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								minY					= (int32_t)::min3(triangle.A.y, triangle.B.y, triangle.C.y);
	int32_t								maxX					= (int32_t)::max3(triangle.A.x, triangle.B.x, triangle.C.x);
	int32_t								maxY					= (int32_t)::max3(triangle.A.y, triangle.B.y, triangle.C.y);

	// Clip against screen bounds
	minX							= ::std::max(minX, 0);
	minY							= ::std::max(minY, 0);
	maxX							= ::std::min(maxX, (int32_t)targetSize.x - 1);
	maxY							= ::std::min(maxY, (int32_t)targetSize.y - 1);

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
		if(proportionABC == 0)
			continue;
		double								proportionA				= w0 / proportionABC;
		double								proportionB				= w1 / proportionABC;
		double								proportionC				= 1.0 - (proportionA + proportionB);

		// Calculate interpolated depth
		double								finalZ					= triangle.A.z * proportionA;
		finalZ							+= triangle.B.z * proportionB;
		finalZ							+= triangle.C.z * proportionC;
		if(finalZ >= 1.0 || finalZ <= 0)
			continue;

		uint32_t							intZ					= uint32_t(0xFFFFFFFFU * finalZ);
		uint32_t							& currentDepth			= depthBuffer[p.y][p.x];
		if(currentDepth < intZ)
			continue;

		currentDepth					= intZ;
		pixelCoords.push_back(p);
		proportions.push_back({(float)proportionA, (float)proportionB, (float)proportionC});

	}
	return 0;
}

int													ced::drawQuadTriangle
	( ::ced::view_grid<::ced::SColorBGRA>				targetPixels
	, const ::ced::STriangle3	<float>					& triangleWorld
	, const ::ced::STriangle3	<float>					& triangleScreen
	, const ::ced::SCoord3		<float>					& normal
	, const ::ced::STriangle2	<float>					& triangleTexCoords
	, const ::ced::SCoord3		<float>					& lightVector
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<const ::ced::SColorBGRA>			textureImage
	, ::ced::container<::ced::SCoord3<float>>			& lightPoints
	, ::ced::container<::ced::SColorBGRA>				& lightColors
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	::ced::drawTriangle(targetPixels.metrics(), triangleScreen, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::ced::SCoord2<float>									imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	double														lightFactorDirectional	= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>										pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<float>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::ced::SCoord2<float>										texCoord				= ::ced::triangleWeight(vertexWeights, triangleTexCoords);
		::ced::SCoord3<float>										position				= ::ced::triangleWeight(vertexWeights, triangleWorld);
		::ced::SColorFloat											texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.Metrics.y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.Metrics.x];
		::ced::SColorFloat											fragmentColor			= {};
		static constexpr	const double							rangeLight				= 10.0 * 10;
		static constexpr	const double							rangeUnit				= 1.0 / rangeLight;
		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			::ced::SCoord3<float>										lightToPoint			= lightPoints[iLight] - position;
			::ced::SCoord3<float>										vectorToLight			= lightToPoint;
			double														lightFactor				= vectorToLight.Dot(normal);
			const double												distanceToLight			= lightToPoint.LengthSquared();
			if(distanceToLight > rangeLight || lightFactor <= 0)
				continue;
			double														invAttenuation			= ::std::max(0.0, 1.0 - (distanceToLight * rangeUnit));
			fragmentColor											+= ::ced::SColorFloat{texelColor * (lightColors[iLight] * invAttenuation)};
		}
		::ced::setPixel(targetPixels, pixelCoord, texelColor *.35 + fragmentColor);
		(void)lightVector;
	}
	return 0;
}

int													ced::drawQuadTriangle
	( ::ced::view_grid<::ced::SColorBGRA>				targetPixels
	, ::ced::STriangle3			<float>					triangle
	, ::ced::SCoord3			<float>					normal
	, const ::ced::STriangle2	<float>					& triangleTexCoords
	, const ::ced::SMatrix4<float>						& matrixTransform
	, const ::ced::SMatrix4<float>						& matrixTransformView
	, const ::ced::SCoord3<float>						& lightVector
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<const ::ced::SColorBGRA>			textureImage
	, ::ced::container<::ced::SCoord3<float>>			& lightPoints
	, ::ced::container<::ced::SColorBGRA>				& lightColors
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	::ced::STriangle3	<float>								triangleWorld		= triangle;
	::ced::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;
	::ced::transform(triangleWorld, matrixTransform);
	normal												= matrixTransform.TransformDirection(normal).Normalize();
	return ::ced::drawQuadTriangle(targetPixels, triangleWorld, triangle, normal, triangleTexCoords, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, depthBuffer);
}

int													ced::drawQuadTriangle
	( ::ced::view_grid<::ced::SColorBGRA>				targetPixels
	, const ::ced::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::ced::SMatrix4<float>						& matrixTransform
	, const ::ced::SMatrix4<float>						& matrixTransformView
	, const ::ced::SCoord3<float>						& lightVector
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<const ::ced::SColorBGRA>			textureImage
	, ::ced::container<::ced::SCoord3<float>>			& lightPoints
	, ::ced::container<::ced::SColorBGRA>				& lightColors
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	const ::ced::STriangle3	<float>							& triangle			= geometry.Triangles	[iTriangle];;
	const ::ced::STriangle2	<float>							& triangleTexCoords	= geometry.TextureCoords[iTriangle];
	const ::ced::SCoord3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
	return ::ced::drawQuadTriangle(targetPixels, triangle, normal, triangleTexCoords, matrixTransform, matrixTransformView, lightVector, pixelCoords, pixelVertexWeights, textureImage, lightPoints, lightColors, depthBuffer);
}

int													ced::drawQuadTriangle
	( ::ced::view_grid<::ced::SColorBGRA>				targetPixels
	, const ::ced::SGeometryQuads						& geometry
	, const int											iTriangle
	, const ::ced::SMatrix4<float>						& matrixTransform
	, const ::ced::SMatrix4<float>						& matrixTransformView
	, const ::ced::SCoord3<float>						& lightVector
	, const ::ced::SColorBGRA							color
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	::ced::STriangle3	<float>								triangle			= geometry.Triangles	[iTriangle];
	::ced::SCoord3		<float>								normal				= geometry.Normals		[iTriangle / 2];
	normal												= matrixTransform.TransformDirection(normal).Normalize();
	::ced::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	double													lightFactor			= normal.Dot(lightVector);

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>									pixelCoord			= pixelCoords[iPixelCoord];
		::ced::setPixel(targetPixels, pixelCoord, (color * 0.1) + color * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( ::ced::view_grid<::ced::SColorBGRA>				targetPixels
	, const ::ced::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::ced::SMatrix4<float>						& matrixTransform
	, const ::ced::SMatrix4<float>						& matrixTransformView
	, const ::ced::SCoord3<float>						& lightVector
	, const ::ced::SColorBGRA							color
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	::ced::STriangle3		<float>								triangle			= geometry.Triangles	[iTriangle];
	const ::ced::STriangle3	<float>								& triangleNormals	= geometry.Normals		[iTriangle];
	::ced::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::ced::SCoord2<int32_t>							pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<float>					& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::ced::SCoord3<float>									normal				= ::ced::triangleWeight(vertexWeights, triangleNormals);
		normal												= matrixTransform.TransformDirection(normal).Normalize();
		double													lightFactor			= normal.Dot(lightVector);
		::ced::setPixel(targetPixels, pixelCoord, color * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
	, const ::ced::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::ced::SMatrix4<float>						& matrixTransform
	, const ::ced::SMatrix4<float>						& matrixTransformView
	, const ::ced::SCoord3<float>						& lightVector
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<const ::ced::SColorBGRA>			textureImage
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	::ced::STriangle3		<float>								triangle			= geometry.Triangles		[iTriangle];
	const ::ced::STriangle3	<float>								& triangleNormals	= geometry.Normals			[iTriangle];
	const ::ced::STriangle2	<float>								& triangleTexCoords	= geometry.TextureCoords	[iTriangle];
	::ced::transform(triangle, matrixTransformView);
	if( triangle.A.z <= 0 || triangle.A.z >= 1
	 || triangle.B.z <= 0 || triangle.B.z >= 1
	 || triangle.C.z <= 0 || triangle.C.z >= 1
	)
		return 0;

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	const ::ced::SCoord2<float>									imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::ced::SCoord2<int32_t>								pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<float>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::ced::SCoord3<float>										normal				= ::ced::triangleWeight(vertexWeights, triangleNormals);
		normal													= matrixTransform.TransformDirection(normal).Normalize();

		double														lightFactor				= normal.Dot(lightVector);

		::ced::SCoord2<float>										texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord												+= triangleTexCoords.B * vertexWeights.B;
		texCoord												+= triangleTexCoords.C * vertexWeights.C;
		::ced::SColorBGRA											texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		::ced::setPixel(targetPixels, pixelCoord, (texelColor * .3) + texelColor * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
	, const ::ced::SGeometryTriangles					& geometry
	, const int											iTriangle
	, const ::ced::SMatrix4<float>						& matrixTransform
	, const ::ced::SMatrix4<float>						& matrixTransformView
	, const ::ced::SCoord3<float>						& lightVector
	, const ::ced::SColorFloat							& lightColor
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, ::ced::view_grid<const ::ced::SColorBGRA>			textureImage
	, ::ced::container<::ced::SLight3>					& lightPoints
	, ::ced::container<::ced::SColorBGRA>				& lightColors
	, ::ced::view_grid<uint32_t>						depthBuffer
	) {
	::ced::STriangle3		<float>								triangleWorld			= geometry.Triangles		[iTriangle];
	::ced::STriangle3		<float>								triangle				= triangleWorld;
	::ced::transform(triangle, matrixTransformView);
	if( triangle.A.z < 0 || triangle.A.z >= 1
	 || triangle.B.z < 0 || triangle.B.z >= 1
	 || triangle.C.z < 0 || triangle.C.z >= 1
	)
		return 0;

	::ced::transform(triangleWorld, matrixTransform);
	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	const ::ced::STriangle3	<float>								& triangleNormals		= geometry.Normals			[iTriangle];
	const ::ced::STriangle2	<float>								& triangleTexCoords		= geometry.TextureCoords	[iTriangle];
	const ::ced::SCoord2<float>									imageUnit				= {textureImage.metrics().x - 0.000001f, textureImage.metrics().y - 0.000001f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		const ::ced::SCoord2<int32_t>								pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<float>						& vertexWeights			= pixelVertexWeights[iPixelCoord];

		::ced::SCoord3<float>										normal					= ::ced::triangleWeight(vertexWeights, triangleNormals);
		normal													= matrixTransform.TransformDirection(normal).Normalize();
		double														lightFactorDirectional	= normal.Dot(lightVector);
		::ced::SCoord3<float>										position				= ::ced::triangleWeight(vertexWeights, triangleWorld);
		::ced::SCoord2<float>										texCoord				= ::ced::triangleWeight(vertexWeights, triangleTexCoords);
		::ced::SColorFloat											texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y) % textureImage.Metrics.y][(uint32_t)(texCoord.x * imageUnit.x) % textureImage.Metrics.x];
		::ced::SColorFloat											fragmentColor			= {};

		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			const ::ced::SLight3										& light					= lightPoints[iLight];

			::ced::SCoord3<float>										lightToPoint			= light.Position - position;
			::ced::SCoord3<float>										vectorToLight			= lightToPoint;
			vectorToLight.Normalize();
			double														lightFactorPoint		= vectorToLight.Dot(normal);
			if(lightToPoint.Length() > light.Range || lightFactorPoint <= 0)
				continue;
			double														invAttenuation			= ::std::max(0.0, 1.0 - (lightToPoint.Length() / light.Range));
			fragmentColor											+= ::ced::SColorFloat{texelColor * lightColors[iLight] * invAttenuation * .5};
		}
		 (void)lightColor;
		 (void)lightFactorDirectional ;

		::ced::setPixel(targetPixels, pixelCoord, (texelColor * .1) + texelColor * lightColor * lightFactorDirectional + fragmentColor);
	}
	return 0;
}

