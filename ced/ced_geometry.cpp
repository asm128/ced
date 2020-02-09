#include "ced_geometry.h"

// Vertex coordinates for cube faces
static constexpr const ::ced::STriangle3<int8_t>	geometryCube	[12]						=
	{ {{0, 0, 0}, {0, 1, 0}, {1, 0, 0}}	// Right	- first
	, {{0, 1, 0}, {1, 1, 0}, {1, 0, 0}}	// Right	- second

	, {{0, 1, 0}, {0, 0, 0}, {0, 0, 1}}	// Back		- first
	, {{0, 1, 1}, {0, 1, 0}, {0, 0, 1}}	// Back		- second

	, {{0, 0, 1}, {0, 0, 0}, {1, 0, 0}}	// Bottom	- first
	, {{1, 0, 1}, {0, 0, 1}, {1, 0, 0}}	// Bottom	- second

	, {{0, 1, 1}, {0, 0, 1}, {1, 0, 1}}	// Left		- first
	, {{1, 1, 1}, {0, 1, 1}, {1, 0, 1}}	// Left		- second

	, {{1, 1, 0}, {1, 1, 1}, {1, 0, 1}}	// Front	- first
	, {{1, 0, 0}, {1, 1, 0}, {1, 0, 1}}	// Front	- second

	, {{0, 1, 1}, {1, 1, 1}, {1, 1, 0}}	// Top		- first
	, {{0, 1, 0}, {0, 1, 1}, {1, 1, 0}}	// Top		- second
	};

// Vertex coordinates for cube faces
static constexpr const ::ced::STriangle2<int8_t>	texCoordCubeYUp		[12]						=
	{ {{0, 1}, {0, 0}, {1, 1}}	// Right	- first
	, {{0, 0}, {1, 0}, {1, 1}}	// Right	- second
	//{} //
	, {{1, 0}, {1, 1}, {0, 1}}	// Back		- first
	, {{0, 0}, {1, 0}, {0, 1}}	// Back		- second
	//{} //
	, {{0, 0}, {1, 0}, {1, 1}}	// Bottom	- first
	, {{0, 1}, {0, 0}, {1, 1}}	// Bottom	- second
	//{} //
	, {{1, 0}, {1, 1}, {0, 1}}	// Left		- second
	, {{0, 0}, {1, 0}, {0, 1}}	// Left		- first
	//{} //
	, {{0, 0}, {1, 0}, {1, 1}}	// Front	- first
	, {{0, 1}, {0, 0}, {1, 1}}	// Front	- second
	//{} //
	, {{1, 1}, {0, 1}, {0, 0}}	// Top		- first
	, {{1, 0}, {1, 1}, {0, 0}}	// Top		- second
	};

static constexpr const ::ced::SCoord3<int8_t>		geometryNormals	[6]		=
	{ { 0, 0,-1} // Right
	, {-1, 0, 0} // Back
	, { 0,-1, 0} // Bottom
	, { 0, 0, 1} // Left
	, { 1, 0, 0} // Front
	, { 0, 1, 0} // Top
	};

int													ced::geometryBuildCube	(SGeometryQuads & geometry, const ::ced::SCoord3<float> & scale)	{
	const uint32_t											triangleOffset			= geometry.Triangles.size();
	geometry.Triangles		.resize(triangleOffset + (uint32_t)::std::size(geometryCube));
	geometry.Normals		.resize(triangleOffset + (uint32_t)::std::size(geometryNormals));
	geometry.TextureCoords	.resize(triangleOffset + (uint32_t)::std::size(texCoordCubeYUp));

	for(uint32_t iTriangle = 0, countTriangles = (uint32_t)::std::size(geometryCube); iTriangle < countTriangles; ++iTriangle) {
		::ced::STriangle3<float>								& newTriangle		= geometry.Triangles[triangleOffset + iTriangle];
		newTriangle											= geometryCube[iTriangle].Cast<float>();
		newTriangle.A										-= {.5, .5, .5};
		newTriangle.B										-= {.5, .5, .5};
		newTriangle.C										-= {.5, .5, .5};
		newTriangle.Scale(scale);

		::ced::STriangle2<float>								& newTriangleTex	= geometry.TextureCoords[triangleOffset + iTriangle];
		newTriangleTex										= ::texCoordCubeYUp[iTriangle].Cast<float>();

		::ced::SCoord3<float>									& newNormal			= geometry.Normals[(triangleOffset + iTriangle) / 2];
		::ced::SCoord3<float>									normal				= (newTriangle.A - newTriangle.B).Normalize().Cross((newTriangle.A - newTriangle.C).Normalize());
		normal.Normalize();
		newNormal											= normal; //geometryNormals[iTriangle / 2].Cast<float>();
	}
	return 0;
}

