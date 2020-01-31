#include "ced_matrix.h"
#include "ced_geometry.h"
#include "ced_color.h"

#include <algorithm>

#ifndef CED_DRAW_H_29837429837
#define CED_DRAW_H_29837429837

namespace ced
{
	struct SLight3 {
		::ced::SCoord3<float>			Position;
		float							Range;
	};

	int								setPixel			(::ced::view_grid<::ced::SColorBGRA> pixels	, ::ced::SCoord2	<int32_t>	position	, ::ced::SColorBGRA color);
	int								drawRectangle		(::ced::view_grid<::ced::SColorBGRA> pixels	, ::ced::SRectangle	<int32_t>	rectangle	, ::ced::SColorBGRA color);
	int								drawCircle			(::ced::view_grid<::ced::SColorBGRA> pixels	, ::ced::SCircle	<int32_t>	circle		, ::ced::SColorBGRA color);
	int								drawLine			(::ced::view_grid<::ced::SColorBGRA> pixels	, ::ced::SLine2		<int32_t>	line		, ::ced::SColorBGRA color);
	int								drawLine			(::ced::view_grid<::ced::SColorBGRA> pixels	, ::ced::SLine2		<int32_t>	line		, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords);
	int								drawLine
		( const ::ced::view_grid<::ced::SColorBGRA>		pixels
		, const ::ced::SLine3<float>					& lineFloat
		, ::ced::container<::ced::SCoord3<float>>		& pixelCoords
		, ::ced::view_grid<uint32_t>					depthBuffer
		);
	int								drawTriangle		(::ced::view_grid<::ced::SColorBGRA> pixels	, ::ced::STriangle2	<int32_t>	triangle	, ::ced::SColorBGRA color);
	int								drawTriangle		(::ced::SCoord2<uint32_t> targetSize	, ::ced::STriangle3	<float>		triangle	, ::ced::container<::ced::SCoord2<int32_t>> & pixelCoords, ::ced::container<::ced::STriangleWeights<double>> & proportions, ::ced::view_grid<uint32_t> depthBuffer);
	int								drawQuadTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, const ::ced::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::ced::SMatrix4<float>						& matrixTransform
		, const ::ced::SMatrix4<float>						& matrixTransformView
		, const ::ced::SCoord3<float>						& lightVector
		, const ::ced::SColorBGRA							color
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, const ::ced::SGeometryQuads						& geometry
		, const int											iTriangle
		, const ::ced::SMatrix4<float>						& matrixTransform
		, const ::ced::SMatrix4<float>						& matrixTransformView
		, const ::ced::SCoord3<float>						& lightVector
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColorBGRA>				textureImage
		, ::ced::container<::ced::SCoord3<float>>			& lightPoints
		, ::ced::container<::ced::SColorBGRA>				& lightColors
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, const ::ced::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::ced::SMatrix4<float>						& matrixTransform
		, const ::ced::SMatrix4<float>						& matrixTransformView
		, const ::ced::SCoord3<float>						& lightVector
		, ::ced::SColorBGRA									color
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, ::ced::STriangle3			<float>					triangle
		, ::ced::SCoord3			<float>					normal
		, const ::ced::STriangle2	<float>					& triangleTexCoords
		, const ::ced::SMatrix4<float>						& matrixTransform
		, const ::ced::SMatrix4<float>						& matrixTransformView
		, const ::ced::SCoord3<float>						& lightVector
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColorBGRA>				textureImage
		, ::ced::container<::ced::SCoord3<float>>			& lightPoints
		, ::ced::container<::ced::SColorBGRA>				& lightColors
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
	int								drawQuadTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, const ::ced::STriangle3	<float>					& triangleWorld
		, const ::ced::STriangle3	<float>					& triangleScreen
		, const ::ced::SCoord3		<float>					& normal
		, const ::ced::STriangle2	<float>					& triangleTexCoords
		, const ::ced::SCoord3		<float>					& lightVector
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColorBGRA>				textureImage
		, ::ced::container<::ced::SCoord3<float>>			& lightPoints
		, ::ced::container<::ced::SColorBGRA>				& lightColors
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, const ::ced::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::ced::SMatrix4<float>						& matrixTransform
		, const ::ced::SMatrix4<float>						& matrixTransformView
		, const ::ced::SCoord3<float>						& lightVector
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColorBGRA>				textureImage
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
	int								drawTriangle
		( const ::ced::view_grid<::ced::SColorBGRA>			targetPixels
		, const ::ced::SGeometryTriangles					& geometry
		, const int											iTriangle
		, const ::ced::SMatrix4<float>						& matrixTransform
		, const ::ced::SMatrix4<float>						& matrixTransformView
		, const ::ced::SCoord3<float>						& lightVector
		, const ::ced::SColorFloat							& lightColor
		, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
		, ::ced::container<::ced::STriangleWeights<double>>	& pixelVertexWeights
		, ::ced::view_grid<::ced::SColorBGRA>				textureImage
		, ::ced::container<::ced::SLight3>					& lightPoints
		, ::ced::container<::ced::SColorBGRA>				& lightColors
		, ::ced::view_grid<uint32_t>						depthBuffer
		);
} // namespace

#endif // CED_DRAW_H_29837429837
