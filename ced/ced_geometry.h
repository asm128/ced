#include "ced_view.h"
#include "ced_color.h"

#ifndef CED_GEOMETRY_H_29234234
#define CED_GEOMETRY_H_29234234

namespace ced
{
#pragma pack(push, 1)
	struct SGeometryGroupModes {
		uint16_t											Transparent			: 1;
		uint16_t											SkipSpecular		: 1;
		uint16_t											SkipAmbient			: 1;
		uint16_t											SkipDiffuse			: 1;
	};
	struct SGeometryGroup {
		int16_t												Image				;
		::ced::SGeometryGroupModes							Modes				;
		::ced::SSlice<uint16_t>								Slice				;
		//::ced::container<uint16_t>	IndicesPointLightCache	;
	};
#pragma pack(pop)

	struct SGeometryQuads {
		::ced::container<::ced::STriangle3	<float>>		Triangles;
		::ced::container<::ced::SCoord3		<float>>		Normals;
		::ced::container<::ced::STriangle2	<float>>		TextureCoords;
	};

	struct SGeometryTriangles {
		::ced::container<::ced::STriangle3<float>>			Triangles;
		::ced::container<::ced::STriangle3<float>>			Normals;
		::ced::container<::ced::STriangle2<float>>			TextureCoords;
	};

	struct SModelQuads {
		::ced::container<::ced::STriangle3	<float>>		Triangles;
		::ced::container<::ced::SCoord3		<float>>		Normals;
		::ced::container<::ced::STriangle2	<float>>		TextureCoords;
		::ced::container<::ced::SGeometryGroup>				GeometryGroups;
	};

	struct SModelTriangles {
		::ced::container<::ced::STriangle3<float>>			Triangles;
		::ced::container<::ced::STriangle3<float>>			Normals;
		::ced::container<::ced::STriangle2<float>>			TextureCoords;
		::ced::container<::ced::SGeometryGroup>				GeometryGroups;
	};

	struct SModelManager {

	};

	int													geometryBuildCube				(::ced::SGeometryQuads & geometry, const ::ced::SCoord3<float> & scale);
	int													geometryBuildGrid				(::ced::SGeometryQuads & geometry, ::ced::SCoord2<uint32_t> gridSize, ::ced::SCoord2<float> gridCenter, const ::ced::SCoord3<float> & scale);
	int													geometryBuildSphere				(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter);
	int													geometryBuildCylinder			(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale);
	int													geometryBuildHalfHelix			(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale);
	int													geometryBuildHelix				(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale);
	int													geometryBuildTender				(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale);
	int													geometryBuildFigure0			(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter);
	int													geometryBuildFigure1			(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter);

	int													geometryBuildSphere				(::ced::SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter);
#pragma pack(push, 1)
	struct STile {
		float												Height[4]	;
		int16_t												Top			;
		int16_t												Front		;
		int16_t												Right		;
		int16_t												Flags		;
	};
#pragma pack(pop)
	int													geometryBuildTileListFromImage	(::ced::view_grid<const ::ced::SColorBGRA> image, ::ced::container<STile> & out_tiles, uint32_t imagePitch = 0);
	int													geometryBuildGridFromTileList	(::ced::SGeometryQuads & geometry, ::ced::view_grid<const ::ced::STile> image, ::ced::SCoord2<float> gridCenter, const ::ced::SCoord3<float> & scale);

} // namespace

#endif // CED_GEOMETRY_H_29234234
