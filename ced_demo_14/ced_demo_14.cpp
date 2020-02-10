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
	::ced::SWindow										Window					= {};
	::ced::SColorBGRA									* Pixels				= 0;
	::ced::STimer										Timer					= {};
	bool												Running					= true;
	double												TotalTime				= 0;
	static constexpr	const ::ced::SColorBGRA			Colors		[]			= { ::ced::BROWN, ::ced::PURPLE, ::ced::TURQUOISE, ::ced::ORANGE, ::ced::PANOCHE, ::ced::RED, ::ced::GREEN, ::ced::BLUE, ::ced::YELLOW, ::ced::CYAN };

	::ced::SCoord3<float>								LightVector0			= {-15, 12, -15};
	::ced::SCoord3<float>								LightVector1			= { 0, 3, -5};
	::ced::SCoord3<float>								LightVector2			= { 0, 3, 5};

	::ced::SCoord3<float>								CameraTarget			= {-0,   0, -42.5000f};
	::ced::SCoord3<float>								CameraPosition			= {-0, 148, -42.50001f};
	::ced::SCoord3<float>								CameraUp				= {0, 1, 0};
	float												CameraDistance			= 1;

	::ced::container<::ced::SCoord2<int32_t>>			PixelCoords				= {};
	::ced::container<::ced::STriangleWeights<float>>	PixelVertexWeights		= {};

	::ced::container<::SModel3>							Models					= {};
	::ced::container<uint32_t>							DepthBuffer				= {};
	::ced::SGeometryQuads								GeometryFont			= {};
	::ced::SGeometryQuads								GeometryLetters	[256]	= {};

	static constexpr	const ::ced::SCoord2<uint32_t>	MetricsLetter			= {12, 12};
	static constexpr	const ::ced::SCoord2<uint32_t>	MetricsMap				= {16, 16};
	static constexpr	const ::ced::SCoord3<float>		FontMapTranslation		= {0, 100, -50};


	static constexpr const uint8_t						MapToDraw[][16]			=
		//{ {'\0', '\0', '\0', '\0', 0xDA, 0xC4,  0xC4,  0xC4,  0xC4,  0xC4, 0xC4, 0xBF, '\0', '\0', '\0', '\0',  }
		//, {'\0', '\0', '\0', '\0', 0xB3, '\0',  'H',  'o',  'L',  'A', '\0', 0xB3, '\0', '\0', '\0', '\0',  }
		//, {'\0', '\0', '\0', '\0', 0xB3,  'A',  'm',  'I',  'g',  '0',  's', 0xB3, '\0', '\0', '\0', '\0',  }
		//, {'\0', '\0', '\0', '\0', 0xC0, 0xC4,  0xC4,  0xC4,  0xC4,  0xC4, 0xC4, 0xD9, '\0', '\0', '\0', '\0',  }
		{ {0xDA, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xBF}
		//, {0xB3,  'H',  'e'	,  'a'	,  'l'	,  't'	,  'h'	,  ':'	,  '5'	, '\0'	, '\0'	, '\0'	, '\0'	, 0		, 0		, 0xB3}
		//, {0xC3, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xB4}
		, {0xB3,  'S',  'c'	,  'o'	,  'r'	,  'e'	, '\0'	,  ':'	,  '2'	,  '4'	, '\0'	, '\0'	, '\0'	, 0		, 0		, 0xB3}
		, {0xC3, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xB4}
		, {0xB3,  'T',  'i'	,  'm'	,  'e'	,  '\0'	, '\0'	,  ':'	,  '0'	,  '1'	,  ':'	,  '3'	,  '7'	, '.'	, '1'	, 0xB3}
		, {0xC0, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xD9}
		};

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
	const uint32_t											imagePitch			= app.MetricsLetter.x * app.MetricsMap.x;
	::ced::geometryBuildTileListFromImage(image, tiles);
	::ced::geometryBuildGridFromTileList(app.GeometryFont, ::ced::view_grid<::ced::STile>{tiles.begin(), image.Metrics}, {image.Metrics.x / 2.0f * .1f, image.Metrics.y / 2.0f * -.1f}, {.1f, .5f, .1f});
	::ced::view_grid<::ced::SGeometryQuads>					viewGeometries		= {app.GeometryLetters, {16, 16}};
	for(uint32_t y = 0; y < app.MetricsMap.y; ++y)
	for(uint32_t x = 0; x < app.MetricsMap.x; ++x) {
		tiles.clear();
		const uint32_t											asciiCode			= y * app.MetricsMap.x + x;
		const ::ced::SCoord2<uint32_t>							asciiCoords			= {asciiCode % app.MetricsMap.x, asciiCode / app.MetricsMap.x};
		const uint32_t											offsetPixelCoord	= (asciiCoords.y * app.MetricsLetter.y) * imagePitch + (asciiCoords.x * app.MetricsLetter.x);
		::ced::geometryBuildTileListFromImage({&image.Pixels[offsetPixelCoord], app.MetricsLetter}, tiles, app.MetricsLetter.x * app.MetricsMap.x);
		::ced::geometryBuildGridFromTileList(app.GeometryLetters[asciiCode], ::ced::view_grid<::ced::STile>{tiles.begin(), app.MetricsLetter}, {image.Metrics.x / 2.0f * .2f, image.Metrics.y / 2.0f * -.2f}, {.2f, 1.5f, .2f});
	}

	app.Models.resize(1 + 16 * 16);
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		SModel3												& model			= app.Models[iModel]	= {};
		model.Scale										= {1, 1, 1};
	}

	//for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel) {
	//	SModel3												& model			= app.Models[iModel]	= {};
	//	model.Position									= {(256.0f + (iModel % 16)) * .1f, (256.0f + iModel / 16.0f) * .1f};
	//}
	return 0;
}

