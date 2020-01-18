#include "ced_model.h"
#include "ced_geometry.h"
#include "ced_image.h"
#include "ced_draw.h"

#include "ced_framework.h"

#include <cstdint>
#include <algorithm>



struct SApplication {
	::ced::SFramework									Framework			= {};

	::ced::SImage										Image				= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesLocal	= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesGlobal	= {};
	::ced::container<::ced::SModel3D>					Models				= {};
	::ced::container<::ced::SEntity>					Entities			= {};
	::ced::SGeometryTriangles							Geometry			= {};
	::ced::SCamera										Camera				= {};
	::ced::SCoord3<float>								LightVector			= {15, 12, 0};

	::ced::container<::ced::SCoord2<float>>				StarPosition		= {};
	::ced::container<float>								StarSpeed			= {};
	::ced::container<float>								StarBrightness		= {};
};

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }
int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);

	app.StarSpeed		.resize(128);
	app.StarBrightness	.resize(128);
	app.StarPosition	.resize(128);
	for(uint32_t iStar = 0; iStar < app.StarBrightness.size(); ++iStar) {
		app.StarSpeed		[iStar]						= float(16 + (rand() % 64));
		app.StarBrightness	[iStar]						= float(1.0 / 65535 * rand());
		app.StarPosition	[iStar].y					= float(rand() % framework.Window.Size.y);
		app.StarPosition	[iStar].x					= float(rand() % framework.Window.Size.x);
	}

	//::ced::geometryBuildCube(app.Geometry);
	//::ced::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Geometry, 4U, 2U, 1, {0, 1});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});
	app.ModelMatricesLocal	.resize(7);
	app.ModelMatricesGlobal	.resize(7);
	app.Models				.resize(7);
	app.Entities			.resize(7);
	app.Models		[0]									= {};
	app.Models		[0].Scale							= {1, 1, 1};
	app.Models		[0].Rotation.z						= (float)(::ced::MATH_PI_2);
	app.Entities	[0]									= {-1};
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		model.Position										= {2, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / (app.Models.size() - 1)* iModel);
		::ced::SEntity											& entity		= app.Entities[iModel];
		entity.Parent										= 0;
 		app.Entities[0].Children.push_back(iModel);
	}

	app.Image.Metrics										= {256, 256};
	app.Image.Pixels.resize(app.Image.Metrics.x * app.Image.Metrics.y);
	for(uint32_t y = 0; y < app.Image.Metrics.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < app.Image.Metrics.x; ++x) {
		app.Image.Pixels[y * app.Image.Metrics.x + x] = {(uint8_t)rand(), (uint8_t)rand(), (uint8_t)rand(), 0xFF};
	}

	return 0;
}