int													ced::geometryBuildGrid	(SGeometryQuads & geometry, ::ced::SCoord2<uint32_t> gridSize, ::ced::SCoord2<float> gridCenter, const ::ced::SCoord3<float> & scale)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / gridSize.x, 1.0f / gridSize.y};
	for(uint32_t z = 0; z < gridSize.y; ++z)
	for(uint32_t x = 0; x < gridSize.x; ++x)  {
		::ced::SCoord3<float>									coords	[4]			=
			{ {0, 0, 0}
			, {0, 0, 1}
			, {1, 0, 0}
			, {1, 0, 1}
			};
		::ced::SCoord2<float>									texcoordsOffset			= {x * texCoordUnits.x, z * texCoordUnits.y};
		::ced::SCoord2<float>									texcoords	[4]			=
			{ {0.0f * texCoordUnits.x + texcoordsOffset.x, 0.0f * texCoordUnits.y + texcoordsOffset.y}
			, {0.0f * texCoordUnits.x + texcoordsOffset.x, 1.0f * texCoordUnits.y + texcoordsOffset.y}
			, {1.0f * texCoordUnits.x + texcoordsOffset.x, 0.0f * texCoordUnits.y + texcoordsOffset.y}
			, {1.0f * texCoordUnits.x + texcoordsOffset.x, 1.0f * texCoordUnits.y + texcoordsOffset.y}
			};
		::ced::STriangle2<float>								triangleATex		= {texcoords[0], texcoords[1], texcoords[2]};
		::ced::STriangle2<float>								triangleBTex		= {texcoords[1], texcoords[3], texcoords[2]};
		::ced::STriangle3<float>								triangleA			= {coords[0], coords[1], coords[2]};
		::ced::STriangle3<float>								triangleB			= {coords[1], coords[3], coords[2]};
		triangleA.A											+= {(float)x, 0, (float)z};
		triangleA.B											+= {(float)x, 0, (float)z};
		triangleA.C											+= {(float)x, 0, (float)z};
		triangleB.A											+= {(float)x, 0, (float)z};
		triangleB.B											+= {(float)x, 0, (float)z};
		triangleB.C											+= {(float)x, 0, (float)z};
		triangleA.Scale(scale);
		triangleB.Scale(scale);

		triangleA.Translate(::ced::SCoord3<float>{gridCenter.x, 0, gridCenter.y} * -1);
		triangleB.Translate(::ced::SCoord3<float>{gridCenter.x, 0, gridCenter.y} * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
		::ced::SCoord3<float>									normal				= (triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize());
		normal.Normalize();
		geometry.Normals		.push_back(normal);
	}
	return 0;
}

//
int													ced::geometryBuildFigure0	(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord2<float>									texcoords	[4]			=
			{ {0, 0}
			, {0, 1}
			, {1, 0}
			, {1, 1}
			};
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * x			/ slices), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), cos(::ced::MATH_PI * (x + 1) /slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * x			/ slices), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), cos(::ced::MATH_PI * (x + 1)	/slices)}
			};
		::ced::STriangle3<float>								triangleA				= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle3<float>								triangleB				= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle2<float>								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::ced::STriangle2<float>								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().Cast<float>());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

