#include "ced_rigidbody.h"

#include "ced_geometry.h"
#include "ced_image.h"
#include "ced_draw.h"

#include "ced_framework.h"
#include "ced_model.h"
#include "ced_particles.h"

#include <cstring>
#include <cstdint>
#include <algorithm>

enum PLANET
	{ PLANET_MERCURY
	, PLANET_VENUS
	, PLANET_EARTH
	, PLANET_MARS
	, PLANET_JUPITER
	, PLANET_SATURN
	, PLANET_URANUS
	, PLANET_NEPTUNE
	, PLANET_PLUTO

	, PLANET_COUNT
	};

struct SModelPivot {
	::ced::SCoord3<float>								Scale							= {1, 1, 1};
	::ced::SCoord3<float>								Position						= {};
};

struct SScene {
	::ced::container<::SModelPivot>						Pivot							= {};
	::ced::container<::ced::SGeometryTriangles>			Geometry						= {};
	::ced::container<::ced::SMatrix4<float>>			Transform						= {};

	::ced::SCamera										Camera							= {};
};

struct SEntity {
	int32_t												IndexParent						;
	int32_t												IndexGeometry					;
	int32_t												IndexModel						;
	int32_t												IndexImage						;
	int32_t												IndexBody						;
	::ced::container<uint32_t>							IndexChild						;
};


struct SDebris	{
	::ced::SColorBGRA							Colors[4]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0xAF, 0xFF, }
		, {0x80, 0xCF, 0xFF, }
		//, {0x00, 0x00, 0xFF, }
		};
	::ced::container<float>						Brightness			= {};
	::ced::SParticles3							Particles			= {};

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed, float brightness)	{
		Particles.Spawn(position, direction, speed);
		return Brightness.push_back(brightness);
	}
	int											SpawnSpherical		(uint32_t countDebris, const ::ced::SCoord3<float> & position, float speedDebris, float brightness, float offset)	{
		for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
			::ced::SCoord3<float>									direction				= {0, 1 * offset, 0};
			direction.RotateX(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.RotateY(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.RotateZ(rand() * (::ced::MATH_2PI / RAND_MAX));
			const ::ced::SCoord3<float>newPosition		= position + direction;
			direction.Normalize();
			Spawn(newPosition, direction, speedDebris, brightness);
		}
		return 0;
	}
	int											Update				(double secondsLastFrame)	{
		Particles.IntegrateSpeed(secondsLastFrame);
		const float										fLastFrame			= (float)secondsLastFrame;
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			float											& speed				= Particles.Speed		[iShot];
			float											& brightness 		= Brightness			[iShot];
			brightness									-= fLastFrame;
			speed										-= fLastFrame * (rand() % 16);
			if(brightness < 0) {
				Particles.Remove(iShot);
				Brightness.remove_unordered(iShot--);
			}
		}
		return 0;
	}
};

struct SSolarSystem {
	::SDebris											SunFire							= {};
	::ced::SIntegrator3									World							= {};
	::SScene											Scene							= {};
	::ced::container<::SEntity>							Entities						= {};
	::ced::container<::ced::SImage>						Image							= {};
};

struct SApplication {
	::ced::SFramework									Framework						= {};
	::SSolarSystem										SolarSystem						= {};
};

																				//	- Mercury	- Venus		- Earth		- Mars		- Jupiter	- Saturn	- Uranus	- Neptune	- Pluto
static constexpr const double	PLANET_MASSES				[PLANET_COUNT]	=	{	0.330f		, 4.87f		, 5.97f		, 0.642f	, 1899		, 568		, 86.8f		, 102		, 0.0125f	};
static constexpr const double	PLANET_SCALES				[PLANET_COUNT]	=	{	4879		, 12104		, 12756		, 6792		, 142984	, 120536	, 51118		, 49528		, 2390		};
static constexpr const double	PLANET_DAY					[PLANET_COUNT]	=	{	4222.6f		, 2802.0f	, 23.9f		, 24.7f		, 9.9f		, 10.7f		, 17.2f		, 16.1f		, 153.3f	};
static constexpr const double	PLANET_AXIALTILT			[PLANET_COUNT]	=	{	0.01f		, 177.4f	, 23.4f		, 25.2f		, 3.1f		, 26.7f		, 97.8f		, 28.3f		, 122.5f	};
static constexpr const double	PLANET_DISTANCE				[PLANET_COUNT]	=	{	57.9f		, 108.2f	, 149.6f	, 227.9f	, 778.6f	, 1433.5f	, 2872.5f	, 4495.1f	, 5870.0f	};

