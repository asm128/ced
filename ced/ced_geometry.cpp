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
	{ { 0, 0, 1} // Right
	, {-1, 0, 0} // Back
	, { 0,-1, 0} // Bottom
	, { 0, 0,-1} // Left
	, { 1, 0, 0} // Front
	, { 0, 1, 0} // Top
	};

int													ced::geometryBuildCube	(SGeometryQuads & geometry)	{
	geometry.Triangles		.resize((uint32_t)::std::size(geometryCube));
	geometry.Normals		.resize((uint32_t)::std::size(geometryNormals));
	geometry.TextureCoords	.resize((uint32_t)::std::size(texCoordCubeYUp));

	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		::ced::STriangle3<float>								& newTriangle		= geometry.Triangles[iTriangle];
		newTriangle											= geometryCube[iTriangle].Cast<float>();
		newTriangle.A										-= {.5, .5, .5};
		newTriangle.B										-= {.5, .5, .5};
		newTriangle.C										-= {.5, .5, .5};

		::ced::STriangle2<float>								& newTriangleTex	= geometry.TextureCoords[iTriangle];
		newTriangleTex										= ::texCoordCubeYUp[iTriangle].Cast<float>();

		::ced::SCoord3<float>									& newNormal			= geometry.Normals[iTriangle / 2];
		newNormal											= geometryNormals[iTriangle / 2].Cast<float>();
	}
	return 0;
}

int													ced::geometryBuildGrid	(SGeometryQuads & geometry, ::ced::SCoord2<uint32_t> gridSize, ::ced::SCoord2<float> gridCenter, const ::ced::SCoord3<float> & scale)	{
	for(uint32_t z = 0; z < gridSize.y; ++z)
	for(uint32_t x = 0; x < gridSize.x; ++x)  {
		::ced::SCoord3<float>									coords	[4]			=
			{ {0, 0, 0}
			, {0, 0, 1}
			, {1, 0, 0}
			, {1, 0, 1}
			};
		::ced::SCoord2<float>									texcoords	[4]			=
			{ {0, 0}
			, {0, 1}
			, {1, 0}
			, {1, 1}
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

		triangleA.A											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleA.B											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleA.C											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.A											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.B											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		triangleB.C											-= {(float)gridCenter.x, 0, (float)gridCenter.y};
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
	}
	return 0;
}

//
int													ced::geometryBuildFigure0	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
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
		triangleA.A											-= gridCenter;
		triangleA.B											-= gridCenter;
		triangleA.C											-= gridCenter;
		triangleB.A											-= gridCenter;
		triangleB.B											-= gridCenter;
		triangleB.C											-= gridCenter;
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().Cast<float>());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

int													ced::geometryBuildFigure1	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
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
		triangleA.A											-= gridCenter;
		triangleA.B											-= gridCenter;
		triangleA.C											-= gridCenter;
		triangleB.A											-= gridCenter;
		triangleB.B											-= gridCenter;
		triangleB.C											-= gridCenter;
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().Cast<float>());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

int													ced::geometryBuildHalfHelix	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
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
			{ {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_PI * x			/slices), cos(::ced::MATH_2PI * x		/ slices)}
			, {sin(::ced::MATH_PI * z		/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * z		/ stacks	) * cos(::ced::MATH_PI * (x + 1)	/slices), cos(::ced::MATH_2PI * (x + 1)	/ slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * x			/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_PI * x			/slices), cos(::ced::MATH_2PI * x		/ slices)}
			, {sin(::ced::MATH_PI * (z + 1)	/ stacks) * sin(::ced::MATH_2PI * (x + 1)	/ slices), sin(::ced::MATH_PI * (z + 1)	/ stacks	) * cos(::ced::MATH_PI * (x + 1)	/slices), cos(::ced::MATH_2PI * (x + 1)	/ slices)}
			};
		::ced::STriangle3<float>								triangleA				= {coords[0].Cast<float>() * radius, coords[1].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle3<float>								triangleB				= {coords[1].Cast<float>() * radius, coords[3].Cast<float>() * radius, coords[2].Cast<float>() * radius};
		::ced::STriangle2<float>								triangleATex			= {texcoords[0], texcoords[1], texcoords[2]};
		::ced::STriangle2<float>								triangleBTex			= {texcoords[1], texcoords[3], texcoords[2]};
		triangleA.A											-= gridCenter;
		triangleA.B											-= gridCenter;
		triangleA.C											-= gridCenter;
		triangleB.A											-= gridCenter;
		triangleB.B											-= gridCenter;
		triangleB.C											-= gridCenter;
		geometry.Triangles		.push_back(triangleA);
		geometry.Triangles		.push_back(triangleB);
		geometry.Normals		.push_back((triangleA.A - triangleA.B).Normalize().Cross((triangleB.A - triangleB.B).Normalize()).Normalize().Cast<float>());
		//geometry.Normals		.push_back((coords[0] - coords[1]).Normalize().Cross((coords[1] - coords[3]).Normalize()).Normalize().Cast<float>());
		geometry.TextureCoords	.push_back(triangleATex);
		geometry.TextureCoords	.push_back(triangleBTex);
	}
	return 0;
}

//
int													ced::geometryBuildSphere	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
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


//
int													ced::geometryBuildCylinder	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
	::ced::SCoord2<float>									texCoordUnits				= {1.0f / slices, 1.0f / stacks};
	for(uint32_t y = 0; y < stacks; ++y)
	for(uint32_t z = 0; z < slices; ++z)
	{
		{
			::ced::SCoord2<float>									texcoords	[4]				=
				{ {(z		) * texCoordUnits.x, (y		) * texCoordUnits.y}
				, {(z		) * texCoordUnits.x, (y + 1	) * texCoordUnits.y}
				, {(z + 1	) * texCoordUnits.x, (y		) * texCoordUnits.y}
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


int													ced::geometryBuildTender	(SGeometryQuads & geometry, uint32_t stacks, uint32_t slices, float radius, ::ced::SCoord3<float> gridCenter)	{
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