int													ced::geometryBuildFigure1	(::ced::SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter)	{
	(void)radius;
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord2<float>									texcoords	[4]			=
			{ {0, 0}
			, {0, 1}
			, {1, 0}
			, {1, 1}
			};
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_2PI * x			/ slices), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_2PI * (x + 1)	/ slices), cos(::ced::MATH_PI * (x + 1) /slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_2PI * x			/ slices), cos(::ced::MATH_PI * x		/slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_2PI * (x + 1)	/ slices), cos(::ced::MATH_PI * (x + 1)	/slices)}
			};
		::ced::STriangle3<float>								triangleA				= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle3<float>								triangleB				= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle2<float>								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::ced::STriangle2<float>								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().Cast<float>());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

int													ced::geometryBuildHalfHelix	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord2<float>									texcoords	[4]			=
			{ {(x		) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x		) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			};
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_PI * x			/slices), cos(::ced::MATH_2PI * x		/ slices)}
			, {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_PI * (x + 1)	/slices), cos(::ced::MATH_2PI * (x + 1)	/ slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_PI * x			/slices), cos(::ced::MATH_2PI * x		/ slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_PI * (x + 1)	/slices), cos(::ced::MATH_2PI * (x + 1)	/ slices)}
			};
		::ced::STriangle3<float>								triangleA				= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle3<float>								triangleB				= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle2<float>								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::ced::STriangle2<float>								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.Scale(scale);
		triangleB.Scale(scale);
		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

int													ced::geometryBuildHelix		(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		::ced::SCoord2<float>									texcoords	[4]			=
			{ {(x		) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x		) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			};
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_PI * x			/slices), cos(::ced::MATH_2PI * x		/ slices)}
			, {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_PI * (x + 1)	/slices), cos(::ced::MATH_2PI * (x + 1)	/ slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_PI * x			/slices), cos(::ced::MATH_2PI * x		/ slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_PI * (x + 1)	/slices), cos(::ced::MATH_2PI * (x + 1)	/ slices)}
			};
		::ced::STriangle3<float>								triangleA				= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle3<float>								triangleB				= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle2<float>								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::ced::STriangle2<float>								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		::ced::STriangle3<float>								triangleC				= triangleA;
		::ced::STriangle3<float>								triangleD				= triangleB;
		triangleA.Translate({0, 0, -radius});
		triangleB.Translate({0, 0, -radius});
		triangleC.Translate({0, 0, -radius});
		triangleD.Translate({0, 0, -radius});
		triangleC.Scale({-1, 1, -1});
		triangleD.Scale({-1, 1, -1});

		triangleA.Scale(scale);
		triangleB.Scale(scale);
		triangleC.Scale(scale);
		triangleD.Scale(scale);

		triangleA.Translate(gridCenter * -1);
		triangleB.Translate(gridCenter * -1);
		triangleC.Translate(gridCenter * -1);
		triangleD.Translate(gridCenter * -1);
		::ced::SCoord3<float>									normal					= (triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>();
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Triangles		.push_back(triangleC);
		geometry.Triangles		.push_back(triangleD);
		geometry.Normals		.push_back(normal);
		normal.Scale({-1, -1, 0});
		geometry.Normals		.push_back(normal);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

//
int													ced::geometryBuildSphere	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::ced::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		::ced::SCoord3<double>									coords	[4]				=
			{ {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * x		/slices), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * z			/ stacks)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * (x + 1)	/slices), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * z			/ stacks)}
			, {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * x		/slices), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * (x + 1)	/slices), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks)}
			};
		{
			::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>() * radius, coords[2].Cast<float>() * radius, coords[1].Cast<float>() * radius};
			::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius, coords[3].Cast<float>() * radius};
			triangleA.Translate(gridCenter * -1);
			triangleB.Translate(gridCenter * -1);
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


