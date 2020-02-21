#include "ced_demo_13.h"
#include <time.h>
#include <process.h>

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }
int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);
	srand((uint32_t)time(0));
	::ssg::solarSystemSetup(app.SolarSystem, framework.Window.Size);
	framework.UseDoubleBuffer							= true;

	::ced::SImage											fontImage;
	::ced::bmpFileLoad("../ced_data/cp437_12x12.bmp", fontImage, true);
	::ced::view_grid<::ced::SGeometryQuads>					viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	const uint32_t											imagePitch			= app.TextOverlay.MetricsLetter.x * app.TextOverlay.MetricsMap.x;

	::ced::container<::ced::STile>							tiles;
	for(uint32_t y = 0; y < app.TextOverlay.MetricsMap.y; ++y)
	for(uint32_t x = 0; x < app.TextOverlay.MetricsMap.x; ++x) {
		tiles.clear();
		const uint32_t											asciiCode			= y * app.TextOverlay.MetricsMap.x + x;
		const ::ced::SCoord2<uint32_t>							asciiCoords			= {asciiCode % app.TextOverlay.MetricsMap.x, asciiCode / app.TextOverlay.MetricsMap.x};
		const uint32_t											offsetPixelCoord	= (asciiCoords.y * app.TextOverlay.MetricsLetter.y) * imagePitch + (asciiCoords.x * app.TextOverlay.MetricsLetter.x);
		::ced::geometryBuildTileListFromImage({&fontImage.Pixels[offsetPixelCoord], app.TextOverlay.MetricsLetter}, tiles, app.TextOverlay.MetricsLetter.x * app.TextOverlay.MetricsMap.x);
		::ced::geometryBuildGridFromTileList(app.TextOverlay.GeometryLetters[asciiCode], ::ced::view_grid<::ced::STile>{tiles.begin(), app.TextOverlay.MetricsLetter}, {}, {1, 12.0f, 1});
	}
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	{
		::std::lock_guard<::std::mutex>							lockUpdate			(app.SolarSystem.LockUpdate);
		if(1 == ::ced::frameworkUpdate(app.Framework))
			framework.Running									= false;
	}
	if(framework.Window.Resized) {
		::ced::SMatrix4<float>									& matrixProjection	= app.SolarSystem.Scene.MatrixProjection;
		matrixProjection.FieldOfView(::ced::MATH_PI * .25, framework.Window.Size.x / (double)framework.Window.Size.y, 0.01, 500);
		::ced::SMatrix4<float>									matrixViewport		= {};
		matrixViewport.Viewport(framework.Window.Size);
		matrixProjection									*= matrixViewport;

		{
			::std::lock_guard<::std::mutex>							lockUpdate			(app.SolarSystem.LockUpdate);
			::ssg::solarSystemSetupBackgroundImage(app.SolarSystem.BackgroundImage, framework.Window.Size);
			::ssg::setupStars(app.SolarSystem.Stars, framework.Window.Size);
		}
	}
	double													secondsLastFrame	= framework.Timer.ElapsedMicroseconds * .001;
	secondsLastFrame									*= .001;
	::ssg::solarSystemUpdate(app.SolarSystem, secondsLastFrame, framework.Window.Size);
	Sleep(1);
	return framework.Running ? 0 : 1;
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
	double													lightFactorDirectional		= normal.Dot(lightVector);
	(void)lightFactorDirectional;
	for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
		::ced::SCoord2<int32_t>									pixelCoord					= pixelCoords		[iPixelCoord];
		const ::ced::STriangleWeights<float>					& vertexWeights				= pixelVertexWeights[iPixelCoord];
		const ::ced::SCoord3<float>								position					= ::ced::triangleWeight(vertexWeights, triangleWorld);
		double													factorWave					= (::std::max(0.0, sin(- timeAnimation * 4 + position.y * .75))) * .6;
		double													factorWave2					= (::std::max(0.0, sin(- timeAnimation + position.x * .0125 + position.z * .125))) * .5;
		::ced::setPixel(targetPixels, pixelCoord, targetPixels[pixelCoord.y][pixelCoord.x] * .25 + (texelColor * (lightFactorDirectional * 2) + texelColor * factorWave + texelColor * factorWave2));
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
	, ::ssg::SSolarSystemDrawCache							& drawCache
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

