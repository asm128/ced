#include "ced_math.h"
#include "ced_draw.h"
#include <Windows.h>

#ifndef CED_WINDOW_H_23987423984
#define CED_WINDOW_H_23987423984

namespace ced
{
	struct SWindow {
		WNDCLASSEX						Class				= {sizeof(WNDCLASSEX),};
		const char						ClassName	[256]	= "ced_window";
		::ced::SCoord2<int32_t>			Position			= {10, 10};
		::ced::SCoord2<uint32_t>		Size				= {800, 600};
		HWND							Handle				= 0;
		bool							Resized				= true;
	};

	int32_t							windowSetup		(::ced::SWindow & window);
	int32_t							windowUpdate	(::ced::SWindow & window, const ::ced::SColor* pixels);
	int32_t							windowCleanup	(::ced::SWindow & window);
} // namespace

#endif // CED_WINDOW_H_23987423984
