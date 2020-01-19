#include "ced_model.h"
#include "ced_image.h"
#include "ced_draw.h"
#include "ced_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837


struct SStars	{
	::ced::container<::ced::SCoord2<float>>				Position		= {};
	::ced::container<float>								Speed			= {};
	::ced::container<float>								Brightness		= {};
};

struct SShots	{
	double												Delay			= 0;
	::ced::container<::ced::SCoord3<float>>				PositionPrev	= {};
	::ced::container<::ced::SCoord3<float>>				Position		= {};
	::ced::container<float>								Speed			= {};
	::ced::container<float>								Brightness		= {};
};

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

	::SScene											Scene				;
	::SStars											Stars				;
	::SShots											Shots				;
};

int													draw				(SApplication & app);

#endif // CED_DEMO_08_H_298837492837
