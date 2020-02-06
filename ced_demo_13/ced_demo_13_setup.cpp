#include "ced_demo_13_game.h"
#include <windows.h>

int													setupStars			(SStars & stars, ::ced::SCoord2<uint32_t> targetSize)	{
	if(0 == targetSize.y) return 0;
	if(0 == targetSize.x) return 0;
	stars.Speed		.resize(128);
	stars.Brightness.resize(128);
	stars.Position	.resize(128);
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		stars.Speed			[iStar]							= float(16 + (rand() % 64));
		stars.Brightness	[iStar]							= float(1.0 / RAND_MAX * rand());
		stars.Position		[iStar].y						= float(rand() % targetSize.y);
		stars.Position		[iStar].x						= float(rand() % targetSize.x);
	}
	return 0;
}

static	int											shipCreate			(::SSolarSystem & solarSystem, int32_t teamId, int32_t iGeometry, int32_t iImage)	{
	::SShipScene											& scene				= solarSystem.Scene;
	const uint32_t											countParts			= 6;

	SShip													ship				= {};
	{	// Create main ship entity
		::SEntity												entity				= {-1};

		::ced::SModel3											pivot				= {};
		pivot.Scale											= {1, 1, 1};

		entity												= {-1};
		entity.Geometry										= iGeometry % 5;
		entity.Transform									= scene.Transforms.push_back(pivot);
		entity.Image										= iImage % 5;
		entity.Body											= -1;

		ship.Entity											= solarSystem.Entities.push_back(entity);
		ship.Team											= teamId;

		const int32_t											indexBody			= solarSystem.ShipPhysics.Spawn();
		scene.ModelMatricesLocal	.push_back({});
		scene.ModelMatricesGlobal	.push_back({});
	}
	const int32_t											indexShip			= solarSystem.Ships.push_back(ship);
	ship.Parts.reserve(countParts);
	for(uint32_t iPart = 0; iPart < countParts; ++iPart) {	// Create child parts
		::ced::SModel3											pivot				= {};
		pivot.Scale											= {1, 1, 1};
		//pivot.Position										= {2.5, 0.5};
		//pivot.Position.RotateY(::ced::MATH_2PI / countParts * iPart);
		::SEntity												entityOrbit				= {ship.Entity};
		::SEntity												entityPart				= {-1};
		entityOrbit.Parent									= ship.Entity;
		entityOrbit.Geometry								= -1;
		entityOrbit.Transform								= scene.Transforms.push_back(pivot);
		entityOrbit.Image									= -1;
		entityOrbit.Body									= ::ced::createOrbiter(solarSystem.ShipPhysics
			, 1		//PLANET_MASSES				[iPlanet]
			, 2.5	//PLANET_DISTANCE			[iPlanet]
			, 0		//PLANET_AXIALTILT			[iPlanet]
			, 1		//PLANET_DAY				[iPlanet]
			, 1		//PLANET_DAY				[PLANET_EARTH]
			, 6		//PLANET_ORBITALPERIOD		[iPlanet]
			, 0		//PLANET_ORBITALINCLINATION	[iPlanet]
			, 1		//1.0 / PLANET_DISTANCE		[PLANET_COUNT - 1] * 2500
			);
		entityPart.Parent									= solarSystem.Entities.push_back(entityOrbit);
		entityPart.Geometry									= (iGeometry + (iPart % 2)) % 5;
		entityPart.Transform								= scene.Transforms.push_back(pivot);
		entityPart.Image									= (4 + iImage - (iPart % 2)) % 32;
		entityPart.Body										= entityOrbit.Body + 1;
		int32_t													indexEntityPart				= solarSystem.Entities.push_back(entityPart);
		solarSystem.Entities[entityPart.Parent].Children.push_back(indexEntityPart);
		solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation	= {};
		solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.MakeFromEulerTaitBryan(0, (float)(::ced::MATH_2PI / countParts * iPart), 0);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();

		::SShipPart												shipPart				= {};
		shipPart.Entity										= entityPart.Parent;
		::SEntity												& parentEntity			= solarSystem.Entities[ship.Entity];
 		parentEntity.Children.push_back(shipPart.Entity);

		::SShip													& parentShip			= solarSystem.Ships[indexShip];
		parentShip.Parts.push_back(shipPart);

		scene.ModelMatricesLocal	.push_back({});
		scene.ModelMatricesGlobal	.push_back({});
		scene.ModelMatricesLocal	.push_back({});
		scene.ModelMatricesGlobal	.push_back({});
	}
	return indexShip;
}

