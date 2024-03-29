#include "ced_geometry.h"
#include "ced_image.h"
#include "ced_draw.h"

#include "ced_framework.h"
#include "ced_model.h"

#include <cstdint>
#include <algorithm>


struct SApplication {
	::ced::SFramework									Framework			= {};

	::ced::SImage										Image				= {};
	::ced::container<::ced::SModel3>					Models;
	::ced::SGeometryTriangles							Geometry;
};

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }
int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);

	//::ced::geometryBuildCube(app.Geometry);
	//::ced::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Geometry, 16U, 16U, 2, {});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});
	app.Models.resize(6);
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		::ced::SModel3											& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		model.Position										= {4, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / app.Models.size() * iModel);
	}

	::ced::SColorFloat										colors []		=
		{ ::ced::DARKRED
		, ::ced::ORANGE + ::ced::WHITE * .7
		, ::ced::GREEN
		, ::ced::BLUE
		, ::ced::ORANGE
		};

	app.Image.Metrics									= {24, 12};
	app.Image.Pixels.resize(app.Image.Metrics.x * app.Image.Metrics.y);
	for(uint32_t y = 0; y < app.Image.Metrics.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < app.Image.Metrics.x; ++x)
		app.Image.Pixels[y * app.Image.Metrics.x + x]		= colors[x % ::std::size(colors)];//{(uint8_t)(rand() % 32), (uint8_t)rand(), (uint8_t)(rand() % 32), 0xFF};

	return 0;
}

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running									= false;
	//------------------------------------------- Handle input
	double													secondsLastFrame	= framework.Timer.ElapsedMicroseconds * .000001;

	//------------------------------------------- Handle input
	::ced::SCoord3<float>									cameraTarget		= {};
	static ::ced::SCoord3<float>							cameraPosition		= {15, 5, 0};
	::ced::SCoord3<float>									cameraUp			= {0, 1, 0};

	if(GetAsyncKeyState('Q')) cameraPosition.y				-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) cameraPosition.y				+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	//------------------------------------------- Transform and Draw
	::ced::view_grid<::ced::SColorBGRA>						targetPixels		= {framework.Pixels.begin(), framework.Window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColorBGRA) * targetPixels.size());
	::ced::view_grid<uint32_t>								depthBuffer			= {framework.DepthBuffer.begin(), framework.Window.Size};
	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());

	static ::ced::SCoord3<float>							lightVector			= {15, 12, 0};
	lightVector											= lightVector	.RotateY(secondsLastFrame * 2);
	cameraPosition										= cameraPosition.RotateY(secondsLastFrame / 2);

	lightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(cameraPosition, cameraTarget, cameraUp);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics());
	matrixView											= matrixView * matrixProjection;
	matrixView											= matrixView * matrixViewport;

	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	::ced::container<::ced::STriangleWeights<float>>		pixelVertexWeights;
	::ced::SModelMatrices									matrices;
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		matrices.Scale		.Scale			(app.Models[iModel].Scale	, true);
		matrices.Rotation	.Rotation		(app.Models[iModel].Rotation);
		matrices.Position	.SetTranslation	(app.Models[iModel].Position, true);

		::ced::SMatrix4<float>									matrixTransform		= matrices.Scale * matrices.Rotation * matrices.Position;
		::ced::SMatrix4<float>									matrixTransformView	= matrixTransform * matrixView;
		for(uint32_t iTriangle = 0; iTriangle < app.Geometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawTriangle(targetPixels, app.Geometry, iTriangle, matrixTransform, matrixTransformView, lightVector, pixelCoords, pixelVertexWeights, app.Image, depthBuffer);
			//::ced::drawLine(targetPixels, )
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
