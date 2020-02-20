#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"
#include "ced_matrix.h"
#include "ced_geometry.h"
#include "ced_image.h"

#include <cstdint>
#include <cstring>

struct SModel3 {
	::ced::SCoord3<float>								Scale;
	::ced::SCoord3<float>								Rotation;
	::ced::SCoord3<float>								Position;
};

struct SDrawCache {
	::ced::container<::ced::SCoord2<int32_t>>			PixelCoords				= {};
	::ced::container<::ced::STriangleWeights<float>>	PixelVertexWeights		= {};
	::ced::container<::ced::SCoord3<float>>				LightPointsModel		= {};
	::ced::container<::ced::SColorBGRA>					LightColorsModel		= {};
};

struct SApplication {
	::ced::SWindow										Window					= {};
	::ced::SColorBGRA									* Pixels				= 0;
	::ced::STimer										Timer					= {};
	bool												Running					= true;
	double												TotalTime				= 0;
	static constexpr	const ::ced::SColorBGRA			Colors		[]			= { ::ced::BROWN, ::ced::PURPLE, ::ced::TURQUOISE, ::ced::ORANGE, ::ced::PANOCHE, ::ced::RED, ::ced::GREEN, ::ced::BLUE, ::ced::YELLOW, ::ced::CYAN };

	::ced::SCoord3<float>								LightVector0			= {-15, 50, -15};
	::ced::SCoord3<float>								LightPosition0			= { 0, 5, -7};
	::ced::SCoord3<float>								LightPosition1			= { 0, 5, 7};
	::ced::SCoord3<float>								LightPosition2			= { 0, 5, 7};

	::ced::SCoord3<float>								CameraTarget			= {0, 0, 0};
	::ced::SCoord3<float>								CameraPosition			= {.0001f, 650.1f, -0.00001f};
	::ced::SCoord3<float>								CameraUp				= {0, 0, 1};
	float												CameraDistance			= 600;

	::SDrawCache										DrawCache;
	::ced::SImage										FontImage;

	::ced::container<::SModel3>							Models					= {};
	::ced::container<uint32_t>							DepthBuffer				= {};
	::ced::SGeometryQuads								GeometryFont			= {};
	::ced::SGeometryQuads								GeometryLetters	[256]	= {};


	static constexpr	const ::ced::SCoord2<uint32_t>	MetricsLetter			= {12, 12};
	static constexpr	const ::ced::SCoord2<uint32_t>	MetricsMap				= {16, 16};
	static constexpr	const ::ced::SCoord3<float>		FontMapTranslation		= {0, 0, -50};
	static constexpr	const ::ced::SCoord3<float>		ControlTranslation		= {0, 0, 20.0f};