int													modelsSetup	(::SShipScene & scene)			{
	scene.Geometry.resize(5);
	//::ced::geometryBuildCube	(scene.Geometry[0]);
	::ced::geometryBuildCylinder	(scene.Geometry[0], 2U, 8U, .25f, {0, 0});
	::ced::geometryBuildSphere		(scene.Geometry[0], 8U, 5U, .7f, {0, 0});
	::ced::geometryBuildFigure0		(scene.Geometry[0], 2U, 8U, 1, {});

	::ced::geometryBuildCube	(scene.Geometry[1]);
	::ced::geometryBuildGrid	(scene.Geometry[1], {2U, 2U}, {1, 1}, {1, 1, 1});
	::ced::geometryBuildGrid	(scene.Geometry[1], {2U, 2U}, {-1, 1}, {-1, 1, 1});
	::ced::geometryBuildSphere	(scene.Geometry[2], 8U, 5U, .5f, {0, 0});
	::ced::geometryBuildFigure0	(scene.Geometry[2], 2U, 8U, 1, {});
	::ced::geometryBuildCube	(scene.Geometry[3]);
	::ced::geometryBuildSphere	(scene.Geometry[3], 4U, 2U, 1, {0, 0});
	::ced::geometryBuildSphere	(scene.Geometry[4], 6U, 2U, 1, {0, 0});

	{
		::ced::SColorFloat										baseColor	[]			=
			{ ::ced::LIGHTGREEN
			, ::ced::LIGHTBLUE
			, ::ced::LIGHTRED
			, ::ced::LIGHTCYAN
			, ::ced::LIGHTORANGE
			, ::ced::LIGHTYELLOW
			, ::ced::YELLOW
			, ::ced::RED
			, ::ced::BLUE
			, ::ced::BROWN
			, ::ced::GRAY
			, ::ced::PANOCHE
			, ::ced::TURQUOISE
			};

		scene.Image.resize(32);
		//::ced::bmpFileLoad("../ced_data/mars_color.bmp", solarSystem.Scene.Image[0], true);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			::ced::SImage											& image							= scene.Image[iImage];
			if(image.Pixels.size())
				continue;
			image.Resize(32, 5);
			for(uint32_t y = 0; y < image.Metrics.y; ++y) {// Generate noise color for planet texture
				bool													xAffect						= (y % 2);
				::ced::SColorFloat										lineColor					= baseColor[rand() % ::std::size(baseColor)];
				for(uint32_t x = 0; x < image.Metrics.x; ++x) {
					image.Pixels[y * image.Metrics.x + x]				= lineColor * (xAffect ? ::std::max(.25, sin(x * (1.0 / image.Metrics.x * ::ced::MATH_PI))) : 1);
				}
			}
		}
	}
	return 0;
}

int													solarSystemSetupBackgroundImage	(::ced::SImage & backgroundImage, ::ced::SCoord2<uint32_t> windowSize)	{
	backgroundImage.Resize(windowSize);
	const ::ced::SColorBGRA									colorBackground					= {0x20, 0x8, 0x4};
	//colorBackground									+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < windowSize.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < windowSize.x; ++x)
		backgroundImage.Pixels[y * windowSize.x + x]		= colorBackground;
	return 0;
}

