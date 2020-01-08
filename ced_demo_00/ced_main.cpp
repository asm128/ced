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

	::ced::SRectangle	<int32_t>	Rectangles	[4]		= {};
	::ced::SCircle		<int32_t>	Circles		[4]		= {};
	::ced::STriangle	<int32_t>	Triangles	[4]		= {};
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
		::ced::SRectangle	<int32_t>			& rectangle			= app.Rectangles[y * 2 + x];
		::ced::SCircle		<int32_t>			& circle			= app.Circles	[y * 2 + x];
		rectangle.Position					= {(int32_t)(window.Size.x / 2 * x	), (int32_t)(window.Size.y / 2 * y	)};
		rectangle.Size						= {(int32_t)(window.Size.x / 2		), (int32_t)(window.Size.y / 2		)};
		circle.Position						= {rectangle.Position.x + rectangle.Size.x / 2, rectangle.Position.y + rectangle.Size.y / 2};
		circle.Radius						= 10;

		::ced::STriangle	<int32_t>			& triangle			= app.Triangles	[y * 2 + x];
		triangle.A							= {0	, 32};
		triangle.B							= {-32	, -32};
		triangle.C							= {32	, -32};

		triangle.A							+= window.Size.Cast<int32_t>() / 4;
		triangle.B							+= window.Size.Cast<int32_t>() / 4;
		triangle.C							+= window.Size.Cast<int32_t>() / 4;

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
			::ced::SRectangle	<int32_t>			& rectangle			= app.Rectangles[y * 2 + x];
			::ced::SCircle		<int32_t>			& circle			= app.Circles	[y * 2 + x];
			rectangle.Position					= {(int32_t)(window.Size.x / 2 * x	), (int32_t)(window.Size.y / 2 * y	)};
			rectangle.Size						= {(int32_t)(window.Size.x / 2		), (int32_t)(window.Size.y / 2		)};
			circle.Position						= {rectangle.Position.x + rectangle.Size.x / 2, rectangle.Position.y + rectangle.Size.y / 2};
			circle.Radius						= 10;
		}
	}
	::ced::view_grid<::ced::SColor>			viewTarget			= {app.Pixels, window.Size};
	::ced::SLine<int32_t>					lineA				= {{0, 0}, window.Size.Cast<int32_t>() / 2};
	::ced::SLine<int32_t>					lineB				= {{(int32_t)window.Size.x / 2, 0}, {0, (int32_t)window.Size.y / 2}};
	static double							angle				= 0;
	angle								+= lastFrameSeconds / 10;
	for(uint32_t iRectangle = 0; iRectangle < ::std::size(app.Rectangles); ++iRectangle) {
		const ::ced::SRectangle<int32_t>		& rectangle			= app.Rectangles[iRectangle];
		::ced::drawRectangle	(viewTarget, rectangle, app.Colors[iRectangle]);
		::ced::drawTriangle		(viewTarget, app.Triangles	[iRectangle], app.Colors[(iRectangle + 1) % 4]);
		::ced::drawCircle		(viewTarget, app.Circles	[iRectangle], app.Colors[(iRectangle + 3) % 4]);
		::ced::SLine							finalLineA			= lineA;
		::ced::SLine							finalLineB			= lineB;
		finalLineA.A						= finalLineA.A.Rotated(angle);
		finalLineA.B						= finalLineA.B.Rotated(angle);
		finalLineB.A						= finalLineB.A.Rotated(angle);
		finalLineB.B						= finalLineB.B.Rotated(angle);
		finalLineA.A						+= rectangle.Position;
		finalLineA.B						+= rectangle.Position;
		finalLineB.A						+= rectangle.Position;
		finalLineB.B						+= rectangle.Position;
		::ced::drawLine			(viewTarget, finalLineA, app.Colors[(iRectangle + 2) % 4]);
		::ced::drawLine			(viewTarget, finalLineB, app.Colors[(iRectangle + 2) % 4]);
	}
	return app.Running ? 0 : 1;
}

int									main				()	{
	SApplication							app;
	setup(app);
	while(app.Running) {
		if(1 == update(app))
			break;
	}
	cleanup(app);
	return 0;
}