int													draw				(SApplication & app)	{
	//------------------------------------------- Transform and Draw
	::ced::SFramework										& framework			= app.Framework;
	::ced::view_grid<::ced::SColor>							targetPixels		= {framework.Pixels, framework.Window.Size};
	if(0 == targetPixels.size())
		return 1;
	::ced::SColor											colorBackground		= {0x20, 0x8, 0x4};
	::ced::SColor											colorStar			= {0xfF, 0xfF, 0xfF};
	//colorBackground										+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < framework.Window.Size.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < framework.Window.Size.x; ++x)
		framework.Pixels[y * framework.Window.Size.x + x]	= colorBackground;

	for(uint32_t iStar = 0; iStar < app.StarBrightness.size(); ++iStar) {
		::ced::SCoord2<float>								starPos			= app.StarPosition[iStar];
		::ced::SColor										starFinalColor	= colorStar * app.StarBrightness[iStar];
		::ced::setPixel(targetPixels, starPos.Cast<int32_t>(), starFinalColor);
		const	int32_t										brightRadius	= 5;
		double												brightUnit		= 1.0 / brightRadius;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::ced::SCoord2<float>								brightPos		= {(float)x, (float)y};
			const double										brightDistance	= brightPos.Length();
			if(brightDistance <= brightRadius) {
				::ced::SCoord2<int32_t>								pixelPos		= (starPos + brightPos).Cast<int32_t>();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
					)
					::ced::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + colorStar * app.StarBrightness[iStar] * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}

	}

	app.LightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(app.Camera.Position, app.Camera.Target, app.Camera.Up);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics(), 0.01, 1000);
	matrixView											= matrixView * matrixProjection;
	matrixViewport										= matrixViewport.GetInverse();
	matrixViewport._41									+= targetPixels.metrics().x / 2;
	matrixViewport._42									+= targetPixels.metrics().y / 2;

	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights;
	::ced::SModelTransform									matrices;
	::ced::SModelTransform									matricesParent;
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel) {
		matrices.Scale		.Scale			(app.Models[iModel].Scale	, true);
		matrices.Rotation	.Rotation		(app.Models[iModel].Rotation);
		matrices.Position	.SetTranslation	(app.Models[iModel].Position, true);

		::ced::SEntity											& entity				= app.Entities[iModel];
		matricesParent.Scale	.Scale			(app.Models[entity.Parent].Scale, true);
		matricesParent.Rotation	.Rotation		(app.Models[entity.Parent].Rotation);
		matricesParent.Position	.SetTranslation	(app.Models[entity.Parent].Position, true);

		::ced::SMatrix4<float>									matrixTransform			= matrices.Scale * matrices.Rotation * matrices.Position;
		::ced::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
		matrixTransform										= matrixTransform  * matrixTransformParent ;
		for(uint32_t iTriangle = 0; iTriangle < app.Geometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawTriangle(targetPixels, app.Geometry, iTriangle, matrixTransform, matrixView, matrixViewport, app.LightVector, pixelCoords, pixelVertexWeights, {app.Image.Pixels.begin(), app.Image.Metrics}, framework.DepthBuffer);
		}
	}
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running = false;
	//------------------------------------------- Handle input
	double													speed				= 10;
	double													lastFrameSeconds	= framework.Timer.ElapsedMicroseconds * .000001;
	if(GetAsyncKeyState('Q')) app.Camera.Position.y				-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.Camera.Position.y				+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) app.Models[0].Position.x			-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('S')) app.Models[0].Position.x			+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('A')) app.Models[0].Position.z			-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('D')) app.Models[0].Position.z			+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	if(GetAsyncKeyState(VK_NUMPAD8)) app.Models[0].Rotation.x	-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD2)) app.Models[0].Rotation.x	+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD6)) app.Models[0].Rotation.z	-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD4)) app.Models[0].Rotation.z	+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	app.Models[0].Rotation.y							+= (float)lastFrameSeconds;
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel)
		app.Models[iModel].Rotation.y						+= (float)lastFrameSeconds * 5;

	app.LightVector										= app.LightVector.RotateY(lastFrameSeconds * 2);

	if(framework.Window.Resized) {
		if(framework.Window.Size.x)
			for(uint32_t iStar = 0; iStar < app.StarBrightness.size(); ++iStar) {
				::ced::SCoord2<float>								 & starPos			= app.StarPosition[iStar];
				starPos.x										= (float)(rand() % framework.Window.Size.x);
				starPos.y										= (float)(rand() % framework.Window.Size.y);
			}
	}
	for(uint32_t iStar = 0; iStar < app.StarBrightness.size(); ++iStar) {
		::ced::SCoord2<float>								 & starPos			= app.StarPosition[iStar];
		starPos.y										+= float(app.StarSpeed[iStar] * lastFrameSeconds);
		app.StarBrightness	[iStar]						= float(1.0 / 65535 * rand());
		if(starPos.y >= framework.Window.Size.y) {
			app.StarSpeed		[iStar]						= float(16 + (rand() % 64));
			starPos.y										= 0;
		}
	}

	draw(app);
	return framework.Running ? 0 : 1;
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
	while(app.Framework.Running) {
		if(1 == ::update(app))
			break;
	}
	::cleanup(app);
	return 0;
}
