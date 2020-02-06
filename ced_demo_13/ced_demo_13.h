#include "ced_demo_13_game.h"

#include "ced_rigidbody.h"
#include "ced_model.h"
#include "ced_image.h"
#include "ced_framework.h"

#ifndef CED_DEMO_08_H_298837492837
#define CED_DEMO_08_H_298837492837


struct SApplication {
	::ced::SFramework									Framework					= {};

	SSolarSystem										SolarSystem					= {};

	volatile long long									ThreadSignal				= 0;
};

#endif // CED_DEMO_08_H_298837492837