int													draw					(SApplication & app) {
	::ced::view_grid<::ced::SColorBGRA>						targetPixels			= {app.Framework.DoubleBuffer[InterlockedIncrement64(&app.Framework.CurrentRenderBuffer) % 2].begin(), app.Framework.Window.Size};
	::ced::view_grid<uint32_t>								depthBuffer				= {app.Framework.DepthBuffer.begin(), app.Framework.Window.Size};
	::ssg::solarSystemDraw(app.SolarSystem, app.SolarSystem.DrawCache, app.SolarSystem.LockUpdate, targetPixels, depthBuffer);

	app.SolarSystem.DrawCache							= {};


	app.TextOverlay.LightVector0.Normalize();
	::ced::view_grid<::ced::SGeometryQuads>					viewGeometries		= {app.TextOverlay.GeometryLetters, {16, 16}};
	uint32_t												colorIndex			= 0;
	::ced::view_grid<uint8_t>								mapToDraw			= {app.MapToDraw[0], {::ced::size(app.MapToDraw[0]), ::ced::size(app.MapToDraw)}};
	uint32_t												timeHours			= (int)app.Framework.TotalTime / 3600;
	uint32_t												timeMinutes			= (int)app.Framework.TotalTime / 60 % 60;
	uint32_t												timeSeconds			= (int)app.Framework.TotalTime % 60;
	uint32_t												timeCents			= int(app.Framework.TotalTime * 10) % 10;

	char													strStage		[17]	= {};
	char													strScore		[17]	= {};
	char													strTimeHours	[3]	= {};
	char													strTimeMinutes	[3]	= {};
	char													strTimeSeconds	[3]	= {};
	char													strTimeCents	[2]	= {};
	memset(&mapToDraw[1][1], 0, mapToDraw.metrics().x - 2);
	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	sprintf_s(strStage		, "Stage:%u", (uint32_t)(app.SolarSystem.Stage)); memcpy(&mapToDraw[1][1], strStage, strlen(strStage));
	sprintf_s(strScore		, "Score:%u", (uint32_t)(app.SolarSystem.Score)); memcpy(&mapToDraw[1][mapToDraw.metrics().x - (uint32_t)strlen(strScore) - 1], strScore, strlen(strScore));
	sprintf_s(strTimeHours	, "%.2u" , timeHours	); memcpy(&mapToDraw[3][ 6], strTimeHours	, 2);
	sprintf_s(strTimeMinutes, "%.2u" , timeMinutes	); memcpy(&mapToDraw[3][ 9], strTimeMinutes	, 2);
	sprintf_s(strTimeSeconds, "%.2u" , timeSeconds	); memcpy(&mapToDraw[3][12], strTimeSeconds	, 2);
	sprintf_s(strTimeCents	, "%.1u" , timeCents	); memcpy(&mapToDraw[3][15], strTimeCents	, 1);
	::ced::SCoord3<float>									offset			= app.TextOverlay.ControlTranslation;
	offset												-= ::ced::SCoord3<float>{mapToDraw.metrics().x * .5f, 0, mapToDraw.metrics().y * .5f * -1.f};

	::ced::SMatrix4<float>									matrixView					= {};
	::ced::SMatrix4<float>									matrixProjection			= {};
	::ced::SMatrix4<float>									matrixViewport				= {};
	matrixView.Identity();
	matrixProjection.Identity();
	matrixViewport.Identity();
	matrixView.LookAt(app.TextOverlay.CameraPosition, app.TextOverlay.CameraTarget, app.TextOverlay.CameraUp);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	{
		::std::lock_guard<::std::mutex>							lockUpdate				(app.SolarSystem.LockUpdate);
		for(uint32_t y = 0; y < mapToDraw.metrics().y; ++y)
		for(uint32_t x = 0; x < mapToDraw.metrics().x; ++x) {
			const uint8_t											asciiCode			= mapToDraw[y][x];
			if(0 == asciiCode)
				continue;
			::ced::SCoord3<float>									position			= offset;
			position.x											+= x;
			position.z											-= y;
			draw3DCharacter(targetPixels, app.TextOverlay.MetricsLetter, app.TextOverlay.MetricsMap, asciiCode, position, app.TextOverlay.LightVector0, matrixView, viewGeometries, app.SolarSystem.DrawCache, depthBuffer, app.Framework.TotalTime);
			++colorIndex;
		}
	}
	return 0;
}

void												threadDraw				(void * pApp) {
	SApplication											& app					= *(SApplication*)pApp;
	while(1 != InterlockedCompareExchange64(&app.ThreadSignal, 0, 1)) {
		draw(app);
		Sleep(1);
	}
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
	if(1 == ::update(app))
		return -1;
	InterlockedIncrement64(&app.ThreadSignal);
	InterlockedIncrement64(&app.ThreadSignal);
	_beginthread(threadDraw, 0, &app);
	while(app.Framework.Running) {
		if(1 == ::update(app)) {
			break;
		}
		//::draw(app);
	}
	InterlockedDecrement64(&app.ThreadSignal);
	while(0 != InterlockedCompareExchange64(&app.ThreadSignal, 0, 0))
		Sleep(10);
	::cleanup(app);
	return 0;
}
