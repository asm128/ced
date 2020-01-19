#include "ced_matrix.h"
#include "ced_geometry.h"
#include "ced_color.h"

#include <algorithm>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace ced
{

	int								setPixel			(::ced::view_grid<::ced::SColor> pixels	, ::ced::SCoord2	<int32_t>	position	, ::ced::SColor color);
	int								drawRectangle		(::ced::view_grid<::ced::SColor> pixels	, ::ced::SRectangle	<int32_t>	rectangle	, ::ced::SColor color);
	int								drawCircle			(::ced::view_grid<::ced::SColor> pixels	, ::ced::SCircle	<int32_t>	circle		, ::ced::SColor color);
	int								drawLine			(::ced::view_grid<::ced::SColor> pixels	, ::ced::SLine		<int32_t>	line		, ::ced::SColor color);
	int								drawLine			(::ced::view_grid<::ced::SColor> pixels	, ::ced::SLine		<int32_t>	line		, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords);
	int								drawTriangle		(::ced::view_grid<::ced::SColor> pixels	, ::ced::STriangle	<int32_t>	triangle	, ::ced::SColor color);
	int								drawTriangle		(::ced::SCoord2<uint32_t> targetSize	, ::ced::STriangle3	<float>		triangle	, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords, ::ced::container<::ced::STriangleWeights<double>> & proportions, ::ced::container<uint32_t> & depthBuffer);
	int								drawQuadTriangle
		( ::ced::view_grid<::ced::SColor>					targetPixels
		, ::ced::SGeometryQuads								& geometry
		, int												iTriangle
		, ::ced::SMatrix4<float>							& matrixTransform
		, ::ced::SMatrix4<float>							& matrixView
		, ::ced::SMatrix4<float>							& matrixViewport
		, ::ced::SCoord3<float>								& lightVector
		, ::ced::SColor										color
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::container<uint32_t>						& depthBuffer
		);
		int								drawQuadTriangle
		( ::ced::view_grid<::ced::SColor>					targetPixels
		, ::ced::SGeometryQuads								& geometry
		, int												iTriangle
		, ::ced::SMatrix4<float>							& matrixTransform
		, ::ced::SMatrix4<float>							& matrixView
		, ::ced::SMatrix4<float>							& matrixViewport
		, ::ced::SCoord3<float>								& lightVector
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColor>					textureImage
		, ::ced::container<uint32_t>						& depthBuffer
		);
	int								drawTriangle
		( ::ced::view_grid<::ced::SColor>					targetPixels
		, ::ced::SGeometryTriangles							& geometry
		, int												iTriangle
		, ::ced::SMatrix4<float>							& matrixTransform
		, ::ced::SMatrix4<float>							& matrixView
		, ::ced::SMatrix4<float>							& matrixViewport
		, ::ced::SCoord3<float>								& lightVector
		, ::ced::SColor										color
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::container<uint32_t>						& depthBuffer
		);

	int								drawTriangle
		( ::ced::view_grid<::ced::SColor>					targetPixels
		, ::ced::SGeometryTriangles							& geometry
		, int												iTriangle
		, ::ced::SMatrix4<float>							& matrixTransform
		, ::ced::SMatrix4<float>							& matrixView
		, ::ced::SMatrix4<float>							& matrixViewport
		, ::ced::SCoord3<float>								& lightVector
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColor>					textureImage
		, ::ced::container<uint32_t>						& depthBuffer
		);
} // namespace

#endif // CED_DRAW_H_29837429837