//
int													ced::geometryBuildCylinder	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t y = 0; y < stacks; ++y)
	for(uint32_t z = 0; z < slices; ++z)
	{
		{
			::ced::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.x, (y		) * texCoordUnits.y}
				, {(z + 1	) * texCoordUnits.x, (y		) * texCoordUnits.y}
				, {(z		) * texCoordUnits.x, (y	+ 1	) * texCoordUnits.y}
				, {(z + 1	) * texCoordUnits.x, (y + 1	) * texCoordUnits.y}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		//const ::ced::SPairSinCos									pairSinCos				= ::ced::getSinCos(::ced::MATH_2PI * z / slices);
		//const double												px						= x * pairSinCos.Cos - z * pairSinCos.Sin;
		//z														= (x * pairSinCos.Sin + z * pairSinCos.Cos);
		//x														= px;
		(void)radius;
		::ced::SCoord3<double>									coords	[4]				=
			{ {1 * radius, (double)(y		)}
			, {1 * radius, (double)(y		)}
			, {1 * radius, (double)(y + 1	)}
			, {1 * radius, (double)(y + 1	)}
			};
		coords[0].RotateY(::ced::MATH_2PI / slices * (z + 0));
		coords[1].RotateY(::ced::MATH_2PI / slices * (z + 1));
		coords[2].RotateY(::ced::MATH_2PI / slices * (z + 0));
		coords[3].RotateY(::ced::MATH_2PI / slices * (z + 1));

		{
			::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>(), coords[2].Cast<float>(), coords[1].Cast<float>()};
			::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>(), coords[2].Cast<float>(), coords[3].Cast<float>()};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate(gridCenter * -1);
			triangleB.Translate(gridCenter * -1);

			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			geometry.Normals	.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().Cast<float>());
		}
	}
	return 0;
}


int													ced::geometryBuildTender	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter, const ::ced::SCoord3<float> & scale)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::ced::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		//const ::ced::SPairSinCos									pairSinCos				= ::ced::getSinCos(::ced::MATH_2PI * z / slices);
		//const double												px						= x * pairSinCos.Cos - z * pairSinCos.Sin;
		//z														= (x * pairSinCos.Sin + z * pairSinCos.Cos);
		//x														= px;
		(void)radius;
		::ced::SCoord3<double>									coords	[4]				=
			{ {1 * radius, (double)(z		)}
			, {1 * radius, (double)(z		)}
			, {1 * radius, (double)(z + 1	)}
			, {1 * radius, (double)(z + 1	)}
			};
		coords[0].RotateY(::ced::MATH_2PI / slices * (x + 0));
		coords[1].RotateY(::ced::MATH_2PI / slices * (x + 1));
		coords[2].RotateY(::ced::MATH_2PI / slices * (x + 0));
		coords[3].RotateY(::ced::MATH_2PI / slices * (x + 1));
		coords[0].Normalize();
		coords[1].Normalize();
		coords[2].Normalize();
		coords[3].Normalize();

		{
			::ced::STriangle3<float>								triangleA			= {coords[0].Cast<float>(), coords[2].Cast<float>(), coords[1].Cast<float>()};
			::ced::STriangle3<float>								triangleB			= {coords[1].Cast<float>(), coords[2].Cast<float>(), coords[3].Cast<float>()};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate(gridCenter * -1);
			triangleB.Translate(gridCenter * -1);
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			geometry.Normals	.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		}
	}
	return 0;
}

int													ced::geometryBuildSphere	(SGeometryTriangles & geometry, uint32_t stacks, uint32_t slices, float radius, const ::ced::SCoord3<float> & gridCenter)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t z = 0; z < stacks; ++z)
	for(uint32_t x = 0; x < slices; ++x)  {
		{
			::ced::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z		) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x		) * texCoordUnits.x}
				, {(z + 1	) * texCoordUnits.y, (x + 1	) * texCoordUnits.x}
				};
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		::ced::SCoord3<double>									coords		[4]				=
			{ {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * x		/slices), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * z			/ stacks)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * z			/ stacks), cos(::ced::MATH_PI * (x + 1) /slices), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * z			/ stacks)}
			, {sin(::ced::MATH_PI * x		/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * x		/slices), sin(::ced::MATH_PI * x		/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks)}
			, {sin(::ced::MATH_PI * (x + 1)	/slices) * cos(::ced::MATH_2PI * (z + 1)	/ stacks), cos(::ced::MATH_PI * (x + 1)	/slices), sin(::ced::MATH_PI * (x + 1)	/ slices) * sin(::ced::MATH_2PI * (z + 1)	/ stacks)}
			};
		{
			geometry.Normals.push_back(
				{ coords[0].Normalize().Cast<float>()
				, coords[2].Normalize().Cast<float>()
				, coords[1].Normalize().Cast<float>()
				});
			geometry.Normals.push_back(
				{ coords[1].Normalize().Cast<float>()
				, coords[2].Normalize().Cast<float>()
				, coords[3].Normalize().Cast<float>()
				});
		}
		{
			::ced::STriangle3<float>								triangleA			= {(coords[0] * radius).Cast<float>(), (coords[2] * radius).Cast<float>(), (coords[1] * radius).Cast<float>()};
			::ced::STriangle3<float>								triangleB			= {(coords[1] * radius).Cast<float>(), (coords[2] * radius).Cast<float>(), (coords[3] * radius).Cast<float>()};
			triangleA.Translate(gridCenter * -1);
			triangleB.Translate(gridCenter * -1);
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
		}
	}
	return 0;
}


