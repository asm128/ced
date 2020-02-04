#include "ced_view.h"

#ifndef CED_GEOMETRY_H_29234234
#define CED_GEOMETRY_H_29234234

namespace ced
{
	struct SGeometryQuads {
		::ced::container<::ced::STriangle3	<float>>		Triangles;
		::ced::container<::ced::SCoord3		<float>>		Normals;
		::ced::container<::ced::STriangle2<float>>			TextureCoords;
	};

	struct SGeometryTriangles {
		::ced::container<::ced::STriangle3<float>>			Triangles;
		::ced::container<::ced::STriangle3<float>>			Normals;
		::ced::container<::ced::STriangle2<float>>			TextureCoords;
	};

	int													geometryBuildCube		(SGeometryQuads & geometry);
	int													geometryBuildGrid		(SGeometryQuads & geometry, ::ced::SCoord2<uint32_t> gridSize, ::ced::SCoord2<float> gridCenter, const ::ced::SCoord3<float> & scale);
	int													geometryBuildFigure0	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter);
	int													geometryBuildSphere		(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter);
	int													geometryBuildCylinder	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter);
	int													geometryBuildTender		(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter);

	int													geometryBuildSphere		(SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter);
} // namespace

#endif // CED_GEOMETRY_H_29234234
