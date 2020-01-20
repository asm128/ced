#include "ced_window.h"

LRESULT	WINAPI							wndProc					(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int										blitBuffer				(HDC hdc, const ::ced::SColorBGRA * pixels, ::ced::SCoord2<uint32_t> targetSize)	{
	BITMAPINFO									bitmapInfo				= {};
	bitmapInfo.bmiHeader.biSize				= sizeof(BITMAPINFO);
	bitmapInfo.bmiHeader.biWidth			= targetSize.x;
	bitmapInfo.bmiHeader.biHeight			= targetSize.y;
	bitmapInfo.bmiHeader.biPlanes			= 1;
	bitmapInfo.bmiHeader.biBitCount			= 32;
	bitmapInfo.bmiHeader.biCompression		= BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage		= targetSize.x * targetSize.y * sizeof(::ced::SColorBGRA);
	bitmapInfo.bmiHeader.biXPelsPerMeter	= 0xce4;
	bitmapInfo.bmiHeader.biYPelsPerMeter	= 0xce4;

	HDC											hdcCompatible			= CreateCompatibleDC(hdc);
	if(hdcCompatible) {
		void										* pvBits;
		HBITMAP										dibSection				= CreateDIBSection(hdcCompatible, &bitmapInfo, DIB_RGB_COLORS, (void**)&pvBits, 0, 0);
		if(dibSection) {
			RGBQUAD										* reverseYpixels		= (RGBQUAD*)malloc(sizeof(RGBQUAD) * targetSize.x * targetSize.y);
			if(reverseYpixels) {
				for(uint32_t y = 0; y < targetSize.y; ++y)
					memcpy(&reverseYpixels[(targetSize.y - 1 - y) * targetSize.x], &pixels[y * targetSize.x], sizeof(RGBQUAD) * targetSize.x);
				SetDIBits(hdcCompatible, dibSection, 0, targetSize.y, reverseYpixels, &bitmapInfo, DIB_RGB_COLORS);
				free(reverseYpixels);
			}
			//SetDIBits(hdcCompatible, dibSection, 0, targetSize.y, pixels, &bitmapInfo, DIB_RGB_COLORS);
			HBITMAP										oldBitmap				= (HBITMAP)SelectObject(hdcCompatible, dibSection);
			BitBlt(hdc, 0, 0, targetSize.x, targetSize.y, hdcCompatible, 0, 0, SRCCOPY);
			SelectObject(hdcCompatible, oldBitmap);
			DeleteObject(dibSection);
		}
		DeleteDC(hdcCompatible);
	}
	return 0;
}

int32_t									ced::windowCleanup		(::ced::SWindow & window) { UnregisterClass(window.ClassName, window.Class.hInstance); return 0; }
int32_t									ced::windowSetup		(::ced::SWindow & window) {
	window.Class.lpszClassName				= window.ClassName;
	window.Class.lpfnWndProc				= wndProc;
	window.Class.hInstance					= GetModuleHandle(0);
	RegisterClassEx(&window.Class);

	RECT										windowRect				= {0, 0, (LONG)window.Size.x, (LONG)window.Size.y};
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	window.Handle							= CreateWindowEx(0, window.Class.lpszClassName, "Window 0", WS_OVERLAPPEDWINDOW, window.Position.x, window.Position.y, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 0, 0, window.Class.hInstance, 0);
	ShowWindow(window.Handle, SW_SHOW);
	return 0;
}

int32_t									ced::windowUpdate		(::ced::SWindow & window, const ::ced::SColorBGRA* pixels) {
	HDC											deviceContext			= GetDC(window.Handle);
	::blitBuffer(deviceContext, pixels, window.Size);
	ReleaseDC	(window.Handle, deviceContext);

	MSG											msg						= {};
	bool										quit					= false;
	window.Resized							= false;
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage	(&msg);
		if(WM_QUIT == msg.message)
			quit									= true;
	}
	RECT										clientRect				= {};
	GetClientRect(window.Handle, &clientRect);
	if(clientRect.right != (LONG)window.Size.x || clientRect.bottom != (LONG)window.Size.y) {
		window.Size.x							= clientRect.right	;
		window.Size.y							= clientRect.bottom	;
		window.Resized							= true;
	}
	return quit ? 1 : 0;
}