int													ced::geometryBuildTileListFromImage		(::ced::view_grid<const ::ced::SColorBGRA> image, ::ced::container<::ced::STile> & out_tiles)	{
	{
		::ced::STile											newTile;
		for(uint32_t z = 0; z < image.metrics().y; ++z)
		for(uint32_t x = 0; x < image.metrics().x; ++x) {
			::ced::SColorFloat										currentPixel							= image[z][x];
			float													pixelHeight								= float((currentPixel.r + (double)currentPixel.g + currentPixel.b) / 3.0);
			for(uint32_t iCorner = 0; iCorner < 4; ++iCorner)
				newTile.Height[iCorner]								= pixelHeight;
			newTile.Top											= 1;
			newTile.Front										= -1;
			newTile.Right										= -1;
			newTile.Flags										= 0;
			out_tiles.push_back(newTile);
		}
	}
	for(uint32_t z = 0; z < image.metrics().y; ++z)
	for(uint32_t x = 0; x < image.metrics().x - 1; ++x) {
		::ced::STile											& currentTile							= out_tiles[z		* image.metrics().y + x];
		::ced::STile											& frontTile								= out_tiles[z		* image.metrics().y + x + 1];
		if(currentTile.Height[1] != frontTile.Height[0] || currentTile.Height[3] != frontTile.Height[2])
			currentTile.Front									= 1;
	}
	for(uint32_t z = 0; z < image.metrics().y - 1; ++z)
	for(uint32_t x = 0; x < image.metrics().x; ++x) {
		::ced::STile											& currentTile							= out_tiles[z		* image.metrics().y + x];
		::ced::STile											& rightTile								= out_tiles[(z + 1)	* image.metrics().y + x];
		if(currentTile.Height[2] != rightTile.Height[0] || currentTile.Height[3] != rightTile.Height[1])
			currentTile.Right									= 1;
	}
	return 0;
}

