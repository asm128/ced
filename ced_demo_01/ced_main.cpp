#include "ced_draw.h"
#include "ced_timer.h"
#include "ced_window.h"

#include <cstdint>
#include <algorithm>

struct SApplication {
	::ced::SWindow					Window				= {};
	::ced::SColor					* Pixels			= 0;
	::ced::STimer					Timer				= {};
	bool							Running				= true;
};

int									cleanup				(SApplication & app)	{
	free(app.Pixels);
	::ced::windowCleanup(app.Window);
	return 0;
}

int									setup				(SApplication & app)	{
	::ced::SWindow							& window			= app.Window;
	::ced::windowSetup(app.Window);
	app.Pixels							= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
	return 0;
}

int									update				(SApplication & app)	{
	::ced::SWindow							& window			= app.Window;
	double									lastFrameSeconds	= app.Timer.Tick() * .000001;
	(void)lastFrameSeconds;
	if(1 == ::ced::windowUpdate(window, app.Pixels))
		return 1;
	if(window.Resized) {
		free(app.Pixels);
		app.Pixels							= (::ced::SColor*)malloc(sizeof(::ced::SColor) * window.Size.x * window.Size.y);
	}
	return app.Running ? 0 : 1;
}

int	WINAPI							WinMain
(	HINSTANCE	// hInstance
,	HINSTANCE	// hPrevInstance
,	LPSTR		// lpCmdLine
,	INT			// nShowCmd
) {
	SApplication							app;
	setup(app);
	while(app.Running) {
		if(1 == update(app))
			break;
	}
	cleanup(app);
	return 0;
}