static constexpr const double	PLANET_ORBITALPERIOD		[PLANET_COUNT]	=	{	88.0f	, 224.7f	, 365.2f	, 687.0f	, 4331		, 10747		, 30589		, 59800		, 90588		};
static constexpr const double	PLANET_ORBITALVELOCITY		[PLANET_COUNT]	=	{	47.9f	, 35.0f		, 29.8f		, 24.1f		, 13.1f		, 9.7f		, 6.8f		, 5.4f		, 4.7f		};
static constexpr const double	PLANET_ORBITALINCLINATION	[PLANET_COUNT]	=	{	7.0		, 3.4f		, 0.0f		, 1.9f		, 1.3f		, 2.5f		, 0.8f		, 1.8f		, 17.2f		};
static constexpr const double	PLANET_ORBITALECCENTRICITY	[PLANET_COUNT]	=	{	0.205f	, 0.007f	, 0.017f	, 0.094f	, 0.049f	, 0.057f	, 0.046f	, 0.011f	, 0.244f	};
static constexpr const char*	PLANET_IMAGE				[PLANET_COUNT]	=	{	"mercury_color.bmp"	, "venus_color.bmp"	, "earth_color.bmp"	, "mars_color.bmp"	, "jupiter_color.bmp"	, "saturn_color.bmp"	, "uranus_color.bmp"	, "neptune_color.bmp"	, "pluto_color.bmp"	};

static	int											drawDebris			(::ced::view_grid<::ced::SColorBGRA> targetPixels, SDebris & debris, const ::ced::SMatrix4<float> & matrixVPV, ::ced::view_grid<uint32_t> depthBuffer)	{
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		::ced::SColorFloat										colorShot			= debris.Colors[iParticle % ::std::size(debris.Colors)];
		::ced::SCoord3<float>									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		const ::ced::SCoord2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::ced::SColorFloat											starFinalColor	= colorShot * debris.Brightness[iParticle];
		starFinalColor.g										= ::std::max(0.0f, starFinalColor.g - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b										= ::std::max(0.0f, starFinalColor.b - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		//::ced::setPixel(targetPixels, pixelCoord, starFinalColor);
		const	double											brightRadius		= 2.0;
		const	double											brightRadiusSquared	= brightRadius * brightRadius;
		double													brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = (int32_t)-brightRadius - 1; y < (int32_t)brightRadius + 1; ++y)
		for(int32_t x = (int32_t)-brightRadius - 1; x < (int32_t)brightRadius + 1; ++x) {
			::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::ced::SCoord2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
				)
					continue;
				uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
				//if(depth > blendVal)
				//	continue;
				blendVal											= depth;
				double													finalBrightness					= 1.0-(brightDistance * brightUnit);
				::ced::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
				::ced::SColorFloat										pixelColor						= starFinalColor * finalBrightness + pixelVal;
				pixelVal											= pixelColor;
			}
		}
	}
	return 0;
}

