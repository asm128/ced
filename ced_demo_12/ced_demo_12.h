#include "ced_demo_12_game.h"

#include "ced_rigidbody.h"
#include "ced_model.h"
#include "ced_image.h"
#include "ced_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837

#pragma pack(push, 1)
struct SEntityFlags {
	int32_t												Padding						:1;
};
#pragma pack(pop)

struct SEntity {
	int32_t												Parent						;
	int32_t												Geometry					;
	int32_t												Transform					;
	int32_t												Image						;
	int32_t												Body						;
	SEntityFlags										Flags						;
	::ced::container<uint32_t>							Children					;
};

struct SShipPart {
	int32_t												Entity	;
	int32_t												Health	;
	::SShots											Shots	;
};

struct SShip {
	int32_t												Entity	;
	int32_t												Team	;
	int32_t												Health	;
	::ced::container<::SShipPart>						Parts	;
};

struct SShipScene	{
	::ced::container<::ced::SGeometryQuads>				Geometry					= {};
	::ced::container<::ced::SImage>						Image						= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesLocal			= {};
	::ced::container<::ced::SMatrix4<float>>			ModelMatricesGlobal			= {};
	::ced::container<::ced::SModel3>					Transforms					= {};
	::ced::SCamera										Camera						= {};
	::ced::SCoord3	<float>								LightVector					= {0, -12, 0};
	::ced::SMatrix4	<float>								MatrixProjection			= {};
};

struct SSolarSystem {
	::SShipScene										Scene						= {};
	::ced::container<::SEntity>							Entities					= {};
	::ced::container<::SShip>							Ships						= {};
	::ced::SIntegrator3									ShipPhysics					= {};

	::SStars											Stars						= {};
	::SDebris											Debris						= {};
	::ced::container<::SExplosion>						Explosions					= {};

	double												AnimationTime				= 0;
	double												TimeScale					= 1;

	::ced::SImage										BackgroundImage				= {};
	bool												Slowing						= true;

	uint32_t											Stage						= 20;
};

struct SApplication {
	::ced::SFramework									Framework					= {};

	SSolarSystem										SolarSystem					= {};
};

int													draw						(SApplication & app);

#endif // CED_DEMO_08_H_298837492837
