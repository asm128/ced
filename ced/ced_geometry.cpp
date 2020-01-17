#include "ced_geometry.h"

// Vertex coordinates for cube faces
static constexpr const ::ced::STriangle3<int8_t>	geometryCube	[12]						=
	{ {{1, 0, 0}, {0, 0, 0}, {0, 1, 0}}	// Right	- first
	, {{1, 0, 0}, {0, 1, 0}, {1, 1, 0}}	// Right	- second

	, {{0, 0, 1}, {0, 1, 0}, {0, 0, 0}}	// Back		- first
	, {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}}	// Back		- second

	, {{1, 0, 0}, {0, 0, 1}, {0, 0, 0}}	// Bottom	- first
	, {{1, 0, 0}, {1, 0, 1}, {0, 0, 1}}	// Bottom	- second

	, {{1, 0, 1}, {0, 1, 1}, {0, 0, 1}}	// Left		- first
	, {{1, 0, 1}, {1, 1, 1}, {0, 1, 1}}	// Left		- second

	, {{1, 0, 1}, {1, 1, 0}, {1, 1, 1}}	// Front	- first
	, {{1, 0, 1}, {1, 0, 0}, {1, 1, 0}}	// Front	- second

	, {{1, 1, 0}, {0, 1, 1}, {1, 1, 1}}	// Top		- first
	, {{1, 1, 0}, {0, 1, 0}, {0, 1, 1}}	// Top		- second
	};

static constexpr const ::ced::SCoord3<int8_t>		geometryNormals	[6]		=
	{ { 0, 0, 1} // Right
	, {-1, 0, 0} // Back
	, { 0,-1, 0} // Bottom
	, { 0, 0,-1} // Left
	, { 1, 0, 0} // Front
	, { 0, 1, 0} // Top
	};

int													ced::geometryBuildCube	(SGeometryQuads & geometry)	{
	geometry.Triangles	.resize((uint32_t)::std::size(geometryCube));
	geometry.Normals	.resize((uint32_t)::std::size(geometryNormals));

	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		::ced::STriangle3<float>								& newTriangle		= geometry.Triangles[iTriangle];
		newTriangle											= geometryCube[iTriangle].Cast<float>();
		newTriangle.A										-= {.5, .5, .5};
		newTriangle.B										-= {.5, .5, .5};
		newTriangle.C										-= {.5, .5, .5};

		::ced::SCoord3<float>									& newNormal			= geometry.Normals[iTriangle / 2];
		newNormal											= geometryNormals[iTriangle / 2].Cast<float>();
	}
	return 0;
}

int													ced::geometryBuildGrid	(SGeometryQuads & geometry, ::ced::SCoord2<uint32_t> gridSize, ::ced::SCoord2<float> gridCenter)	{
	for(uint32_t z = 0; z < gridSize.y; ++z)
	for(uint32_t x = 0; x < gridSize.x; ++x)  {
		::ced::SCoord3<double>									coords	[4]			=
			{ {0, 0, 0}
			, {0, 0, 1}
			, {1, 0, 0}
			, {1, 0, 1}
			};
		::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>(), coords[1].Cast<float>(), coords[2].Cast<float>()};
		::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>(), coords[3].Cast<float>(), coords[2].Cast<float>()};
		//::ced::STriangle3<float>								triangleA			= {{1, 0, 0}, {0, 0, 1}, {1, 0, 1}};
		//::ced::STriangle3<float>								triangleB			= {{1, 0, 0}, {0, 0, 0}, {0, 0, 1}};
		triangleA.A											+= {(float)x, 0, (float)z};
		triangleA.B											+= {(float)x, 0, (float)z};
		triangleA.C											+= {(float)x, 0, (float)z};
		triangleB.A											+= {(float)x, 0, (float)z};
		triangleB.B											+= {(float)x, 0, (float)z};
		triangleB.C											+= {(float)x, 0, (float)z};

		triangleA.A											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleA.B											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleA.C											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.A											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.B											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.C											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		geometry.Triangles	.push_back(triangleA);
		geometry.Triangles	.push_back(triangleB);
		geometry.Normals	.push_back({0, 1, 0});
	}
	return 0;
}

//
int													ced::geometryBuildFigure0	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * x			/ slices), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), cos(::ced::MATH_PI * (x + 1) /slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * x			/ slices), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), cos(::ced::MATH_PI * (x + 1)	/slices)}
			};
		::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		triangleA.A											-= gridCenter;
		triangleA.B											-= gridCenter;
		triangleA.C											-= gridCenter;
		triangleB.A											-= gridCenter;
		triangleB.B											-= gridCenter;
		triangleB.C											-= gridCenter;
		geometry.Triangles	.push_back(triangleA);
		geometry.Triangles	.push_back(triangleB);
		geometry.Normals	.push_back({0, 1, 0});
	}
	return 0;
}

//
int													ced::geometryBuildSphere	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * z			/ stacks), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * z			/ stacks), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * (x + 1) /slices)}
			, {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * (x + 1)	/slices)}
			};
		{
			::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
			::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
			triangleA.A											-= gridCenter;
			triangleA.B											-= gridCenter;
			triangleA.C											-= gridCenter;
			triangleB.A											-= gridCenter;
			triangleB.B											-= gridCenter;
			triangleB.C											-= gridCenter;
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
		}
		{
			::ced::SCoord3<float>									normal				= coords[0].Cast<float>();
			normal												+= coords[1].Cast<float>();
			normal												+= coords[2].Cast<float>();
			normal												+= coords[3].Cast<float>();
			normal												/= 4;
			normal.Normalize();
			geometry.Normals.push_back(normal);
		}
	}
	return 0;
}

int													ced::geometryBuildSphere	(SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * z			/ stacks), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * z			/ stacks), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * (x + 1) /slices)}
			, {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * (x + 1)	/slices)}
			};
		{
			geometry.Normals.push_back(
				{ coords[0].Normalize().Cast<float>()
				, coords[1].Normalize().Cast<float>()
				, coords[2].Normalize().Cast<float>()
				});
			geometry.Normals.push_back(
				{ coords[1].Normalize().Cast<float>()
				, coords[3].Normalize().Cast<float>()
				, coords[2].Normalize().Cast<float>()
				});
		}
		{
			::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
			::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
			triangleA.A											-= gridCenter;
			triangleA.B											-= gridCenter;
			triangleA.C											-= gridCenter;
			triangleB.A											-= gridCenter;
			triangleB.B											-= gridCenter;
			triangleB.C											-= gridCenter;
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
		}
	}
	return 0;
}