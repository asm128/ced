#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"
#include "ced_matrix.h"
#include "ced_geometry.h"

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
	::ced::SColorBGRA									Colors		[4]		= { {0xff}, {0, 0xFF, 0}, {0, 0, 0xFF}, {0xFF, 0xC0, 0x40} };

	::ced::container<::SModel3>						Models;
	::ced::container<uint32_t>							DepthBuffer;
	::ced::SGeometryQuads								Geometry;
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
	//::ced::geometryBuildCube(app.Geometry);
	//::ced::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Geometry, 12U, 7U, 1, {});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});
	app.Models.resize(6);
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		SModel3												& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		model.Position										= {4, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / app.Models.size() * iModel);
	}
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SWindow											& window			= app.Window;
	double													secondsLastFrame	= app.Timer.Tick() * .000001;
	app.TotalTime										+= secondsLastFrame;
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

	if(GetAsyncKeyState('Q')) cameraPosition.y				-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) cameraPosition.y				+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	//------------------------------------------- Transform and Draw
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
	::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights;
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
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			uint32_t												colorIndex			= (uint32_t)iModel % ::std::size(app.Colors);
			::ced::SColorBGRA										triangleColor		= app.Colors[colorIndex];
			::ced::drawQuadTriangle(targetPixels, app.Geometry, iTriangle, matrixTransform, matrixTransformView, lightVector, triangleColor, pixelCoords, pixelVertexWeights, {app.DepthBuffer.begin(), app.Window.Size});
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