int													stageSetup						(::SSolarSystem & solarSystem)	{	// Set up enemy ships
	static constexpr	const uint32_t						partHealthPlayer				= 3;
	static constexpr	const uint32_t						partHealthEnemy					= 10;

	struct SShipPart {
		SHIP_PART_TYPE	Type	;
		WEAPON_TYPE		Weapon;
		MUNITION_TYPE	Munition;
		double			MaxDelay;
		int32_t			Damage	;
	};

	static constexpr const SShipPart							weaponDefinitions		[]		=
		{ {SHIP_PART_TYPE_GUN	, WEAPON_TYPE_GUN		, MUNITION_TYPE_BULLET	, .1, 1}
		, {SHIP_PART_TYPE_GUN	, WEAPON_TYPE_SHOTGUN	, MUNITION_TYPE_BULLET	, .1, 1}
		, {SHIP_PART_TYPE_WAFER	, WEAPON_TYPE_LASER		, MUNITION_TYPE_RAY		, .3, 3}
		, {SHIP_PART_TYPE_WAFER	, WEAPON_TYPE_SHOTGUN	, MUNITION_TYPE_RAY		, .3, 3}
		, {SHIP_PART_TYPE_CANNON, WEAPON_TYPE_CANNON	, MUNITION_TYPE_SHELL	,  2, 3}
		, {SHIP_PART_TYPE_CANNON, WEAPON_TYPE_CANNON	, MUNITION_TYPE_FLARE	,  2, 3}
		, {SHIP_PART_TYPE_SHIELD, WEAPON_TYPE_ROCKET	, MUNITION_TYPE_FLARE	,  2, 3}
		, {SHIP_PART_TYPE_SHIELD, WEAPON_TYPE_SHIELD	, MUNITION_TYPE_FLARE	,  2, 3}
		, {SHIP_PART_TYPE_SILO	, WEAPON_TYPE_GUN		, MUNITION_TYPE_RAY		,  2, 3}
		};

	if(0 == solarSystem.Ships.size()) { // Create player ship
		const int32_t											indexShip						= ::shipCreate(solarSystem, 0, 0, 0);
		::SShip													& playerShip					= solarSystem.Ships[indexShip];
		::ced::SModel3											& shipPivot						= solarSystem.Scene.Transforms[solarSystem.Entities[playerShip.Entity].Transform];
		shipPivot.Rotation									= {0, 0, (float)(-::ced::MATH_PI_2)};
		shipPivot.Position									= {-30};
	}
	while(((int)solarSystem.Ships.size() - 2) < (int)solarSystem.Stage) {	// Create enemy ships depending on stage.
		int32_t													indexShip						= ::shipCreate(solarSystem, 1, solarSystem.Stage + solarSystem.Ships.size(), solarSystem.Stage + solarSystem.Ships.size());
		::SShip													& enemyShip						= solarSystem.Ships[indexShip];
		::ced::SModel3											& shipTransform					= solarSystem.Scene.Transforms[solarSystem.Entities[enemyShip.Entity].Transform];
		shipTransform.Rotation.z							= (float)(::ced::MATH_PI_2);
		shipTransform.Position								= {5.0f + 5 * solarSystem.Ships.size()};
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart)
			solarSystem.ShipPhysics.Forces[solarSystem.Entities[enemyShip.Parts[iPart].Entity].Body].Rotation.y	*= float(1 + indexShip * .35);
	}

	{ // Set up player ship
		::SShip													& playerShip					= solarSystem.Ships[0];
		for(uint32_t iPart = 0; iPart < playerShip.Parts.size(); ++iPart) {
			::SShipPart												& shipPart						= playerShip.Parts[iPart];
			shipPart.Health										= partHealthPlayer;
			shipPart.Type										= weaponDefinitions[(iPart % 2) ? 2 : 0].Type;
			shipPart.Shots.MaxDelay								= weaponDefinitions[(iPart % 2) ? 2 : 0].MaxDelay;
			shipPart.Shots.Weapon								= weaponDefinitions[(iPart % 2) ? 2 : 0].Weapon;
			shipPart.Shots.Type									= weaponDefinitions[(iPart % 2) ? 2 : 0].Munition;
			shipPart.Shots.Damage								= weaponDefinitions[(iPart % 2) ? 2 : 0].Damage;
			shipPart.Shots.Delay								= shipPart.Shots.MaxDelay / playerShip.Parts.size() * iPart;
		}
		playerShip.Health									= partHealthPlayer * playerShip.Parts.size();
	}
	{ // Set up enemy ships
		for(uint32_t iShip = 1; iShip < solarSystem.Ships.size(); ++iShip) {
			::SShip													& enemyShip					= solarSystem.Ships[iShip];
			for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart) {
				::SShipPart												& shipPart						= enemyShip.Parts[iPart];
				shipPart.Health										= partHealthEnemy;
				shipPart.Type										= weaponDefinitions[4].Type;
				shipPart.Shots.Weapon								= weaponDefinitions[4].Weapon;
				shipPart.Shots.MaxDelay								= weaponDefinitions[4].MaxDelay + iPart;
				shipPart.Shots.Type									= weaponDefinitions[4].Munition;
				shipPart.Shots.Damage								= weaponDefinitions[4].Damage;
				shipPart.Shots.Delay								= shipPart.Shots.MaxDelay / enemyShip.Parts.size() * iPart;
			}
			enemyShip.Health									= partHealthEnemy * enemyShip.Parts.size();
		}
	}
	++solarSystem.Stage;
	solarSystem.Slowing									= true;
	PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMSTART, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	return 0;
}

int													solarSystemSetup	(::SSolarSystem & solarSystem, ::ced::SCoord2<uint32_t> windowSize)	{
	::solarSystemSetupBackgroundImage(solarSystem.BackgroundImage, windowSize);
	::setupStars(solarSystem.Stars, windowSize);
	::SShipScene											& scene				= solarSystem.Scene;
	::modelsSetup(scene);
	::stageSetup(solarSystem);
	scene.Camera.Target									= {};
	scene.Camera.Position								= {-0.000001f, 135, 0};
	scene.Camera.Up										= {0, 1, 0};
	scene.Camera.Position.RotateZ(ced::MATH_PI * .25);
	return 0;
}
