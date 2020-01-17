#include "ced_view.h"
#include "ced_color.h"

#ifndef CED_IMAGE_H_2390487239847
#define CED_IMAGE_H_2390487239847

namespace ced
{
	struct SImage {
		::ced::SCoord2<uint32_t>							Metrics		;
		::ced::container<::ced::SColor>						Pixels		;
	};

	int													bmpFileLoad				(const char* filename, ::ced::SImage & imageLoaded);
} // namespace

#endif // CED_IMAGE_H_2390487239847
