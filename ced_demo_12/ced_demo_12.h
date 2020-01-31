#include "ced_demo_12_game.h"

#include "ced_rigidbody.h"
#include "ced_model.h"
#include "ced_image.h"
#include "ced_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837

struct SEntity {
	int32_t												Parent						;
	int32_t												Geometry					;
	int32_t												Transform					;
	int32_t												Image						;
	int32_t												Body						;
	::ced::container<uint32_t>							Children					;
};

struct SShipScene	{
	::ced::container<::ced::SImage>						Image						= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesLocal			= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesGlobal			= {};
	::ced::container<::ced::SModel3>					Transforms					= {};
	::ced::container<::ced::SGeometryQuads>				Geometry					= {};
	::ced::SCamera										Camera						= {};
	::ced::SCoord3<float>								LightVector					= {15, 12, 0};
};

struct SShip {
	int32_t												Entity;
};

struct SSolarSystem {
	::SShipScene										Scene						;
	::ced::container<::SEntity>							Entities					= {};
	::SStars											Stars						;
	::SShots											ShotsPlayer					;
	::SShots											ShotsEnemy					;
	::SDebris											Debris						;
	::SDebris											SunFire						= {};
	::ced::SIntegrator3									Bodies						= {};
	::ced::container<::ced::SImage>						Image						= {};
	::ced::container<::SExplosion>						Explosions;

	double												AnimationTime				= 0;
	double												TimeScale					= 1;
	::ced::container<int32_t>							Health						= {};
};

struct SApplication {
	::ced::SFramework									Framework					= {};

	SSolarSystem										SolarSystem					= {};
};

int													draw						(SApplication & app);

#endif // CED_DEMO_08_H_298837492837
