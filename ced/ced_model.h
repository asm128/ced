#include "ced_view.h"
#include "ced_matrix.h"

#ifndef CED_MODEL_H_2983472398
#define CED_MODEL_H_2983472398

namespace ced
{
	struct SModel3D {
		::ced::SCoord3<float>								Scale;
		::ced::SCoord3<float>								Rotation;
		::ced::SCoord3<float>								Position;
	};

	struct SModelTransform {
		::ced::SMatrix4<float>								Scale		;
		::ced::SMatrix4<float>								Rotation	;
		::ced::SMatrix4<float>								Position	;
	};

	struct SEntity {
		int32_t												Parent;
		::ced::container<int32_t>							Children			= {};
	};

	struct SCamera {
		::ced::SCoord3<float>								Target				= {};
		::ced::SCoord3<float>								Position			= {-0.000001f, 100, 0};
		::ced::SCoord3<float>								Up					= {0, 1, 0};
	};


} // namespace

#endif // CED_MODEL_H_2983472398
