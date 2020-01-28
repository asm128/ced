#include "ced_view.h"
#include "ced_color.h"

#ifndef CED_IMAGE_H_2390487239847
#define CED_IMAGE_H_2390487239847

namespace ced
{
	struct SImage {
		::ced::SCoord2<uint32_t>			Metrics	;
		::ced::container<::ced::SColorBGRA>	Pixels	;

		operator							view_grid<::ced::SColorBGRA>		()										{ return {Pixels.begin(), Metrics}; }
		operator							view_grid<const ::ced::SColorBGRA>	()								const	{ return {Pixels.begin(), Metrics}; }

				uint32_t					Resize								(uint32_t x, uint32_t y)				{ Pixels.resize(x * y); Metrics = {x, y}; return Pixels.size(); }
		inline	uint32_t					Resize								(::ced::SCoord2<uint32_t> & metrics)	{ Resize(metrics.x, metrics.y); }
	};

	int									bmpFileLoad							(const char* filename, ::ced::SImage & imageLoaded);
} // namespace

#endif // CED_IMAGE_H_2390487239847
