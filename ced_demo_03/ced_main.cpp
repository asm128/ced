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

struct SModel3D {
	::ced::SCoord3<float>								Scale;
	::ced::SCoord3<float>								Rotation;
	::ced::SCoord3<float>								Position;
};

struct SApplication {
	::ced::SWindow										Window				= {};
	::ced::SColor										* Pixels			= 0;
	::ced::STimer										Timer				= {};
	bool												Running				= true;
	double												TotalTime			= 0;
	::ced::SColor										Colors		[4]		= { {0xff}, {0, 0xFF}, {0, 0, 0xFF}, {0xFF, 0xC0, 0x40} };

	::ced::container<::SModel3D>						Models;
	::ced::container<::ced::STriangle3	<float>>		Triangles;
	::ced::container<::ced::SCoord3		<float>>		Normals;
};

int													cleanup				(SApplication & app)	{
	free(app.Pixels);
	::ced::windowCleanup(app.Window);
	return 0;
}

int													setupTriangles		(SApplication & app)	{
	app.Triangles	.resize((uint32_t)::std::size(geometryCube));
	app.Normals		.resize((uint32_t)::std::size(geometryNormals));

	for(uint32_t iTriangle = 0; iTriangle < app.Triangles.size(); ++iTriangle) {
		::ced::STriangle3<float>								& newTriangle		= app.Triangles[iTriangle];
		newTriangle											= geometryCube[iTriangle].Cast<float>();
		newTriangle.A										-= {.5, .5, .5};
		newTriangle.B										-= {.5, .5, .5};
		newTriangle.C										-= {.5, .5, .5};

		::ced::SCoord3<float>									& newNormal			= app.Normals[iTriangle / 2];
		newNormal											= geometryNormals[iTriangle / 2].Cast<float>();

	}
	return 0;
}

int													setup				(SApplication & app)	{
	::ced::SWindow											& window			= app.Window;
	::ced::windowSetup(window);
	app.Pixels											= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
	::setupTriangles(app);

	app.Models.resize(6);
	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		SModel3D												& model			= app.Models[iModel];
		model.Scale											= {1, 2, 3};
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
		app.Pixels											= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
		::setupTriangles(app);
	}
	::ced::view_grid<::ced::SColor>							targetPixels		= {app.Pixels, window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColor) * targetPixels.size());

	::ced::SCoord3<float>									cameraTarget		= {};
	static ::ced::SCoord3<float>							cameraPosition		= {15, 2, 0};
	::ced::SCoord3<float>									cameraUp			= {0, 1, 0};

	static ::ced::SCoord3<float>							lightVector			= {15, 2, 0};
	lightVector											= lightVector	.RotateY(lastFrameSeconds * 2);
	cameraPosition										= cameraPosition.RotateY(lastFrameSeconds / 2);

	lightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(cameraPosition, cameraTarget, cameraUp);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics(), 0.01, 1000);
	matrixView											= matrixView * matrixProjection;
	matrixView											= matrixView * matrixViewport.GetInverse();

	for(uint32_t iModel = 0; iModel < app.Models.size(); ++iModel) {
		::ced::SMatrix4<float>									matrixScale		;
		::ced::SMatrix4<float>									matrixRotation	;
		::ced::SMatrix4<float>									matrixPosition	;
		matrixScale		.Scale			(app.Models[iModel].Scale	, true);
		matrixRotation	.Rotation		(app.Models[iModel].Rotation);
		matrixPosition	.SetTranslation	(app.Models[iModel].Position, true);

		::ced::SMatrix4<float>									matrixTransform		= matrixScale * matrixPosition * matrixRotation;
		for(uint32_t iTriangle = 0; iTriangle < app.Triangles.size(); ++iTriangle) {
			::ced::STriangle3	<float>								triangle			= app.Triangles	[iTriangle];
			::ced::SCoord3		<float>								normal				= app.Normals	[iTriangle / 2];
			triangle.A											= matrixTransform.Transform(triangle.A);
			triangle.B											= matrixTransform.Transform(triangle.B);
			triangle.C											= matrixTransform.Transform(triangle.C);

			triangle.A											= matrixView.Transform(triangle.A);
			triangle.B											= matrixView.Transform(triangle.B);
			triangle.C											= matrixView.Transform(triangle.C);

			double													lightFactor			= normal.Dot(lightVector);

			::ced::STriangle<int32_t>								newTriangle			=
				{ {(int32_t)triangle.A.x, (int32_t)triangle.A.y}
				, {(int32_t)triangle.B.x, (int32_t)triangle.B.y}
				, {(int32_t)triangle.C.x, (int32_t)triangle.C.y}
				};
			::ced::SCoord2	<int32_t>								halfScreen			= window.Size.Cast<int32_t>() / 2;
			newTriangle.A										+= {halfScreen.x, halfScreen.y, };
			newTriangle.B										+= {halfScreen.x, halfScreen.y, };
			newTriangle.C										+= {halfScreen.x, halfScreen.y, };
			uint32_t												colorIndex			= (uint32_t)(app.TotalTime * .2) % ::std::size(app.Colors);
			::ced::drawTriangle(targetPixels, newTriangle, app.Colors[colorIndex] * .1 + app.Colors[colorIndex] * lightFactor); //app.Colors[iTriangle % ::std::size(app.Colors)]);
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
