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

	::ced::SRectangle				Rectangles	[4]		= {};
	::ced::SCircle					Circles		[4]		= {};
	::ced::STriangle				Triangles	[4]		= {};
	::ced::SColor					Colors		[4]		= { {0xff}, {0, 0xFF}, {0, 0, 0xFF} };
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
	::ced::SWindow							& window			= app.Window;
	double									lastFrameSeconds	= app.Timer.Tick() * .000001;
	(void)lastFrameSeconds;
	if(1 == ::ced::windowUpdate(window, app.Pixels))
		return 1;
	if(window.Resized) {
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
	::ced::SLine						lineA				= {{0, 0}, window.Size / 2};
	::ced::SLine						lineB				= {{window.Size.x / 2, 0}, {0, window.Size.y / 2}};
	static double						angle				= 0;
	angle							+= lastFrameSeconds / 10;
	for(uint32_t iRectangle = 0; iRectangle < ::std::size(app.Rectangles); ++iRectangle) {
		const ::ced::SRectangle				& rectangle			= app.Rectangles[iRectangle];
		::ced::drawRectangle	(app.Pixels, window.Size, rectangle, app.Colors[iRectangle]);
		::ced::drawTriangle		(app.Pixels, window.Size, app.Triangles[iRectangle], app.Colors[(iRectangle + 1) % 4]);
		::ced::drawCircle		(app.Pixels, window.Size, app.Circles[iRectangle], app.Colors[(iRectangle + 3) % 4]);
		::ced::SLine						finalLineA			= lineA;
		::ced::SLine						finalLineB			= lineB;
		finalLineA.A					= finalLineA.A.Rotated(angle);
		finalLineA.B					= finalLineA.B.Rotated(angle);
		finalLineB.A					= finalLineB.A.Rotated(angle);
		finalLineB.B					= finalLineB.B.Rotated(angle);
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
