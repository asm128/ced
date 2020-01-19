#include "ced_view.h"

#ifndef CED_STARS_H_2983749823
#define CED_STARS_H_2983749823

namespace ced
{
	struct SStarBackground {
		::ced::container<::ced::SCoord2<float>>				StarPosition		= {};
		::ced::container<float>								StarSpeed			= {};
		::ced::container<float>								StarBrightness		= {};
	};
} // namespace

#endif // CED_STARS_H_2983749823
