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
int								ced::drawLine       	(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SLine<int32_t> line, ::ced::SColorBGRA color)	{
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
int								ced::drawLine       	(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::SLine<int32_t> line, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords)	{
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

//https://fgiesen.wordpress.com/2013/02/08/triangle-rasterization-in-practice/
double									orient2d				(const ::ced::SLine<int32_t>	& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
double									orient2d				(const ::ced::SLine3<int32_t>	& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }
double									orient2d				(const ::ced::SLine3<float>		& segment, const ::ced::SCoord2<int32_t>& point)		{ return (segment.B.x - segment.A.x) * (point.y - segment.A.y) - (segment.B.y - segment.A.y) * (point.x - segment.A.x); }

template <typename _tValue>	_tValue 	max3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::max(::std::max(a, b), c); }
template <typename _tValue>	_tValue 	min3					(_tValue & a, _tValue & b, _tValue & c)			{ return ::std::min(::std::min(a, b), c); }

int								ced::drawTriangle		(::ced::view_grid<::ced::SColorBGRA> pixels, ::ced::STriangle<int32_t> triangle, ::ced::SColorBGRA color){
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
	( ::ced::SCoord2<uint32_t>							targetSize
	, ::ced::STriangle3<float>							triangle
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<double>> & proportions
	, ::ced::container<uint32_t>						& depthBuffer
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

		uint32_t							intZ					= uint32_t((0xFFFFFFFFU) * (1.0 - finalZ));
		if(depthBuffer[p.y * targetSize.x + p.x] > intZ)
			continue;

		depthBuffer[p.y * targetSize.x + p.x] = intZ;
		pixelCoords.push_back(p);
		proportions.push_back({proportionA, proportionB, proportionC});

	}
	return 0;
}

int													ced::drawQuadTriangle
	( ::ced::view_grid<::ced::SColorBGRA>					targetPixels
	, ::ced::SGeometryQuads								& geometry
	, int												iTriangle
	, ::ced::SMatrix4<float>							& matrixTransform
	, ::ced::SMatrix4<float>							& matrixView
	, ::ced::SMatrix4<float>							& matrixViewport
	, ::ced::SCoord3<float>								& lightVector
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
	, ::ced::view_grid<::ced::SColorBGRA>					textureImage
	, ::ced::container<::ced::SCoord3<float>>			& lightPoints
	, ::ced::container<::ced::SColorBGRA>					& lightColors
	, ::ced::container<uint32_t>						& depthBuffer
	) {
	::ced::STriangle3	<float>								triangleWorld		= geometry.Triangles	[iTriangle];
	::ced::STriangle3	<float>								triangle			= geometry.Triangles	[iTriangle];
	::ced::SCoord3		<float>								normal				= geometry.Normals		[iTriangle / 2];

	triangleWorld.A										= matrixTransform.Transform(triangleWorld.A);
	triangleWorld.B										= matrixTransform.Transform(triangleWorld.B);
	triangleWorld.C										= matrixTransform.Transform(triangleWorld.C);

	triangle											= triangleWorld;
	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1) return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1) return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1) return 0;

	triangle.A											= matrixViewport.Transform(triangle.A);
	triangle.B											= matrixViewport.Transform(triangle.B);
	triangle.C											= matrixViewport.Transform(triangle.C);

	normal												= matrixTransform.TransformDirection(normal).Normalize();

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	::ced::SCoord2<float>										imageUnit				= {textureImage.metrics().x - 1.0f, textureImage.metrics().y - 1.0f};
	const ::ced::STriangle	<float>								& triangleTexCoords		= geometry.TextureCoords	[iTriangle];
	double														lightFactorDirectional	= normal.Dot(lightVector.Normalize());
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>										pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<double>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::ced::SCoord2<float>										texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord												+= triangleTexCoords.B * vertexWeights.B;
		texCoord												+= triangleTexCoords.C * vertexWeights.C;
		::ced::SCoord3<float>										position				= triangleWorld.A * vertexWeights.A;
		position												+= triangleWorld.B * vertexWeights.B;
		position												+= triangleWorld.C * vertexWeights.C;
		::ced::SColorBGRA												texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		::ced::SColorBGRA												fragmentColor			= {};
		for(uint32_t iLight = 0; iLight < lightPoints.size(); ++iLight) {
			::ced::SCoord3<float>										lightToPoint		= lightPoints[iLight] - position;
			::ced::SCoord3<float>										vectorToLight		= lightToPoint;
			double														lightFactor			= vectorToLight.Dot(normal.Normalize());
			if(lightToPoint.Length() > 5 || lightFactor <= 0)
				continue;
			double														range				= 10;
			double														invAttenuation		= ::std::max(0.0, 1.0 - (lightToPoint.Length() / range));
			fragmentColor											= texelColor * lightColors[iLight] * invAttenuation * .5;
		}
		::ced::setPixel(targetPixels, pixelCoord, texelColor *.5 + fragmentColor);
		(void)lightVector;
	}
	return 0;
}