int													update						(SApplication & app)	{
	::ced::SWindow											& window					= app.Window;
	const double											secondsLastFrameOriginal	= app.Timer.Tick() * .000001;
	double													secondsLastFrame			= secondsLastFrameOriginal;;//::std::min(secondsLastFrameOriginal, .05);
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
	if(GetAsyncKeyState('Q')) app.CameraPosition.y			-= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2);
	if(GetAsyncKeyState('E')) app.CameraPosition.y			+= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2);
	if(GetAsyncKeyState('W')) { app.CameraDistance -= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2); app.CameraPosition = app.CameraPosition / app.CameraPosition.Length() * (1 + app.CameraDistance); };
	if(GetAsyncKeyState('S')) { app.CameraDistance += (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2); app.CameraPosition = app.CameraPosition / app.CameraPosition.Length() * (1 + app.CameraDistance); };
	//

	//------------------------------------------- Transform and Draw
	app.LightVector0.Normalize();
	//app.LightVector0.RotateY(secondsLastFrame * 2);
	app.LightVector1.RotateY(secondsLastFrame * 2);
	app.LightVector2.RotateY(-secondsLastFrame * .5);
	//app.CameraPosition.RotateY(secondsLastFrame * .2);

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.Identity();
	matrixProjection.Identity();
	matrixViewport.Identity();
	matrixView.LookAt(app.CameraPosition, app.CameraTarget, app.CameraUp);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics());
	//matrixViewport._41									+= app.Window.Size.x / 4.0f;
	//matrixViewport._42									-= app.Window.Size.y / 3.375f;
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	::ced::container<::ced::SCoord3<float>>					lightPointsModel		;
	::ced::container<::ced::SColorBGRA>						lightColorsModel		;
	lightPointsModel.push_back(app.LightVector1 + app.FontMapTranslation);
	lightColorsModel.push_back(::ced::PURPLE);

	lightPointsModel.push_back(app.LightVector2 + app.FontMapTranslation);
	lightColorsModel.push_back(::ced::ORANGE);

	::ced::SMatrix4<float>									matrixScale			;
	::ced::SMatrix4<float>									matrixPosition		;
	::ced::SMatrix4<float>									matrixTransform		;
	::ced::SMatrix4<float>									matrixTransformView	;
	::ced::SMatrix4<float>									matrixRotation		;
	matrixScale		.Scale			(app.Models[0].Scale	, true);
	matrixRotation	.Rotation		(app.Models[0].Rotation);
	matrixPosition	.SetTranslation	(app.FontMapTranslation, true);
	matrixTransform										= matrixScale * matrixRotation * matrixPosition;
	matrixTransformView									= matrixTransform * matrixView;
	for(uint32_t iTriangle = 0; iTriangle < app.GeometryFont.Triangles.size(); ++iTriangle) {
		app.PixelCoords			.clear();
		app.PixelVertexWeights	.clear();
		uint32_t												colorIndex			= 0;
		::ced::SColorBGRA										triangleColor		= app.Colors[colorIndex];
		::ced::drawQuadTriangle(targetPixels, app.GeometryFont, iTriangle, matrixTransform, matrixTransformView, app.LightVector0, app.PixelCoords, app.PixelVertexWeights, {}, lightPointsModel, lightColorsModel, depthBuffer);
	}
	::ced::view_grid<::ced::SGeometryQuads>					viewGeometries		= {app.GeometryLetters, {16, 16}};
	uint32_t												colorIndex			= 0;
	for(uint32_t y = 0; y < ::std::size(app.MapToDraw); ++y)
	for(uint32_t x = 0; x < ::std::size(app.MapToDraw[0]); ++x) {
		const uint32_t											asciiCode			= app.MapToDraw[y][x];
		if(0 == asciiCode)
			continue;
		matrixScale		.Scale			({1, 1, 1}, true);
		::ced::SCoord3<float>									translation			= {};
		translation.x										= float(x * app.MetricsLetter.x);
		translation.z										= float(y * app.MetricsLetter.y);// + (12 * ::std::size(app.MapToDraw) / 2) + 12;
		matrixPosition	.SetTranslation	({translation.x * .2f, 0, translation.z * -.2f}, true);
		matrixTransform										= matrixScale * matrixPosition;
		matrixTransformView									= matrixTransform * matrixView;
		const ::ced::SCoord2<uint32_t>							asciiCoords			= {asciiCode % app.MetricsMap.x, asciiCode / app.MetricsMap.x};
		::ced::SGeometryQuads									& selectedGeometry	= viewGeometries[asciiCoords.y][asciiCoords.x];
		for(uint32_t iTriangle = 0; iTriangle < selectedGeometry.Triangles.size(); ++iTriangle) {
			app.PixelCoords			.clear();
			app.PixelVertexWeights	.clear();
			::ced::SColorBGRA										triangleColor		= app.Colors[int(app.TotalTime * 5 + x) % ::std::size(app.Colors)];
			::ced::drawQuadTriangle(targetPixels, selectedGeometry, iTriangle, matrixTransform, matrixTransformView, app.LightVector0, triangleColor, app.PixelCoords, app.PixelVertexWeights, depthBuffer);
			//::ced::drawQuadTriangle(targetPixels, selectedGeometry, iTriangle, matrixTransform, matrixTransformView, app.LightVector0, app.PixelCoords, app.PixelVertexWeights, {}, lightPointsModel, lightColorsModel, depthBuffer);
		}
		++colorIndex;
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
