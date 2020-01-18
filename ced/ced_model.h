#include "ced_view.h"

#ifndef CED_MODEL_H_2983472398
#define CED_MODEL_H_2983472398

namespace ced
{
	struct SModel3D {
		::ced::SCoord3<float>								Scale;
		::ced::SCoord3<float>								Rotation;
		::ced::SCoord3<float>								Position;
	};

	struct SEntity {
		int32_t												Parent;
		::ced::container<int32_t>							Children			= {};
	};
} // namespace

#endif // CED_MODEL_H_2983472398