int													ced::drawQuadTriangle
	( ::ced::view_grid<::ced::SColorBGRA>	targetPixels
	, ::ced::SGeometryQuads				& geometry
	, int								iTriangle
	, ::ced::SMatrix4<float>			& matrixTransform
	, ::ced::SMatrix4<float>			& matrixView
	, ::ced::SMatrix4<float>			& matrixViewport
	, ::ced::SCoord3<float>				& lightVector
	, ::ced::SColorBGRA						 color
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
	, ::ced::container<uint32_t>						& depthBuffer
	) {
	::ced::STriangle3	<float>								triangle			= geometry.Triangles	[iTriangle];
	::ced::SCoord3		<float>								normal				= geometry.Normals		[iTriangle / 2];
	normal												= matrixTransform.TransformDirection(normal).Normalize();

	triangle.A											= matrixTransform.Transform(triangle.A);
	triangle.B											= matrixTransform.Transform(triangle.B);
	triangle.C											= matrixTransform.Transform(triangle.C);

	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1) return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1) return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1) return 0;

	triangle.A											= matrixViewport.Transform(triangle.A);
	triangle.B											= matrixViewport.Transform(triangle.B);
	triangle.C											= matrixViewport.Transform(triangle.C);

	double													lightFactor			= normal.Dot(lightVector);

	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>									pixelCoord			= pixelCoords[iPixelCoord];
		//color.r										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].A);
		//color.g										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].B);
		//color.b										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].C);
		::ced::setPixel(targetPixels, pixelCoord, (color * 0.1) + color * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( ::ced::view_grid<::ced::SColorBGRA>	targetPixels
	, ::ced::SGeometryTriangles			& geometry
	, int								iTriangle
	, ::ced::SMatrix4<float>			& matrixTransform
	, ::ced::SMatrix4<float>			& matrixView
	, ::ced::SMatrix4<float>			& matrixViewport
	, ::ced::SCoord3<float>				& lightVector
	, ::ced::SColorBGRA						 color
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
	, ::ced::container<uint32_t>						& depthBuffer
	) {
	::ced::STriangle3		<float>								triangle			= geometry.Triangles	[iTriangle];
	const ::ced::STriangle3	<float>								& triangleNormals	= geometry.Normals		[iTriangle];
	triangle.A											= matrixTransform.Transform(triangle.A);
	triangle.B											= matrixTransform.Transform(triangle.B);
	triangle.C											= matrixTransform.Transform(triangle.C);

	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1) return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1) return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1) return 0;

	triangle.A											= matrixViewport.Transform(triangle.A);
	triangle.B											= matrixViewport.Transform(triangle.B);
	triangle.C											= matrixViewport.Transform(triangle.C);
	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>									pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<double>					& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::ced::SCoord3<float>									normal					= triangleNormals.A * vertexWeights.A;
		normal												+= triangleNormals.B * vertexWeights.B;
		normal												+= triangleNormals.C * vertexWeights.C;
		normal												= matrixTransform.TransformDirection(normal).Normalize();
		double													lightFactor			= normal.Dot(lightVector);
		//color.r										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].A);
		//color.g										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].B);
		//color.b										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].C);
		::ced::setPixel(targetPixels, pixelCoord, color * lightFactor);
	}
	return 0;
}

int													ced::drawTriangle
	( ::ced::view_grid<::ced::SColorBGRA>					targetPixels
	, ::ced::SGeometryTriangles							& geometry
	, int												iTriangle
	, ::ced::SMatrix4<float>							& matrixTransform
	, ::ced::SMatrix4<float>							& matrixView
	, ::ced::SMatrix4<float>							& matrixViewport
	, ::ced::SCoord3<float>								& lightVector
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
	, ::ced::view_grid<::ced::SColorBGRA>					textureImage
	, ::ced::container<uint32_t>						& depthBuffer
	) {
	::ced::STriangle3		<float>								triangle			= geometry.Triangles		[iTriangle];
	const ::ced::STriangle3	<float>								& triangleNormals	= geometry.Normals			[iTriangle];
	const ::ced::STriangle	<float>								& triangleTexCoords	= geometry.TextureCoords	[iTriangle];
	triangle.A											= matrixTransform.Transform(triangle.A);
	triangle.B											= matrixTransform.Transform(triangle.B);
	triangle.C											= matrixTransform.Transform(triangle.C);

	triangle.A											= matrixView.Transform(triangle.A);
	triangle.B											= matrixView.Transform(triangle.B);
	triangle.C											= matrixView.Transform(triangle.C);
	if(triangle.A.z < 0 || triangle.A.z >= 1)
		return 0;
	if(triangle.B.z < 0 || triangle.B.z >= 1)
		return 0;
	if(triangle.C.z < 0 || triangle.C.z >= 1)
		return 0;

	triangle.A											= matrixViewport.Transform(triangle.A);
	triangle.B											= matrixViewport.Transform(triangle.B);
	triangle.C											= matrixViewport.Transform(triangle.C);
	::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, depthBuffer);

	::ced::SCoord2<float>										imageUnit			= {textureImage.metrics().x - 1.0f, textureImage.metrics().y - 1.0f};
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>										pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<double>						& vertexWeights			= pixelVertexWeights[iPixelCoord];
		::ced::SCoord3<float>										normal					= triangleNormals.A * vertexWeights.A;
		normal													+= triangleNormals.B * vertexWeights.B;
		normal													+= triangleNormals.C * vertexWeights.C;
		normal													= matrixTransform.TransformDirection(normal).Normalize();

		double														lightFactor				= normal.Dot(lightVector);

		::ced::SCoord2<float>										texCoord				= triangleTexCoords.A * vertexWeights.A;
		texCoord												+= triangleTexCoords.B * vertexWeights.B;
		texCoord												+= triangleTexCoords.C * vertexWeights.C;
		::ced::SColorBGRA												texelColor				= textureImage[(uint32_t)(texCoord.y * imageUnit.y)][(uint32_t)(texCoord.x * imageUnit.x)];
		::ced::setPixel(targetPixels, pixelCoord, (texelColor * .3) + texelColor * lightFactor);
	}
	return 0;
}
