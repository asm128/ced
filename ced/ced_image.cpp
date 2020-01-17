#include "ced_image.h"
#include <Windows.h>

int													ced::bmpFileLoad
	( const char						* filename
	, ::ced::SImage						& imageLoaded
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
