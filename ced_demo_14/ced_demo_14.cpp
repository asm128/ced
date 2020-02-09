#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"
#include "ced_matrix.h"
#include "ced_geometry.h"
#include "ced_image.h"

#include <cstdint>
#include <algorithm>

struct SModel3 {
	::ced::SCoord3<float>								Scale;
	::ced::SCoord3<float>								Rotation;
	::ced::SCoord3<float>								Position;
};

struct SApplication {
	::ced::SWindow										Window				= {};
	::ced::SColorBGRA									* Pixels			= 0;
	::ced::STimer										Timer				= {};
	bool												Running				= true;
	double												TotalTime			= 0;
	::ced::SColorBGRA									Colors		[4]		= { {0xff, 0xFF, 0xFF}, {0, 0xFF, 0}, {0, 0, 0xFF}, {0xFF, 0xC0, 0x40} };

	::ced::SCoord3<float>								LightVector0		= {15, 12, 0};
	::ced::SCoord3<float>								LightVector1		= { 7, 2, 0};
	::ced::SCoord3<float>								LightVector2		= { 3, 2, 0};

	::ced::SCoord3<float>								CameraTarget		= {};
	::ced::SCoord3<float>								CameraPosition		= {15, 5, 0};
	::ced::SCoord3<float>								CameraUp			= {0, 1, 0};
	float												CameraDistance		= 1;

	::ced::container<::ced::SCoord2<int32_t>>			PixelCoords			= {};
	::ced::container<::ced::STriangleWeights<float>>	PixelVertexWeights	= {};

	::ced::container<::SModel3>							Models				= {};
	::ced::container<uint32_t>							DepthBuffer			= {};
	::ced::SGeometryQuads								Geometry			= {};
};

int													cleanup				(SApplication & app)	{
	free(app.Pixels);
	::ced::windowCleanup(app.Window);
	return 0;
}

int													setup				(SApplication & app)	{
	::ced::SWindow											& window			= app.Window;
	::ced::windowSetup(window);
	const uint32_t											pixelCount			= window.Size.x * window.Size.y;
	app.Pixels											= (::ced::SColorBGRA*)malloc(sizeof(::ced::SColorBGRA) * pixelCount);
	app.DepthBuffer.resize(pixelCount);
	::ced::SImage											image;
	::ced::container<::ced::STile>							tiles;

	::ced::bmpFileLoad("../ced_data/cp437_12x12.bmp", image, true);

	::ced::geometryBuildTileListFromImage(image, tiles);
	::ced::geometryBuildGridFromTileList(app.Geometry, ::ced::view_grid<::ced::STile>{tiles.begin(), image.Metrics}, {9.6f, -9.6f}, {.1f, .1f, .1f});
	//::ced::geometryBuildGridFromTileList(app.Geometry, ::ced::view_grid<::ced::STile>{tiles.begin(), image.Metrics}, {}, {.1f,-.1f, .1f});

	//::ced::geometryBuildTender(app.Geometry, 10U, 10U, 1, {});
	app.Models.resize(1);
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		SModel3												& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		//model.Position										= {4, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / app.Models.size() * iModel);
	}
	return 0;
}

int													update						(SApplication & app)	{
	::ced::SWindow											& window					= app.Window;
	const double											secondsLastFrameOriginal	= app.Timer.Tick() * .000001;
	double													secondsLastFrame			= ::std::min(secondsLastFrameOriginal, .05);
	app.TotalTime										+= secondsLastFrame;
	if(1 == ::ced::windowUpdate(window, app.Pixels))
		return 1;
	if(window.Resized) {
		free(app.Pixels);
		const uint32_t											pixelCount					= window.Size.x * window.Size.y;
		app.Pixels											= (::ced::SColorBGRA*)malloc(sizeof(::ced::SColorBGRA) * pixelCount);
		app.DepthBuffer.resize(pixelCount);
	}
	::ced::view_grid<::ced::SColorBGRA>						targetPixels				= {app.Pixels, window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColorBGRA) * targetPixels.size());
	::ced::view_grid<uint32_t>								depthBuffer					= {app.DepthBuffer.begin(), app.Window.Size};
	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());

	//------------------------------------------- Handle input
	if(GetAsyncKeyState('Q')) app.CameraPosition.y			-= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.CameraPosition.y			+= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('W')) app.CameraDistance			-= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('S')) app.CameraDistance			+= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	app.CameraPosition									= app.CameraPosition / app.CameraPosition.Length() * (1 + app.CameraDistance);

	//------------------------------------------- Transform and Draw
	app.LightVector0.Normalize();
	app.LightVector1.RotateY(secondsLastFrame * 2);
	app.LightVector2.RotateY(-secondsLastFrame * .5);
	app.CameraPosition									= app.CameraPosition.RotateY(secondsLastFrame * .2);

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(app.CameraPosition, app.CameraTarget, app.CameraUp);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	::ced::container<::ced::SCoord3<float>>					lightPointsModel		;
	::ced::container<::ced::SColorBGRA>						lightColorsModel		;
	lightPointsModel.push_back(app.LightVector1);
	lightColorsModel.push_back(::ced::PURPLE);

	lightPointsModel.push_back(app.LightVector2);
	lightColorsModel.push_back(::ced::ORANGE);

	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		::ced::SMatrix4<float>									matrixScale		;
		::ced::SMatrix4<float>									matrixRotation	;
		::ced::SMatrix4<float>									matrixPosition	;
		matrixScale		.Scale			(app.Models[iModel].Scale	, true);
		matrixRotation	.Rotation		(app.Models[iModel].Rotation);
		matrixPosition	.SetTranslation	(app.Models[iModel].Position, true);

		::ced::SMatrix4<float>									matrixTransform		= matrixScale * matrixRotation * matrixPosition;
		::ced::SMatrix4<float>									matrixTransformView	= matrixTransform * matrixView;
		for(uint32_t iTriangle = 0; iTriangle < app.Geometry.Triangles.size(); ++iTriangle) {
			app.PixelCoords			.clear();
			app.PixelVertexWeights	.clear();
			uint32_t												colorIndex			= (uint32_t)iModel % ::std::size(app.Colors);
			::ced::SColorBGRA										triangleColor		= app.Colors[colorIndex];
			::ced::drawQuadTriangle(targetPixels, app.Geometry, iTriangle, matrixTransform, matrixTransformView, app.LightVector0, app.PixelCoords, app.PixelVertexWeights, {}, lightPointsModel, lightColorsModel, depthBuffer);
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