int													ced::geometryBuildGridFromTileList		(::ced::SGeometryQuads & geometry, ::ced::view_grid<const ::ced::STile> tiles, ::ced::SCoord2<float> gridCenter, const ::ced::SCoord3<float> & scale) {
	::ced::SCoord2<float>									texCoordUnits						= {1.0f / tiles.metrics().x, 1.0f / tiles.metrics().y};
	for(uint32_t z = 0; z < tiles.metrics().y; ++z)
	for(uint32_t x = 0; x < tiles.metrics().x; ++x) {
		::ced::SCoord2<float>									texcoords	[4]				=
			{ {(x		) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z		) * texCoordUnits.y}
			, {(x		) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			, {(x + 1	) * texCoordUnits.x, (z + 1	) * texCoordUnits.y}
			};
		const ::ced::STile										& currentTile							= tiles[z][x];
		{
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		{ // top
			::ced::SCoord3<float>									coords	[4]			=
				{ {x + 0.f, currentTile.Height[0], (z + 0) * -1.f}
				, {x + 0.f, currentTile.Height[1], (z + 1) * -1.f}
				, {x + 1.f, currentTile.Height[2], (z + 0) * -1.f}
				, {x + 1.f, currentTile.Height[3], (z + 1) * -1.f}
				};
			if(coords[0].y || coords[1].y || coords[2].y || coords[3].y) {
				::ced::STriangle3<float>								triangleA			= {coords[0], coords[2], coords[1]};//
				::ced::STriangle3<float>								triangleB			= {coords[1], coords[2], coords[3]};//
				triangleA.Scale(scale);
				triangleB.Scale(scale);
				triangleA.Translate({-gridCenter.x, 0, -gridCenter.y});
				triangleB.Translate({-gridCenter.x, 0, -gridCenter.y});
				geometry.Triangles	.push_back(triangleA);
				geometry.Triangles	.push_back(triangleB);
				geometry.Normals.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().Cast<float>());
				//geometry.Normals.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
			}
		}
		if(0 <= currentTile.Front || (x == (tiles.metrics().x - 1) && (0 != currentTile.Height[1] || 0 != currentTile.Height[3]))) {
			::ced::SCoord3<float>									coords[4];
			if(0 <= currentTile.Front) { // front
				const ::ced::STile										& frontTile			= tiles[z][x + 1];
				coords[0]											= {x + 1.f, currentTile.Height	[1], (z + 0) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 1.f, frontTile.Height	[0], (z + 0) * -1.f};
				coords[3]											= {x + 1.f, frontTile.Height	[2], (z + 1) * -1.f};
			}
			else {
				coords[0]											= {x + 1.f, currentTile.Height	[1], (z + 0) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 1.f, 0, (z + 0) * -1.f};
				coords[3]											= {x + 1.f, 0, (z + 1) * -1.f};
			}
			::ced::STriangle3<float>								triangleA			= {coords[0], coords[2], coords[1]};//{coords[0], coords[2], coords[1]};
			::ced::STriangle3<float>								triangleB			= {coords[1], coords[2], coords[3]};//{coords[1], coords[2], coords[3]};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate({-gridCenter.x, 0, -gridCenter.y});
			triangleB.Translate({-gridCenter.x, 0, -gridCenter.y});
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			//geometry.Normals.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
			geometry.Normals.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().Cast<float>());
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
		if(0 <= currentTile.Right || ((z == tiles.metrics().y - 1) && (0 != currentTile.Height[2] || 0 != currentTile.Height[3]))) {
			::ced::SCoord3<float>									coords[4];
			if(0 <= currentTile.Right) {
				const ::ced::STile										& rightTile			= tiles[z + 1][x];
				coords[0]											= {x + 0.f, currentTile.Height	[2], (z + 1) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 0.f, rightTile.Height	[0], (z + 1) * -1.f};
				coords[3]											= {x + 1.f, rightTile.Height	[1], (z + 1) * -1.f};
			}
			else {
				coords[0]											= {x + 0.f, currentTile.Height	[2], (z + 1) * -1.f};
				coords[1]											= {x + 1.f, currentTile.Height	[3], (z + 1) * -1.f};
				coords[2]											= {x + 0.f, 0, (z + 1) * -1.f};
				coords[3]											= {x + 1.f, 0, (z + 1) * -1.f};
			}
			::ced::STriangle3<float>								triangleA			= {coords[0], coords[1], coords[2]};
			::ced::STriangle3<float>								triangleB			= {coords[1], coords[3], coords[2]};
			triangleA.Scale(scale);
			triangleB.Scale(scale);
			triangleA.Translate({-gridCenter.x, 0, -gridCenter.y});
			triangleB.Translate({-gridCenter.x, 0, -gridCenter.y});
			geometry.Triangles	.push_back(triangleA);
			geometry.Triangles	.push_back(triangleB);
			geometry.Normals.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
			//geometry.Normals.push_back((triangleB.A - triangleB.B).Normalize().Cross((triangleA.A - triangleA.B).Normalize()).Normalize().Cast<float>());
			geometry.TextureCoords.push_back(
				{ texcoords[0]
				, texcoords[2]
				, texcoords[1]
				});
			geometry.TextureCoords.push_back(
				{ texcoords[1]
				, texcoords[2]
				, texcoords[3]
				});
		}
	}
	return 0;
}
