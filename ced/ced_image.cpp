#include "ced_image.h"
#include <Windows.h>

int													ced::bmpFileLoad
	( const char						* filename
	, ::ced::SImage						& imageLoaded
	, bool								invertY
	) {
	HBITMAP										phBitmap									= (HBITMAP)LoadImageA(0, filename, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);		// Use LoadImage() to get the image loaded into a DIBSection
	if(0 == phBitmap)
		return -1; //, "Failed to load bitmap file: %s.", szFileName.begin());

	BITMAP										bm											= {};
	GetObject(phBitmap, sizeof(BITMAP), &bm);		// Get the color depth of the DIBSection
	if(0 > imageLoaded.Pixels.resize(bm.bmWidth * bm.bmHeight))
		return -1; //, "Out of memory? Requested size: {x: %i, y: %i}", (int32_t)bm.bmWidth, (int32_t)bm.bmHeight);
	imageLoaded.Metrics						= {(uint32_t)bm.bmWidth, (uint32_t)bm.bmHeight};
	HDC											hMemDC										= CreateCompatibleDC(0);
	HBITMAP										hOldBitmap;
	hOldBitmap								= (HBITMAP)SelectObject(hMemDC, phBitmap);
	//::ced::SColorBGRA							* bmpPixels		= 0;
	BITMAPINFO									bitmapInfo				= {};
	bitmapInfo.bmiHeader.biSize				= sizeof(BITMAPINFO);
	bitmapInfo.bmiHeader.biWidth			= bm.bmWidth;
	bitmapInfo.bmiHeader.biHeight			= bm.bmHeight;
	bitmapInfo.bmiHeader.biPlanes			= 1;
	bitmapInfo.bmiHeader.biBitCount			= 32;
	bitmapInfo.bmiHeader.biCompression		= BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage		= bm.bmWidth * bm.bmHeight * sizeof(::ced::SColorBGRA);
	bitmapInfo.bmiHeader.biXPelsPerMeter	= 0xce4;
	bitmapInfo.bmiHeader.biYPelsPerMeter	= 0xce4;
	if(false == invertY)
		GetDIBits(hMemDC, phBitmap, 0, imageLoaded.Metrics.y, (void**)imageLoaded.Pixels.begin(), &bitmapInfo, DIB_RGB_COLORS);
	else {
		SImage										imageTemp;
		imageTemp.Resize(imageLoaded.Metrics);
		::ced::view_grid<::ced::SColorBGRA>			viewSrc			= imageTemp;
		::ced::view_grid<::ced::SColorBGRA>			viewDst			= imageLoaded;
		GetDIBits(hMemDC, phBitmap, 0, imageTemp.Metrics.y, (void**)imageTemp.Pixels.begin(), &bitmapInfo, DIB_RGB_COLORS);
		for(uint32_t y = 0; y < viewDst.metrics().y; ++y)
			memcpy(viewDst[y].begin(), viewSrc[viewSrc.metrics().y - 1 - y].begin(), sizeof(::ced::SColorBGRA) * viewSrc.metrics().x);
	}
	SelectObject(hMemDC, hOldBitmap);
	DeleteDC	(hMemDC);
	return 0;
}
