#include "ced_draw.h"

#include <Windows.h>
#include <cstdint>
#include <algorithm>

LRESULT	WINAPI					wndProc				(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch(uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

struct SWindow {
	WNDCLASSEX						Class				= {sizeof(WNDCLASSEX),};
	const char						ClassName	[256]	= "ced_window";
	::ced::SCoord					Position			= {10, 10};
	::ced::SCoord					Size				= {320, 200};
	HWND							Handle				= 0;
};

struct SApplication {
	SWindow							Window;
	::ced::SColor					* Pixels			= 0;
	bool							Running				= true;
	::ced::SRectangle				Rectangles	[4]		= {};
	::ced::SCircle					Circles		[4]		= {};
	::ced::STriangle				Triangles	[4]		= {};
	::ced::SColor					Colors		[4]		= { {0xff}, {0, 0xFF}, {0, 0, 0xFF} };
};

int										blitBuffer			(HDC hdc, ::ced::SColor * pixels, ::ced::SCoord targetSize)	{
	BITMAPINFO									bitmapInfo			= {};
	bitmapInfo.bmiHeader.biSize				= sizeof(BITMAPINFO);
	bitmapInfo.bmiHeader.biWidth			= targetSize.x;
	bitmapInfo.bmiHeader.biHeight			= targetSize.y;
	bitmapInfo.bmiHeader.biPlanes			= 1;
	bitmapInfo.bmiHeader.biBitCount			= 32;
	bitmapInfo.bmiHeader.biCompression		= BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage		= targetSize.x * targetSize.y * sizeof(::ced::SColor);
	bitmapInfo.bmiHeader.biXPelsPerMeter	= 0xce4;
	bitmapInfo.bmiHeader.biYPelsPerMeter	= 0xce4;

	HDC											hdcCompatible		= CreateCompatibleDC(hdc);
	void										* pvBits;
	HBITMAP										dibSection			= CreateDIBSection(hdcCompatible, &bitmapInfo, DIB_RGB_COLORS, (void**)&pvBits, 0, 0);
	SetDIBits(hdcCompatible, dibSection, 0, targetSize.y, pixels, &bitmapInfo, DIB_RGB_COLORS);
	HBITMAP										oldBitmap			= (HBITMAP)SelectObject(hdcCompatible, dibSection);
	BitBlt(hdc, 0, 0, targetSize.x, targetSize.y, hdcCompatible, 0, 0, SRCCOPY);
	SelectObject(hdcCompatible, oldBitmap);
	DeleteObject(dibSection);
	DeleteDC	(hdcCompatible);
	return 0;
}

int									cleanup				(SApplication & app)	{
	free(app.Pixels);
	UnregisterClass(app.Window.ClassName, app.Window.Class.hInstance);
	return 0;
}

int									setup				(SApplication & app)	{
	SWindow									& window			= app.Window;
	window.Class.lpszClassName			= window.ClassName;
	window.Class.lpfnWndProc			= wndProc;
	window.Class.hInstance				= GetModuleHandle(0);
	RegisterClassEx(&window.Class);

	RECT									windowRect			= {0, 0, window.Size.x, window.Size.y};

	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	window.Handle		= CreateWindowEx(0, window.Class.lpszClassName, "Window 0", WS_OVERLAPPEDWINDOW, window.Position.x, window.Position.y, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 0, 0, window.Class.hInstance, 0);
	ShowWindow(window.Handle, SW_SHOW);

	app.Pixels							= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);

	for(uint32_t y = 0; y < (uint32_t)2; ++y)
	for(uint32_t x = 0; x < (uint32_t)2; ++x) {
		::ced::SRectangle						& rectangle			= app.Rectangles[y * 2 + x];
		::ced::SCircle							& circle			= app.Circles	[y * 2 + x];
		rectangle.Position					= {(int32_t)(window.Size.x / 2 * x	), (int32_t)(window.Size.y / 2 * y	)};
		rectangle.Size						= {(int32_t)(window.Size.x / 2		), (int32_t)(window.Size.y / 2		)};
		circle.Position						= {rectangle.Position.x + rectangle.Size.x / 2, rectangle.Position.y + rectangle.Size.y / 2};
		circle.Radius						= 10;

		::ced::STriangle						& triangle			= app.Triangles	[y * 2 + x];
		triangle.A							= {0	, 32};
		triangle.B							= {-32	, -32};
		triangle.C							= {32	, -32};

		triangle.A							+= window.Size / 4;
		triangle.B							+= window.Size / 4;
		triangle.C							+= window.Size / 4;

		triangle.A							+= rectangle.Position;
		triangle.B							+= rectangle.Position;
		triangle.C							+= rectangle.Position;
	}
	return 0;
}

int									update				(SApplication & app)	{
	SWindow									& window			= app.Window;
	MSG										msg					= {};
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage	(&msg);
		if(WM_QUIT == msg.message)
			app.Running							= false;
	}
	RECT									clientRect			= {};
	GetClientRect(window.Handle, &clientRect);
	if(clientRect.right != window.Size.x || clientRect.bottom != window.Size.y) {
		window.Size.x = clientRect.right	;
		window.Size.y = clientRect.bottom	;
		free(app.Pixels);
		app.Pixels							= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
		for(uint32_t y = 0; y < (uint32_t)2; ++y)
		for(uint32_t x = 0; x < (uint32_t)2; ++x) {
			::ced::SRectangle						& rectangle			= app.Rectangles[y * 2 + x];
			::ced::SCircle							& circle			= app.Circles	[y * 2 + x];
			rectangle.Position					= {(int32_t)(window.Size.x / 2 * x	), (int32_t)(window.Size.y / 2 * y	)};
			rectangle.Size						= {(int32_t)(window.Size.x / 2		), (int32_t)(window.Size.y / 2		)};
			circle.Position						= {rectangle.Position.x + rectangle.Size.x / 2, rectangle.Position.y + rectangle.Size.y / 2};
			circle.Radius						= 10;
		}
	}

	HDC									deviceContext		= GetDC(window.Handle);
	blitBuffer(deviceContext, app.Pixels, window.Size);
	ReleaseDC(window.Handle, deviceContext);

	::ced::SLine						lineA				= {{0, 0}, window.Size / 2};
	::ced::SLine						lineB				= {{window.Size.x / 2, 0}, {0, window.Size.y / 2}};
	for(uint32_t iRectangle = 0; iRectangle < ::std::size(app.Rectangles); ++iRectangle) {
		const ::ced::SRectangle				& rectangle			= app.Rectangles[iRectangle];
		::ced::drawRectangle	(app.Pixels, window.Size, rectangle, app.Colors[iRectangle]);
		::ced::drawTriangle		(app.Pixels, window.Size, app.Triangles[iRectangle], app.Colors[(iRectangle + 1) % 4]);
		::ced::drawCircle		(app.Pixels, window.Size, app.Circles[iRectangle], app.Colors[(iRectangle + 3) % 4]);
		::ced::SLine						finalLineA			= lineA;
		::ced::SLine						finalLineB			= lineB;
		finalLineA.A					+= rectangle.Position;
		finalLineA.B					+= rectangle.Position;
		finalLineB.A					+= rectangle.Position;
		finalLineB.B					+= rectangle.Position;
		::ced::drawLine			(app.Pixels, window.Size, finalLineA, app.Colors[(iRectangle + 2) % 4]);
		::ced::drawLine			(app.Pixels, window.Size, finalLineB, app.Colors[(iRectangle + 2) % 4]);
	}
	return app.Running ? 0 : 1;
}

int								main				()	{
	SApplication						app;
	setup(app);
	while(app.Running) {
		if(1 == update(app))
			break;
	}
	cleanup(app);
	return 0;
}
