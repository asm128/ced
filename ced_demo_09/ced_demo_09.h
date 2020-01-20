#include "ced_demo_09_game.h"

#include "ced_model.h"
#include "ced_image.h"
#include "ced_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837

struct SScene	{
	::ced::SImage										Image				= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesLocal	= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesGlobal	= {};
	::ced::container<::ced::SModel3D>					Models				= {};
	::ced::container<::ced::SEntity>					Entities			= {};
	::ced::SGeometryQuads								Geometry			= {};
	::ced::SCamera										Camera				= {};
	::ced::SCoord3<float>								LightVector			= {15, 12, 0};
};

struct SApplication {
	::ced::SFramework									Framework			= {};

	::ced::container<int32_t>							Health				= {};

	::SScene											Scene				;
	::SStars											Stars				;
	::SShots											ShotsPlayer			;
	::SShots											ShotsEnemy			;
	::SDebris											Debris				;

	double												AnimationTime		= 0;
};

int													draw				(SApplication & app);

#endif // CED_DEMO_08_H_298837492837
