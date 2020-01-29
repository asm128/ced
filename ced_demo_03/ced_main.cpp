#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"
#include "ced_matrix.h"

#include <cstdint>
#include <algorithm>

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

struct SModel3 {
	::ced::SCoord3<float>								Scale;
	::ced::SCoord3<float>								Rotation;
	::ced::SCoord3<float>								Position;
};

struct SGeometryQuads {
	::ced::container<::ced::STriangle3	<float>>		Triangles;
	::ced::container<::ced::SCoord3		<float>>		Normals;
};

struct SApplication {
	::ced::SWindow										Window				= {};
	::ced::SColorBGRA										* Pixels			= 0;
	::ced::STimer										Timer				= {};
	bool												Running				= true;
	double												TotalTime			= 0;
	::ced::SColorBGRA									Colors		[4]		= { {0xff,0, 0}, {0, 0xFF, 0}, {0, 0, 0xFF}, {0xFF, 0xC0, 0x40} };

	::ced::container<::SModel3>						Models;
	::ced::container<uint32_t>							DepthBuffer;
	::SGeometryQuads									Geometry;
};

int													cleanup				(SApplication & app)	{
	free(app.Pixels);
	::ced::windowCleanup(app.Window);
	return 0;
}

int													geometryBuildCube	(SGeometryQuads & geometry)	{
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

int													geometryBuildGrid	(SGeometryQuads & geometry, ::ced::SCoord2<uint32_t> gridSize, ::ced::SCoord2<float> gridCenter)	{
	for(uint32_t z = 0; z < gridSize.y; ++z)
	for(uint32_t x = 0; x < gridSize.x; ++x)  {
		::ced::STriangle3<float>								triangleA			= {{1, 0, 0}, {0, 0, 1}, {1, 0, 1}};
		::ced::STriangle3<float>								triangleB			= {{1, 0, 0}, {0, 0, 0}, {0, 0, 1}};
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

int													setup				(SApplication & app)	{
	::ced::SWindow											& window			= app.Window;
	::ced::windowSetup(window);
	const uint32_t											pixelCount			= window.Size.x * window.Size.y;
	app.Pixels											= (::ced::SColorBGRA*)malloc(sizeof(::ced::SColorBGRA) * pixelCount);
	app.DepthBuffer.resize(pixelCount);
	//::geometryBuildCube(app.Geometry);
	::geometryBuildGrid(app.Geometry, {16U, 3U}, {1U, 1U});

	app.Models.resize(10);
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		SModel3												& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		model.Rotation										= {0, 1, 0};
		model.Position										= {4, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / app.Models.size() * iModel);
	}
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SWindow											& window			= app.Window;
	double													lastFrameSeconds	= app.Timer.Tick() * .000001;
	app.TotalTime										+= lastFrameSeconds;
	if(1 == ::ced::windowUpdate(window, app.Pixels))
		return 1;
	if(window.Resized) {
		free(app.Pixels);
		const uint32_t											pixelCount			= window.Size.x * window.Size.y;
		app.Pixels											= (::ced::SColorBGRA*)malloc(sizeof(::ced::SColorBGRA) * pixelCount);
		app.DepthBuffer.resize(pixelCount);
	}
	::ced::view_grid<::ced::SColorBGRA>							targetPixels		= {app.Pixels, window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColorBGRA) * targetPixels.size());
	memset(app.DepthBuffer.begin(), -1, sizeof(::ced::SColorBGRA) * app.DepthBuffer.size());

	//------------------------------------------- Handle input
	::ced::SCoord3<float>									cameraTarget		= {};
	static ::ced::SCoord3<float>							cameraPosition		= {15, 5, 0};
	::ced::SCoord3<float>									cameraUp			= {0, 1, 0};

	if(GetAsyncKeyState('Q')) cameraPosition.y					-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) cameraPosition.y					+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	//------------------------------------------- Transform and Draw
	static ::ced::SCoord3<float>							lightVector			= {15, 12, 0};
	lightVector											= lightVector	.RotateY(lastFrameSeconds * 4);
	cameraPosition										= cameraPosition.RotateY(lastFrameSeconds);

	lightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(cameraPosition, cameraTarget, cameraUp);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics());
	matrixView											= matrixView * matrixProjection;
	matrixView											= matrixView * matrixViewport;

	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		::ced::SMatrix4<float>									matrixScale		;
		::ced::SMatrix4<float>									matrixRotation	;
		::ced::SMatrix4<float>									matrixPosition	;
		matrixScale		.Scale			(app.Models[iModel].Scale	, true);
		matrixRotation	.Rotation		(app.Models[iModel].Rotation);
		matrixPosition	.SetTranslation	(app.Models[iModel].Position, true);

		::ced::SMatrix4<float>									matrixTransform		= matrixScale * matrixPosition * matrixRotation * matrixView;
		::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
		::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights;
		for(uint32_t iTriangle = 0; iTriangle < app.Geometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::STriangle3	<float>								triangle			= app.Geometry.Triangles	[iTriangle];
			::ced::SCoord3		<float>								normal				= app.Geometry.Normals	[iTriangle / 2];
			triangle.A											= matrixTransform.Transform(triangle.A);
			triangle.B											= matrixTransform.Transform(triangle.B);
			triangle.C											= matrixTransform.Transform(triangle.C);

			if(triangle.A.z < 0 || triangle.A.z >= 1) continue;
			if(triangle.B.z < 0 || triangle.B.z >= 1) continue;
			if(triangle.C.z < 0 || triangle.C.z >= 1) continue;

			double													lightFactor			= normal.Dot(lightVector);
			uint32_t												colorIndex			= (uint32_t)iTriangle % ::std::size(app.Colors);
			::ced::drawTriangle(targetPixels.metrics(), triangle, pixelCoords, pixelVertexWeights, {app.DepthBuffer.begin(), app.Window.Size});
			for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
				::ced::SCoord2<int32_t>									pixelCoord			= pixelCoords[iPixelCoord];
				::ced::SColorBGRA											pixelColor			= app.Colors[colorIndex];
				pixelColor.r										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].A);
				pixelColor.g										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].B);
				pixelColor.b										= (uint8_t)(0xFF * pixelVertexWeights[iPixelCoord].C);
				::ced::setPixel(targetPixels, pixelCoord, pixelColor * lightFactor);
			}
		}
	}
	return app.Running ? 0 : 1;
}

int	WINAPI											WinMain
	(	HINSTANCE	// hInstance
	,	HINSTANCE	// hPrevInstance
	,	LPSTR		// lpCmdLine
	,	INT			// nShowCmd
	) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
	SApplication										app;
	::setup(app);
	while(app.Running) {
		if(1 == ::update(app))
			break;
	}
	::cleanup(app);
	return 0;
}
