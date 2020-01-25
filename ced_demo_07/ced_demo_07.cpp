#include "ced_model.h"
#include "ced_geometry.h"
#include "ced_image.h"
#include "ced_draw.h"

#include "ced_framework.h"

#include <cstdint>
#include <algorithm>

struct SApplication {
	::ced::SFramework									Framework;

	::ced::SImage										Image				= {};
	::ced::container<::ced::SModel3D>					Models				= {};
	::ced::container<::ced::SEntity>					Entities			= {};
	::ced::SGeometryTriangles							Geometry			= {};
};

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }
int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);

	//::ced::geometryBuildCube(app.Geometry);
	//::ced::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Geometry, 4U, 2U, 1, {0, 1});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});
	app.Models		.resize(7);
	app.Entities	.resize(7);
	app.Models		[0]									= {};
	app.Models		[0].Scale							= {1, 1, 1};
	app.Models		[0].Rotation.z						= (float)(::ced::MATH_PI_2);
	app.Entities	[0]									= {-1};
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		model.Position										= {4, 0.5};
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

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running = false;
	//------------------------------------------- Handle input
	double													speed				= 10;
	double													lastFrameSeconds	= framework.Timer.ElapsedMicroseconds * .000001;

	//------------------------------------------- Handle input
	::ced::SCoord3<float>									cameraTarget		= {};
	static ::ced::SCoord3<float>							cameraPosition		= {0.000001f, 50, 0};
	::ced::SCoord3<float>									cameraUp			= {0, 1, 0};

	if(GetAsyncKeyState('Q')) cameraPosition.y				-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) cameraPosition.y				+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) app.Models[0].Position.x		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('S')) app.Models[0].Position.x		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('A')) app.Models[0].Position.z		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('D')) app.Models[0].Position.z		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	if(GetAsyncKeyState(VK_NUMPAD8)) app.Models[0].Rotation.x		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD2)) app.Models[0].Rotation.x		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD6)) app.Models[0].Rotation.z		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD4)) app.Models[0].Rotation.z		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	app.Models[0].Rotation.y							+= (float)lastFrameSeconds;
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel)
		app.Models[iModel].Rotation.y						+= (float)lastFrameSeconds * 5;

	static ::ced::SCoord3<float>							lightVector			= {15, 12, 0};
	lightVector											= lightVector.RotateY(lastFrameSeconds * 2);
	//------------------------------------------- Transform and Draw
	::ced::view_grid<::ced::SColorBGRA>							targetPixels		= {framework.Pixels, framework.Window.Size};

	lightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(cameraPosition, cameraTarget, cameraUp);
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
		matricesParent.Rotation.Rotation		(app.Models[entity.Parent].Rotation);
		matricesParent.Position.SetTranslation	(app.Models[entity.Parent].Position, true);

		::ced::SMatrix4<float>									matrixTransform			= matrices.Scale * matrices.Rotation * matrices.Position;
		::ced::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
		matrixTransform										= matrixTransform  * matrixTransformParent ;
		for(uint32_t iTriangle = 0; iTriangle < app.Geometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawTriangle(targetPixels, app.Geometry, iTriangle, matrixTransform, matrixView * matrixViewport, lightVector, pixelCoords, pixelVertexWeights, {app.Image.Pixels.begin(), app.Image.Metrics}, {framework.DepthBuffer.begin(), framework.Window.Size});
		}
	}
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
