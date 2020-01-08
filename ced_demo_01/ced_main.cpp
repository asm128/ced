#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"

#include <cstdint>
#include <algorithm>

struct SApplication {
	::ced::SWindow								Window				= {};
	::ced::SColor								* Pixels			= 0;
	::ced::STimer								Timer				= {};
	bool										Running				= true;
	double										TotalTime			= 0;
	::ced::SColor								Colors		[4]		= { {0xff}, {0, 0xFF}, {0, 0, 0xFF}, {0xFF, 0xC0, 0x40} };

	::ced::container<::ced::STriangle<int32_t>>	Triangles;
};

int									cleanup				(SApplication & app)	{
	free(app.Pixels);
	::ced::windowCleanup(app.Window);
	return 0;
}

int									setupTriangles		(SApplication & app)	{
	::ced::SWindow							& window			= app.Window;
	for(uint32_t iTriangle = 0; iTriangle < app.Triangles.size(); ++iTriangle) {
		::ced::STriangle<int32_t>				& newTriangle		= app.Triangles[iTriangle];
		newTriangle							= {};
		newTriangle.A						= {0, 128};
		newTriangle.B						= newTriangle.A.Rotated(::ced::MATH_2PI / 3);
		newTriangle.C						= newTriangle.A.Rotated(-::ced::MATH_2PI / 3);

		newTriangle.A						+= {0, 128};
		newTriangle.B						+= {0, 128};
		newTriangle.C						+= {0, 128};

		newTriangle.A						= newTriangle.A.Rotated(::ced::MATH_2PI / app.Triangles.size() * iTriangle);
		newTriangle.B						= newTriangle.B.Rotated(::ced::MATH_2PI / app.Triangles.size() * iTriangle);
		newTriangle.C						= newTriangle.C.Rotated(::ced::MATH_2PI / app.Triangles.size() * iTriangle);

		newTriangle.A						+= window.Size.Cast<int32_t>() / 2;
		newTriangle.B						+= window.Size.Cast<int32_t>() / 2;
		newTriangle.C						+= window.Size.Cast<int32_t>() / 2;
	}
	return 0;
}

int									setup				(SApplication & app)	{
	::ced::SWindow							& window			= app.Window;
	::ced::windowSetup(window);
	app.Pixels							= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
	app.Triangles.resize(12);
	setupTriangles(app);
	return 0;
}

int									update				(SApplication & app)	{
	::ced::SWindow							& window			= app.Window;
	double									lastFrameSeconds	= app.Timer.Tick() * .000001;
	app.TotalTime						+= lastFrameSeconds;
	if(1 == ::ced::windowUpdate(window, app.Pixels))
		return 1;
	if(window.Resized) {
		free(app.Pixels);
		app.Pixels							= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
		setupTriangles(app);
	}

	::ced::view_grid<::ced::SColor>			targetPixels		= {app.Pixels, window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColor) * targetPixels.size());
	for(uint32_t iTriangle = 0; iTriangle < app.Triangles.size(); ++iTriangle) {
		::ced::STriangle<int32_t>				newTriangle			= app.Triangles[iTriangle];

		newTriangle.A						-= window.Size.Cast<int32_t>() / 2;
		newTriangle.B						-= window.Size.Cast<int32_t>() / 2;
		newTriangle.C						-= window.Size.Cast<int32_t>() / 2;

		newTriangle.A						= newTriangle.A.Rotated(app.TotalTime / 2);
		newTriangle.B						= newTriangle.B.Rotated(app.TotalTime / 2);
		newTriangle.C						= newTriangle.C.Rotated(app.TotalTime / 2);

		newTriangle.A						+= window.Size.Cast<int32_t>() / 2;
		newTriangle.B						+= window.Size.Cast<int32_t>() / 2;
		newTriangle.C						+= window.Size.Cast<int32_t>() / 2;
		::ced::drawTriangle(targetPixels, newTriangle, app.Colors[iTriangle % ::std::size(app.Colors)]);
	}
	return app.Running ? 0 : 1;
}

int	WINAPI							WinMain
(	HINSTANCE	// hInstance
,	HINSTANCE	// hPrevInstance
,	LPSTR		// lpCmdLine
,	INT			// nShowCmd
) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
	SApplication							app;
	setup(app);
	while(app.Running) {
		if(1 == update(app))
			break;
	}
	cleanup(app);
	return 0;
}
