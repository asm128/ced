#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"
#include "ced_matrix.h"
#include "ced_geometry.h"

#include <cstdint>
#include <algorithm>

struct SImage {
	::ced::SCoord2<uint32_t>							Metrics		;
	::ced::container<::ced::SColor>						Pixels		;
};

struct SApplication {
	::ced::SWindow										Window				= {};
	::ced::SColor										* Pixels			= 0;
	::ced::STimer										Timer				= {};
	bool												Running				= true;
	double												TotalTime			= 0;
	::ced::SColor										Colors		[4]		= { {0xff}, {0, 0xFF}, {0, 0, 0xFF}, {0xFF, 0xC0, 0x40} };

	::SImage											Image				= {};
};

int													bmpFileLoad
	( const char						* filename
	, ::SImage							& imageLoaded
	) {
	HINSTANCE												hInstance			= GetModuleHandle(0);
	HBITMAP													loadedBMP			= (HBITMAP)LoadImage(hInstance, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	HDC														hdc					= CreateCompatibleDC(0);
	BITMAP													bitmap				= {};
	GetObject(loadedBMP, sizeof(BITMAP), &bitmap);
	HBITMAP													oldBMP				= (HBITMAP)SelectObject(hdc, loadedBMP);
	imageLoaded.Pixels.resize(bitmap.bmHeight * bitmap.bmWidth);
	imageLoaded.Metrics.x								= bitmap.bmWidth	;
	imageLoaded.Metrics.y								= bitmap.bmHeight	;
	for(uint32_t y = 0; y < imageLoaded.Metrics.y; ++y)
	for(uint32_t x = 0; x < imageLoaded.Metrics.x; ++x) {
		COLORREF												winColor			= GetPixel(hdc, x, y);
		imageLoaded.Pixels[y * imageLoaded.Metrics.x + x]	= {GetRValue(winColor), GetGValue(winColor), GetBValue(winColor), 0xFF};
	}
	SelectObject(hdc, oldBMP);
	DeleteDC(hdc);
	DeleteObject(loadedBMP);
	return 0;
}

int													cleanup				(SApplication & app)	{
	free(app.Pixels);
	::ced::windowCleanup(app.Window);
	return 0;
}

int													setup				(SApplication & app)	{
	::bmpFileLoad("../ced_data/cp437_12x12.bmp", app.Image);

	::ced::SWindow											& window			= app.Window;
	window.Size											= app.Image.Metrics;
	::ced::windowSetup(window);
	app.Pixels											= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
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
	}
	::ced::view_grid<::ced::SColor>							targetPixels		= {app.Pixels, window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColor) * targetPixels.size());

	::ced::view_grid<const ::ced::SColor>					viewImage				= {app.Image.Pixels.begin(), app.Image.Metrics};
	for(uint32_t y = 0; y < app.Image.Metrics.y; ++y)
	for(uint32_t x = 0; x < app.Image.Metrics.x; ++x) {
		targetPixels[y][x]									= viewImage[y][x];
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