int													cleanup							(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }
int													setup							(SApplication & app)	{
	::ced::SFramework										& framework						= app.Framework;
	::ced::frameworkSetup(framework);

	app.SolarSystem.Scene.Geometry.resize(1);
	::ced::geometryBuildSphere(app.SolarSystem.Scene.Geometry[0], 20U, 16U, 1, {});

	app.SolarSystem.Scene.Pivot.resize	(PLANET_COUNT + 1);
	app.SolarSystem.World.Spawn			(PLANET_COUNT * 2);
	::ced::SIntegrator3										& bodies						= app.SolarSystem.World;
	::SScene												& scene							= app.SolarSystem.Scene;
	::ced::SQuaternion<float>								axialTilt, orbitalInclination;

	for(uint32_t iModel = 0; iModel < app.SolarSystem.Scene.Pivot.size(); ++iModel) {
		int32_t													iPlanet							= iModel - 1;
		const float												scale							= float(1.0 / PLANET_SCALES[PLANET_EARTH] * PLANET_SCALES[iPlanet]) * 2;
		{ // Set up rigid body
			::SModelPivot										& model							= scene.Pivot[iModel];
			if(0 == iModel)
				model.Scale											= {10.f, 10.f, 10.f};
			else {
				model.Scale											= {scale, scale, scale};
			}
			model.Position										= {0, 0.5f};
		}
		if(iModel) { // Set up rigid body
			axialTilt.Identity();
			orbitalInclination.Identity();

			//::ced::SMass3											& orbitMass						= bodies.Masses		[iPlanet * 2]		= {};
			::ced::STransform3										& orbitTransform				= bodies.Transforms	[iPlanet * 2]		= {};
			::ced::SForce3											& orbitForces					= bodies.Forces		[iPlanet * 2]		= {};
			::ced::SMass3											& planetMass					= bodies.Masses		[iPlanet * 2 + 1]	= {};
			::ced::STransform3										& planetTransform				= bodies.Transforms	[iPlanet * 2 + 1]	= {};
			::ced::SForce3											& planetForces					= bodies.Forces		[iPlanet * 2 + 1]	= {};
			planetMass.InverseMass								= float(1.0 / PLANET_MASSES[iPlanet]);
			planetTransform.Position.x							= float(1.0 / PLANET_DISTANCE[PLANET_COUNT - 1] * PLANET_DISTANCE[iPlanet] * 2500);
			planetForces										= {};

			axialTilt.MakeFromEulerTaitBryan((float)(::ced::MATH_2PI / 360.0 * PLANET_AXIALTILT[iPlanet]), 0, 0);					// Calculate the axial inclination of the planet IN RADIANS
			planetTransform.Orientation							= axialTilt;										// Set the calculated axial tilt to the planet

			::ced::SCoord3<float>									rotatedRotation					= { 0.0f, -(float)(::ced::MATH_2PI / PLANET_DAY[iPlanet] * PLANET_DAY[PLANET_EARTH]), 0.0f };	// Calculate the rotation velocity of the planet IN EARTH DAYS
			rotatedRotation										= axialTilt.RotateVector( rotatedRotation );		// Rotate our calculated torque in relation to the planetary axis
			planetForces.Rotation								= rotatedRotation;		// Set the rotation velocity of the planet IN EARTH DAYS

			orbitForces.Rotation								= {0.0f, (float)(::ced::MATH_2PI / PLANET_ORBITALPERIOD[iPlanet]), 0.0f};			// Set the orbital rotation velocity IN EARTH DAYS
			orbitalInclination.MakeFromEulerTaitBryan( (float)(::ced::MATH_2PI / 360.0 * PLANET_ORBITALINCLINATION[iPlanet]), 0.0f, 0.0f );	// Calculate the orbital tilt IN RADIANS
			orbitTransform.Orientation							= orbitalInclination;								// Set the calculated inclination to the orbit
		}
	}

	::ced::SColorFloat										colors []						=
		{ ::ced::YELLOW
		, ::ced::DARKRED
		, ::ced::ORANGE
		, ::ced::BLUE
		, ::ced::LIGHTRED
		, ::ced::LIGHTGRAY
		, ::ced::LIGHTYELLOW
		, ::ced::GREEN
		, ::ced::DARKGRAY
		};

	app.SolarSystem.Image.resize(PLANET_COUNT + 1);
	::ced::bmpFileLoad("../ced_data/sun_color.bmp", app.SolarSystem.Image[0], true);
	for(uint32_t iPlanet = 0; iPlanet < PLANET_COUNT; ++iPlanet) {
		char														finalPath[256] = {};
		sprintf_s(finalPath, "../ced_data/%s", PLANET_IMAGE[iPlanet]);
		::ced::bmpFileLoad(finalPath, app.SolarSystem.Image[iPlanet + 1], true);
	}

	for(uint32_t iImage = 0; iImage < app.SolarSystem.Image.size(); ++iImage) {
		if(app.SolarSystem.Image[iImage].Pixels.size())
			continue;
		app.SolarSystem.Image[iImage].Metrics									= {24, 12};
		app.SolarSystem.Image[iImage].Pixels.resize(app.SolarSystem.Image[iImage].Metrics.x * app.SolarSystem.Image[iImage].Metrics.y);
		for(uint32_t y = 0; y < app.SolarSystem.Image[iImage].Metrics.y; ++y) // Generate noise color for planet texture
		for(uint32_t x = 0; x < app.SolarSystem.Image[iImage].Metrics.x; ++x) {
			app.SolarSystem.Image[iImage].Pixels[y * app.SolarSystem.Image[iImage].Metrics.x + x]		= colors[iImage % ::std::size(colors)];
		}
	}
	app.SolarSystem.Entities.push_back({-1, 0, 0, 0, -1});
	for(uint32_t iPlanet = 0; iPlanet < PLANET_COUNT; ++iPlanet) {
		const uint32_t											iBodyOrbit					= iPlanet * 2;
		const uint32_t											iBodyPlanet					= iBodyOrbit + 1;
		int32_t													iEntityOrbit				= app.SolarSystem.Entities.push_back({0, -1, -1, -1, (int32_t)iBodyOrbit});
		app.SolarSystem.Entities[0].IndexChild.push_back(iEntityOrbit);
		int32_t													iEntityPlanet				= app.SolarSystem.Entities.push_back({iEntityOrbit, 0, (int32_t)iPlanet + 1, (int32_t)iPlanet + 1, (int32_t)iBodyPlanet});
		app.SolarSystem.Entities[iEntityOrbit].IndexChild.push_back(iEntityPlanet);
	}

	// Update physics
	bodies.Integrate((365 * 4 + 1) * 10);

	app.SolarSystem.Scene.Camera.Target					= {};
	app.SolarSystem.Scene.Camera.Position				= {-0.000001f, 500, -1000};
	app.SolarSystem.Scene.Camera.Up						= {0, 1, 0};
	return 0;
}