	uint8_t												MapToDraw[5][32]			=
		{ {0xDA, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xBF}
		, {0xB3, '\0', '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	,  '\0',  '\0',  '\0'	, '\0'	, '\0'	, '\0'	,  '\0'	,  '\0'	,  '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
		, {0xC3, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xB4}
		, {0xB3,  'T',  'i'	,  'm'	,  'e'	,  ':'	,  '0'	,  '0'	,  ':'	,  '0'	,  '0'	, ':'	, '0'	, '0'	, '.'	, '0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0'	, '\0',  '\0'	, '\0'	, '\0'	, '\0'	, 0xB3}
		, {0xC0, 0xC4,  0xC4,  0xC4	,  0xC4	,  0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xC4	, 0xD9}
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
	::ced::container<::ced::STile>							tiles;

	::ced::bmpFileLoad("../ced_data/cp437_12x12.bmp", app.FontImage, true);
	const uint32_t											imagePitch			= app.MetricsLetter.x * app.MetricsMap.x;
	::ced::geometryBuildTileListFromImage(app.FontImage, tiles);
	::ced::geometryBuildGridFromTileList(app.GeometryFont, ::ced::view_grid<::ced::STile>{tiles.begin(), app.FontImage.Metrics}, {app.FontImage.Metrics.x / 2.0f * .1f, app.FontImage.Metrics.y / 2.0f * -.1f}, {1.f, 5.f, 1.f});
	::ced::view_grid<::ced::SGeometryQuads>					viewGeometries		= {app.GeometryLetters, {16, 16}};
	for(uint32_t y = 0; y < app.MetricsMap.y; ++y)
	for(uint32_t x = 0; x < app.MetricsMap.x; ++x) {
		tiles.clear();
		const uint32_t											asciiCode			= y * app.MetricsMap.x + x;
		const ::ced::SCoord2<uint32_t>							asciiCoords			= {asciiCode % app.MetricsMap.x, asciiCode / app.MetricsMap.x};
		const uint32_t											offsetPixelCoord	= (asciiCoords.y * app.MetricsLetter.y) * imagePitch + (asciiCoords.x * app.MetricsLetter.x);
		::ced::geometryBuildTileListFromImage({&app.FontImage.Pixels[offsetPixelCoord], app.MetricsLetter}, tiles, app.MetricsLetter.x * app.MetricsMap.x);
		::ced::geometryBuildGridFromTileList(app.GeometryLetters[asciiCode], ::ced::view_grid<::ced::STile>{tiles.begin(), app.MetricsLetter}, {}, {1, 12.0f, 1});
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
	app.TotalTime	= 3590.5;
	return 0;
}

static	int											drawPixels
	( ::ced::view_grid<::ced::SColorBGRA>				targetPixels
	, const ::ced::STriangle3	<float>					& triangleWorld
	, const ::ced::SCoord3		<float>					& normal
	, const ::ced::SCoord3		<float>					& lightVector
	, const ::ced::SColorFloat							& texelColor
	, ::ced::container<::ced::SCoord2<int32_t>>			& pixelCoords
	, ::ced::container<::ced::STriangleWeights<float>>	& pixelVertexWeights
	, double											timeAnimation
	) {
	double													lightFactorDirectional	= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>									pixelCoord				= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<float>					& vertexWeights			= pixelVertexWeights[iPixelCoord];
		const ::ced::SCoord3<float>								position				= ::ced::triangleWeight(vertexWeights, triangleWorld);
		double													factorWave				= (::std::max(0.0, sin(- timeAnimation * 4 + position.y * .75))) * .6;
		double													factorWave2				= (::std::max(0.0, sin(- timeAnimation + position.x * .0125 + position.z * .125))) * .5;
		::ced::setPixel(targetPixels, pixelCoord, texelColor * (lightFactorDirectional * 2) + texelColor * factorWave + texelColor * factorWave2);
	}
	return 0;
}

int													draw3DCharacter
	( const ::ced::view_grid<::ced::SColorBGRA>				& targetPixels
	, const ::ced::SCoord2<uint32_t>						& metricsCharacter
	, const ::ced::SCoord2<uint32_t>						& metricsMap
	, const uint8_t											asciiCode
	, const ::ced::SCoord3<float>							& position
	, const ::ced::SCoord3<float>							& lightVector
	, const ::ced::SMatrix4<float>							& matrixView
	, const ::ced::view_grid<const ::ced::SGeometryQuads>	& viewGeometries
	, ::SDrawCache											& drawCache
	, ::ced::view_grid<uint32_t>							& depthBuffer
	, double												timeAnimation
	)	{
	::ced::SMatrix4<float>									matrixScale				;
	::ced::SMatrix4<float>									matrixPosition			;
	::ced::SCoord3<float>									translation				= {};
	translation.x										= float(position.x * metricsCharacter.x);
	translation.z										= float(position.z * metricsCharacter.y);
	matrixPosition	.SetTranslation	({translation.x, 0, translation.z}, true);
	matrixScale		.Scale			({1, 1, 1}, true);
	::ced::SMatrix4<float>									matrixTransform										= matrixScale * matrixPosition;
	::ced::SMatrix4<float>									matrixTransformView									= matrixTransform * matrixView;
	const ::ced::SCoord2<uint32_t>							asciiCoords				= {asciiCode % metricsMap.x, asciiCode / metricsMap.x};
	const ::ced::SGeometryQuads								& geometry				= viewGeometries[asciiCoords.y][asciiCoords.x];
	::ced::SColorFloat										color					= ::ced::PANOCHE;//::ced::COLOR_TABLE[((int)timeAnimation) % ::ced::size(::ced::COLOR_TABLE)];
	for(uint32_t iTriangle = 0; iTriangle < geometry.Triangles.size(); ++iTriangle) {
		drawCache.PixelCoords			.clear();
		drawCache.PixelVertexWeights	.clear();
		::ced::STriangle3		<float>							triangle			= geometry.Triangles	[iTriangle];;
		const ::ced::SCoord3	<float>							& normal			= geometry.Normals		[iTriangle / 2];
		::ced::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
		::ced::SCoord3	<float>									xnormal				= matrixTransform.TransformDirection(normal).Normalize();
		::ced::STriangle3		<float>							triangleWorld		= triangle;
		::ced::transform(triangleWorld, matrixTransform);
		::drawPixels(targetPixels, triangleWorld, xnormal, lightVector, color * .75, drawCache.PixelCoords, drawCache.PixelVertexWeights, timeAnimation);
	}
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
	if(GetAsyncKeyState('Q')) app.CameraPosition.y -= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2);
	if(GetAsyncKeyState('E')) app.CameraPosition.y += (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2);
	if(GetAsyncKeyState('W')) { app.CameraDistance -= (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2); app.CameraPosition = app.CameraPosition / app.CameraPosition.Length() * (1 + app.CameraDistance); };
	if(GetAsyncKeyState('S')) { app.CameraDistance += (float)secondsLastFrameOriginal * (GetAsyncKeyState(VK_SHIFT) ? 20 : 2); app.CameraPosition = app.CameraPosition / app.CameraPosition.Length() * (1 + app.CameraDistance); };
	//

	//------------------------------------------- Transform and Draw
	app.LightVector0.Normalize();
	//app.LightVector0.RotateY(secondsLastFrame * 2);
	app.LightPosition0.RotateY(secondsLastFrame * 2);
	app.LightPosition1.RotateY(-secondsLastFrame * .5);
	//app.CameraPosition.RotateY(secondsLastFrame * .2);

	::ced::SMatrix4<float>									matrixView					= {};
	::ced::SMatrix4<float>									matrixProjection			= {};
	::ced::SMatrix4<float>									matrixViewport				= {};
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

	::ced::container<::ced::SCoord3<float>>					& lightPointsModel			= app.DrawCache.LightPointsModel;
	::ced::container<::ced::SColorBGRA>						& lightColorsModel			= app.DrawCache.LightColorsModel;
	lightPointsModel.clear();
	lightColorsModel.clear();
	lightPointsModel.push_back((app.LightPosition0) + ::ced::SCoord3<float>{0.f, 1.0f, 7.5f});
	lightColorsModel.push_back(::ced::PURPLE);

	lightPointsModel.push_back((app.LightPosition1) + ::ced::SCoord3<float>{0.f, 1.0f, 7.5f});
	lightColorsModel.push_back(::ced::ORANGE);

	app.LightPosition0.RotateY(secondsLastFrame * 2);
	app.LightPosition1.RotateY(-secondsLastFrame * .5);
	lightPointsModel.push_back(app.LightPosition0 + app.FontMapTranslation);
	lightColorsModel.push_back(::ced::PURPLE);

	lightPointsModel.push_back(app.LightPosition1 + app.FontMapTranslation);
	lightColorsModel.push_back(::ced::ORANGE);

	//::ced::SMatrix4<float>									matrixRotation		;
	//::ced::SMatrix4<float>									matrixScale			;
	//::ced::SMatrix4<float>									matrixPosition		;
	//matrixScale		.Scale			(app.Models[0].Scale	, true);
	//matrixRotation	.Rotation		(app.Models[0].Rotation);
	//matrixPosition	.SetTranslation	(app.FontMapTranslation, true);
	//::ced::SMatrix4<float>									matrixTransform										= matrixScale * matrixRotation * matrixPosition;
	//::ced::SMatrix4<float>									matrixTransformView									= matrixTransform * matrixView;
	//for(uint32_t iTriangle = 0; iTriangle < app.GeometryFont.Triangles.size(); ++iTriangle) {
	//	app.DrawCache.PixelCoords			.clear();
	//	app.DrawCache.PixelVertexWeights	.clear();
	//	uint32_t												colorIndex			= 0;
	//	::ced::SColorBGRA										triangleColor		= app.Colors[colorIndex];
	//	::ced::drawQuadTriangle(targetPixels, app.GeometryFont, iTriangle, matrixTransform, matrixTransformView, app.LightVector0, app.DrawCache.PixelCoords, app.DrawCache.PixelVertexWeights, {}, lightPointsModel, lightColorsModel, depthBuffer);
	//}


	::ced::view_grid<::ced::SGeometryQuads>					viewGeometries		= {app.GeometryLetters, {16, 16}};
	uint32_t												colorIndex			= 0;
	::ced::view_grid<uint8_t>								mapToDraw			= {app.MapToDraw[0], {::ced::size(app.MapToDraw[0]), ::ced::size(app.MapToDraw)}};
	uint32_t												timeHours			= (int)app.TotalTime / 3600;
	uint32_t												timeMinutes			= (int)app.TotalTime / 60 % 60;
	uint32_t												timeSeconds			= (int)app.TotalTime % 60;
	uint32_t												timeCents			= int(app.TotalTime * 10) % 10;

	char													strLevel		[17]	= {};
	char													strScore		[17]	= {};
	char													strTimeHours	[3]	= {};
	char													strTimeMinutes	[3]	= {};
	char													strTimeSeconds	[3]	= {};
	char													strTimeCents	[2]	= {};
	memset(&mapToDraw[1][1], 0, mapToDraw.metrics().x - 2);
	sprintf_s(strLevel		, "Level:%u", (uint32_t)(app.TotalTime - 3590)); memcpy(&mapToDraw[1][1], strLevel, strlen(strLevel));
	sprintf_s(strScore		, "Score:%u", (uint32_t)(app.TotalTime * 1000 - 3600 * 1000)); memcpy(&mapToDraw[1][mapToDraw.metrics().x - (uint32_t)strlen(strScore) - 1], strScore, strlen(strScore));
	sprintf_s(strTimeHours	, "%.2u" , timeHours	); memcpy(&mapToDraw[3][ 6], strTimeHours	, 2);
	sprintf_s(strTimeMinutes, "%.2u" , timeMinutes	); memcpy(&mapToDraw[3][ 9], strTimeMinutes	, 2);
	sprintf_s(strTimeSeconds, "%.2u" , timeSeconds	); memcpy(&mapToDraw[3][12], strTimeSeconds	, 2);
	sprintf_s(strTimeCents	, "%.1u" , timeCents	); memcpy(&mapToDraw[3][15], strTimeCents	, 1);
	::ced::SCoord3<float>									offset			= app.ControlTranslation;
	offset												-= ::ced::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};
	for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
	for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
		const uint8_t											asciiCode			= mapToDraw[y][x];
		if(0 == asciiCode)
			continue;
		::ced::SCoord3<float>									position			= offset;
		position.x											+= x;
		position.z											-= y;
		draw3DCharacter(targetPixels, app.MetricsLetter, app.MetricsMap, asciiCode, position, app.LightVector0, matrixView, viewGeometries, app.DrawCache, depthBuffer, app.TotalTime);
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