int													updateEntityTransforms		(uint32_t iEntity, ::ced::container<::SEntity> & entities, SScene & scene, ::ced::SIntegrator3 & bodies)	{
	const SEntity											& entity					= entities[iEntity];
	::ced::SModelMatrices									matrices					= {};
	::ced::SMatrix4<float>									matrixBody					= {};
	bodies.GetTransform(entity.IndexBody, matrixBody);
	::ced::SMatrix4<float>									matrixTransform				= matrixBody;
	scene.Transform[iEntity]							= (-1 == entity.IndexParent) ? matrixTransform : matrixTransform * scene.Transform[entity.IndexParent];
	for(uint32_t iChild = 0; iChild < entity.IndexChild.size(); ++iChild) {
		const uint32_t											iChildEntity				= entity.IndexChild[iChild];
		updateEntityTransforms(iChildEntity, entities, scene, bodies);
	}
	return 0;
}
int													update						(SApplication & app)	{
	::ced::SFramework										& framework					= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running									= false;
	// ------------------------------------------- Handle input
	double													secondsLastFrame			= framework.Timer.ElapsedMicroseconds * .000001;

	// ------------------------------------------- Handle input
	if(GetAsyncKeyState('Q')) app.SolarSystem.Scene.Camera.Position.z				-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('E')) app.SolarSystem.Scene.Camera.Position.z				+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 100 : 10);
	if(GetAsyncKeyState('S')) app.SolarSystem.Scene.Camera.Position					+= app.SolarSystem.Scene.Camera.Position / app.SolarSystem.Scene.Camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('W')) app.SolarSystem.Scene.Camera.Position					-= app.SolarSystem.Scene.Camera.Position / app.SolarSystem.Scene.Camera.Position.Length() * (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame;
	if(GetAsyncKeyState('A')) app.SolarSystem.Scene.Camera.Position.RotateY( (GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);
	if(GetAsyncKeyState('D')) app.SolarSystem.Scene.Camera.Position.RotateY(-(GetAsyncKeyState(VK_SHIFT) ? 100 : 2) * secondsLastFrame);

	app.SolarSystem.SunFire.SpawnSpherical(100, {}, 5, 1, 10);

	// Update physics
	app.SolarSystem.SunFire.Update(secondsLastFrame * 2);
	::ced::SIntegrator3										& bodies						= app.SolarSystem.World;
	bodies.Integrate(secondsLastFrame);

	//------------------------------------------- Transform and Draw
	const ::ced::SCamera									& camera					= app.SolarSystem.Scene.Camera;
	::ced::view_grid<::ced::SColorBGRA>						targetPixels				= {framework.Pixels, framework.Window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColorBGRA) * targetPixels.size());
	::ced::SCoord3<float>									lightVector					= camera.Position;
	lightVector.Normalize();

	::ced::SMatrix4<float>									matrixView					= {};
	::ced::SMatrix4<float>									matrixProjection			= {};
	::ced::SMatrix4<float>									matrixViewport				= {};
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.1, 10000);
	matrixViewport.Viewport(targetPixels.metrics());
	matrixView											*= matrixProjection;
	matrixView											*= matrixViewport;

	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords					= {};
	::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights			= {};
	::ced::SModelMatrices									matrices					= {};
	::ced::view_grid<uint32_t>								depthBuffer					= {framework.DepthBuffer.begin(), app.Framework.Window.Size};
	::SScene												& scene						= app.SolarSystem.Scene;

	::ced::SMatrix4<float>									matrixBody					= {};
	scene.Transform.resize(app.SolarSystem.Entities.size());
	for(uint32_t iEntity = 0; iEntity < app.SolarSystem.Entities.size(); ++iEntity) {
		const ::SEntity											& entity					= app.SolarSystem.Entities[iEntity];
		if(-1 != entity.IndexParent)	// process root entities
			updateEntityTransforms(iEntity, app.SolarSystem.Entities, scene, bodies);
		else
			app.SolarSystem.Scene.Transform[iEntity].Identity();
	}

	::ced::container<::ced::SLight3>						lightPoints;
	::ced::container<::ced::SColorBGRA>						lightColors;
	lightPoints.push_back({{0,0,0}, 10000});
	lightColors.push_back(::ced::WHITE);

	for(uint32_t iEntity = 0; iEntity < app.SolarSystem.Entities.size(); ++iEntity) {
		::ced::SMatrix4<float>									matrixTransform				= scene.Transform[iEntity];
		const ::SEntity											& entity					= app.SolarSystem.Entities[iEntity];
		if(-1 == entity.IndexModel)
			continue;
		if(-1 == entity.IndexImage)
			continue;

		matrices.Scale		.Scale			(scene.Pivot[entity.IndexModel].Scale		, true);
		matrices.Position	.SetTranslation	(scene.Pivot[entity.IndexModel].Position	, true);
		::ced::view_grid<::ced::SColorBGRA>						entityImage					= app.SolarSystem.Image[entity.IndexImage];
		::ced::SGeometryTriangles								& entityGeometry			= scene.Geometry[entity.IndexGeometry];
		matrixTransform										= matrices.Scale * matrices.Position * matrixTransform;
		::ced::SMatrix4<float>									matrixTransformView			= matrixTransform * matrixView;
		for(uint32_t iTriangle = 0; iTriangle < entityGeometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawTriangle(targetPixels, entityGeometry, iTriangle, matrixTransform, matrixTransformView, lightVector, iEntity ? ::ced::BLACK : ::ced::WHITE, pixelCoords, pixelVertexWeights, entityImage, lightPoints, lightColors, depthBuffer);
		}
	}
	::drawDebris(targetPixels, app.SolarSystem.SunFire, matrixView, depthBuffer);

	return framework.Running ? 0 : 1;
}

int	WINAPI											WinMain
	(	HINSTANCE	// hInstance
	,	HINSTANCE	// hPrevInstance
	,	LPSTR		// lpCmdLine
	,	INT			// nShowCmd
	) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);
	SApplication										app;
	::setup(app);
	while(app.Framework.Running) {
		if(1 == ::update(app))
			break;
	}
	::cleanup(app);
	return 0;
}

