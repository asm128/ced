#include "ced_demo_13.h"
#include "ced_collision.h"
#include <time.h>

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }

static	int											setupStars			(SStars & stars, ::ced::SCoord2<uint32_t> targetSize)	{
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

static	int											shipCreate			(::SSolarSystem & solarSystem, int32_t teamId, uint32_t partHealth, int32_t iGeometry, int32_t iImage)	{
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

		const uint32_t											entityHealth		= partHealth * countParts;
		ship.Entity											= solarSystem.Entities.push_back(entity);
		ship.Team											= teamId;
		ship.Health											= entityHealth;

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
		entityPart.Image									= (iGeometry - (iPart % 2)) % 10;
		entityPart.Body										= entityOrbit.Body + 1;
		int32_t													indexEntityPart				= solarSystem.Entities.push_back(entityPart);
		solarSystem.Entities[entityPart.Parent].Children.push_back(indexEntityPart);
		//solarSystem.ShipPhysics.Integrate(1.0);
		solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation	= {};
		solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.MakeFromEulerTaitBryan(0, (float)(::ced::MATH_2PI / countParts * iPart), 0);
		//solarSystem.ShipPhysics.Transforms[entityOrbit.Body].Orientation.Normalize();

		::SShipPart												shipPart				= {};
		shipPart.Health										= partHealth;
		shipPart.Entity										= entityPart.Parent;
		shipPart.Type										= SHIP_PART_TYPE_CANNON;
		shipPart.Shots.Delay								= 1.0 / countParts * iPart;
		shipPart.Shots.MaxDelay								= 1 + iPart;
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
		::ced::SColorFloat										baseColor	[4]			=
			{ ::ced::LIGHTGREEN
			, ::ced::LIGHTBLUE
			, ::ced::LIGHTRED
			, ::ced::LIGHTCYAN
			};

		scene.Image.resize(10);
		//::ced::bmpFileLoad("../ced_data/mars_color.bmp", solarSystem.Scene.Image[0], true);
		for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
			if(scene.Image[iImage].Pixels.size())
				continue;
			scene.Image[iImage].Metrics							= {16, 5};
			scene.Image[iImage].Pixels.resize(scene.Image[iImage].Metrics.x * scene.Image[iImage].Metrics.y);
			for(uint32_t y = 0; y < scene.Image[iImage].Metrics.y; ++y) {// Generate noise color for planet texture
				bool													xAffect						= (y % 2);
				::ced::SColorFloat										lineColor					= baseColor[rand() % ::std::size(baseColor)];
				for(uint32_t x = 0; x < scene.Image[iImage].Metrics.x; ++x) {
					scene.Image[iImage].Pixels[y * scene.Image[iImage].Metrics.x + x] = lineColor * (xAffect ? ::std::max(.5, sin(x)) : 1);
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
	static constexpr	const uint32_t						partHealthPlayer				= 1;
	static constexpr	const uint32_t						partHealthEnemy					= 10;
	if(0 == solarSystem.Ships.size()) {	// Set up player ship
		const int32_t											shipIndex						= ::shipCreate(solarSystem, 0, partHealthPlayer, 0, 0);
		::ced::SModel3											& shipPivot						= solarSystem.Scene.Transforms[solarSystem.Entities[solarSystem.Ships[shipIndex].Entity].Transform];
		shipPivot.Rotation									= {0, 0, (float)(-::ced::MATH_PI_2)};
		shipPivot.Position									= {-30};
		//::ced::STransform3										& shipTransform					= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[solarSystem.Ships[shipIndex].Entity].Body];
		//shipTransform.Orientation							= {0, 0, (float)(-::ced::MATH_PI_2), 0};
		//shipTransform.Position								= {-30};
		::SShip													& playerShip					= solarSystem.Ships[shipIndex];
		for(uint32_t iPart = 0; iPart < playerShip.Parts.size(); ++iPart) {
			playerShip.Parts[iPart].Shots.Delay					= 1.0 / playerShip.Parts.size() * iPart;
			playerShip.Parts[iPart].Shots.MaxDelay				= .1;
			playerShip.Parts[iPart].Shots.Type					= (iPart % 2) ? MUNITION_TYPE_RAY : MUNITION_TYPE_BULLET;
			playerShip.Parts[iPart].Type						= (iPart % 2) ? SHIP_PART_TYPE_LASER : SHIP_PART_TYPE_GUN;
			//playerShip.Parts[iPart].Shots.Damage				= 1;
		}
	}
	else {
		::SShip													& playerShip					= solarSystem.Ships[0];
		for(uint32_t iPart = 0; iPart < playerShip.Parts.size(); ++iPart) {
			playerShip.Parts[iPart].Shots.Delay					= 1.0 / playerShip.Parts.size() * iPart;
			playerShip.Parts[iPart].Shots.MaxDelay				= .1;
			playerShip.Parts[iPart].Health						= partHealthPlayer;
		}
		playerShip.Health									= partHealthPlayer * playerShip.Parts.size();
	}
	for(uint32_t iShip = 1; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& enemyShip					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart) {
			enemyShip.Parts[iPart].Shots.Delay					= 1.0 / enemyShip.Parts.size() * iPart;
			enemyShip.Parts[iPart].Shots.MaxDelay				= 1 + iPart;
			enemyShip.Parts[iPart].Health						= partHealthEnemy;
		}
		enemyShip.Health									= partHealthEnemy * enemyShip.Parts.size();
	}
	while(((int)solarSystem.Ships.size() - 2) < (int)solarSystem.Stage) {
		int32_t													indexShip				= ::shipCreate(solarSystem, 1, partHealthEnemy, solarSystem.Stage + solarSystem.Ships.size(), solarSystem.Stage + solarSystem.Ships.size());
		::ced::SModel3											& shipTransform			= solarSystem.Scene.Transforms[solarSystem.Entities[solarSystem.Ships[indexShip].Entity].Transform];
		shipTransform.Rotation.z							= (float)(::ced::MATH_PI_2);
		shipTransform.Position								= {5.0f + 5 * solarSystem.Ships.size()};
		::SShip													& enemyShip					= solarSystem.Ships[indexShip];
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart)
			solarSystem.ShipPhysics.Forces[solarSystem.Entities[enemyShip.Parts[iPart].Entity].Body].Rotation.y	*= float(1 + indexShip * .35);
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
	scene.Camera.Position								= {-0.000001f, 125, 0};
	scene.Camera.Up										= {0, 1, 0};
	scene.Camera.Position.RotateZ(ced::MATH_PI * .25);
	return 0;
}

int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);
	srand((uint32_t)time(0));
	::solarSystemSetup(app.SolarSystem, framework.Window.Size);
	return 0;
}

static	int											explosionAdd		(::ced::container<::SExplosion> & explosions, int32_t indexMesh, uint32_t triangleCount, const ::ced::SCoord3<float> &collisionPoint, double debrisSpeed) {
	::SExplosion											newExplosion				= {};
	newExplosion.IndexMesh								= indexMesh;
	newExplosion.Slices.resize(triangleCount / 6);
	for(uint32_t iSlice = 0, countSlices = newExplosion.Slices.size(); iSlice < countSlices; ++iSlice) {
		newExplosion.Slices[iSlice]							= {(uint16_t)iSlice, (uint16_t)(rand() % 4 + 3)};
		::ced::SCoord3<float>									direction					= {0, 1, 0};
		direction.RotateX(rand() * (::ced::MATH_2PI / RAND_MAX));
		direction.RotateY(rand() * (::ced::MATH_2PI / RAND_MAX));
		direction.RotateZ(rand() * (::ced::MATH_2PI / RAND_MAX));
		direction.Normalize();
		newExplosion.Particles.Spawn(collisionPoint, direction, (float)debrisSpeed);
	}
	for(uint32_t iExplosion = 0; iExplosion < explosions.size(); ++iExplosion) {
		SExplosion												& explosion					= explosions[iExplosion];
		if(0 == explosion.Slices.size()) {
			explosion											= newExplosion;
			return iExplosion;
		}
	}
	return explosions.push_back(newExplosion);
}

static	int											applyDamage
	( const int32_t						weaponDamage
	, int32_t							& healthPart
	, int32_t							& healthParent
	) {
	const uint32_t											finalDamage					= ::ced::min(weaponDamage, healthPart);
	healthPart											-= finalDamage;
	healthParent										-= finalDamage;
	return 0 >= healthPart;
}

static	int											collisionDetect		(::SShots & shots, const ::ced::SCoord3<float> & modelPosition, ::ced::container<::ced::SCoord3<float>> & collisionPoints)	{
	bool													detected			= false;
	collisionPoints.clear();
	for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
		const ::ced::SLine3<float>								shotSegment			= {shots.PositionPrev[iShot], shots.Particles.Position[iShot]};
		float													t					= 0;
		::ced::SCoord3<float>									collisionPoint		= {};
		if( ::ced::intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), {modelPosition, 1.2}, t, collisionPoint)
			&& t < 1
		) {
			detected											= true;
			collisionPoints.push_back(collisionPoint);
			shots.Remove(iShot);
			--iShot;
		}
	}
	return 0;
}

int													handleCollisionPoint	(SSolarSystem & solarSystem, int32_t weaponDamage, ::SShipPart& damagedPart, SShip & damagedShip, const ::ced::SCoord3<float> & sphereCenter, const ::ced::SCoord3<float> & collisionPoint, void* soundAlias)	{
	PlaySound((LPCTSTR)soundAlias, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	const ::ced::SCoord3<float>							bounceVector		= (collisionPoint - sphereCenter).Normalize();
	solarSystem.Debris.SpawnDirected(5, bounceVector, collisionPoint, 50, 1);
	if(::applyDamage(weaponDamage, damagedPart.Health, damagedShip.Health)) {	// returns true if health reaches zero
		const ::SEntity										& entityGeometry	= solarSystem.Entities[damagedPart.Entity + 1];
		const int32_t										indexMesh			= entityGeometry.Geometry;
		const uint32_t										countTriangles		= solarSystem.Scene.Geometry[indexMesh].Triangles.size();
		::explosionAdd(solarSystem.Explosions, indexMesh, countTriangles, collisionPoint, 60);
		solarSystem.Debris.SpawnSpherical(30, collisionPoint, 60, 2);
		if(0 >= damagedShip.Health) {
			const ::ced::SCoord3<float>							& parentPosition	= solarSystem.Scene.Transforms[solarSystem.Entities[damagedShip.Entity].Transform].Position;
			::explosionAdd(solarSystem.Explosions, indexMesh, countTriangles, parentPosition, 13);
			solarSystem.Debris.SpawnSpherical(150, parentPosition, 13, 2.8f);
			solarSystem.Slowing								= true;
			solarSystem.TimeScale							= 1.0;
		}
		return 1;
	}
	return 0;
}

int													updateEntityTransforms		(uint32_t iEntity, ::ced::container<::SEntity> & entities, ::SShipScene & scene, ::ced::SIntegrator3 & bodies)	{
	const SEntity											& entity					= entities[iEntity];
	if(-1 == entity.Body)
		scene.ModelMatricesGlobal[iEntity]					= (-1 == entity.Parent) ? scene.ModelMatricesLocal[iEntity] : scene.ModelMatricesLocal[iEntity] * scene.ModelMatricesGlobal[entity.Parent];
	else {
		::ced::SMatrix4<float>									& matrixGlobal					= scene.ModelMatricesGlobal[iEntity];
		bodies.GetTransform(entity.Body, matrixGlobal);
		matrixGlobal										= scene.ModelMatricesLocal[iEntity] * matrixGlobal;
		if(-1 != entity.Parent)
			matrixGlobal										*= scene.ModelMatricesGlobal[entity.Parent];
	}
	for(uint32_t iChild = 0; iChild < entity.Children.size(); ++iChild) {
		const uint32_t											iChildEntity				= entity.Children[iChild];
		::updateEntityTransforms(iChildEntity, entities, scene, bodies);
	}
	return 0;
}

int													updateShots				(SSolarSystem & solarSystem, double secondsLastFrame)	{
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart)
			ship.Parts[iPart].Shots.Update((float)secondsLastFrame);
	}

	::ced::container<::ced::SCoord3<float>>					collisionPoints;
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& ship					= solarSystem.Ships[iShip];
		for(uint32_t iShip2 = 0; iShip2 < solarSystem.Ships.size(); ++iShip2) {
			::SShip													& ship2					= solarSystem.Ships[iShip2];
			if(ship2.Health <= 0 || ship.Team == ship2.Team)
				continue;
			for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
				::SShipPart												& shipPart				= ship.Parts[iPart];
				void													* soundAlias			= (iShip2 ? (void*)SND_ALIAS_SYSTEMHAND : (void*)SND_ALIAS_SYSTEMEXCLAMATION);
				for(uint32_t iPart2 = 0; iPart2 < ship2.Parts.size(); ++iPart2) {
					::SShipPart												& shipPart2				= ship2.Parts[iPart2];
					if(shipPart2.Health <= 0)
						continue;
					::SEntity												& entity					= solarSystem.Entities[shipPart2.Entity];
					if(-1 != entity.Geometry) {
						const ::ced::SMatrix4<float>							matrixTransform			= solarSystem.Scene.ModelMatricesGlobal[shipPart2.Entity];
						const ::ced::SCoord3<float>								entityPosition			= matrixTransform.GetTranslation();
						::collisionDetect(shipPart.Shots, entityPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, shipPart.Shots.Damage, shipPart2, ship2, entityPosition, collisionPoints[iCollisionPoint], soundAlias))	// returns true if part health reaches zero.
								break;
					}
					for(uint32_t iEntity = 0; iEntity < entity.Children.size(); ++iEntity) {
						::SEntity												& entityChild				= solarSystem.Entities[entity.Children[iEntity]];
						if(-1 == entityChild.Parent)
							continue;
						if(-1 == entityChild.Geometry)
							continue;
						const ::ced::SMatrix4<float>							matrixTransform			= solarSystem.Scene.ModelMatricesGlobal[entity.Children[iEntity]];
						const ::ced::SCoord3<float>								entityPosition			= matrixTransform.GetTranslation();
						::collisionDetect(shipPart.Shots, entityPosition, collisionPoints);
						for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
							if(::handleCollisionPoint(solarSystem, shipPart.Shots.Damage, shipPart2, ship2, entityPosition, collisionPoints[iCollisionPoint], soundAlias))	// returns true if part health reaches zero.
								break;
					}
				}
			}
		}
	}
	return 0;
}
int													updateShipPart			(SSolarSystem & solarSystem, ::SShipPart & shipPart, double secondsLastFrame)	{
	::ced::SModel3											& modelPlayer			= solarSystem.Scene.Transforms[solarSystem.Entities[0].Transform];
	shipPart.Shots.Delay								+= secondsLastFrame;// * .1 * (1 + iPart);

	::SEntity												& entityPart			= solarSystem.Entities[shipPart.Entity];
	::ced::SModel3											& partTransform			= solarSystem.Scene.Transforms[entityPart.Transform + 1];

	//partTransform.Rotation.y							+= (float)(secondsLastFrame * 1);
	::ced::SCoord3<float>									positionGlobal			= solarSystem.Scene.ModelMatricesGlobal[shipPart.Entity + 1].Transform(partTransform.Position);
	for(uint32_t iParticle = 0; iParticle < shipPart.Shots.Particles.Position.size(); ++iParticle)
		shipPart.Shots.Particles.Position[iParticle].x		-= (float)(solarSystem.RelativeSpeedCurrent * secondsLastFrame * .2);
	if(shipPart.Type == SHIP_PART_TYPE_CANNON) {
		if(1 < (modelPlayer.Position - positionGlobal).LengthSquared()) {
			::ced::SCoord3<float>									direction			= modelPlayer.Position - positionGlobal;
			direction.RotateY(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			shipPart.Shots.Spawn(positionGlobal, direction.Normalize(), 25, 1);
		}
	}
	else if(shipPart.Type == SHIP_PART_TYPE_LASER) {
		::ced::SCoord3<float>									direction				= {1, 0, 0};
		//direction.RotateY(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
		//direction.RotateZ(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
		shipPart.Shots.Spawn(positionGlobal, direction, 500, .2f);
	}
	else if(shipPart.Type == SHIP_PART_TYPE_GUN) {
		::ced::SCoord3<float>									direction				= {1, 0, 0};
		direction.RotateY(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .01 * ((rand() % 2) ? -1 : 1));
		direction.RotateZ(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .01 * ((rand() % 2) ? -1 : 1));
		shipPart.Shots.Spawn(positionGlobal, direction, 100, .2f);
	}
	return 0;
}

int													solarSystemUpdate				(SSolarSystem & solarSystem, double secondsLastFrame, ::ced::SCoord2<uint32_t> screenSize)	{
	//------------------------------------------- Handle input
	double													speed							= 10;
	secondsLastFrame									= ::std::min(secondsLastFrame, 0.3);
	if(solarSystem.Slowing) {
		solarSystem.TimeScale								-= secondsLastFrame * .35;
		if(solarSystem.TimeScale < .1)
			solarSystem.Slowing									= false;
	}
	else {
		if(solarSystem.TimeScale < .99)
			solarSystem.TimeScale							= ::ced::min(1.0, solarSystem.TimeScale += secondsLastFrame * .45);
	}
	secondsLastFrame									*= solarSystem.TimeScale;
	solarSystem.AnimationTime							+= secondsLastFrame;

	solarSystem.ShipPhysics.Integrate(secondsLastFrame);
	{
		const int32_t randDebris	= rand();
		solarSystem.Debris.Spawn({200.0f, ((randDebris % 2) ? -1.0f : 1.0f) * (randDebris % 100), ((randDebris % 2) ? -1.0f : 1.0f) * (randDebris % 400)}, {-1, 0, 0}, 400, 2);
	}


	if(GetAsyncKeyState('Q')) solarSystem.Scene.Camera.Position.y	-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) solarSystem.Scene.Camera.Position.y	+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	::ced::SModel3											& modelPlayer			= solarSystem.Scene.Transforms[solarSystem.Entities[0].Transform];
	{
		//::ced::STransform3											& playerBody						= solarSystem.ShipPhysics.Transforms[solarSystem.Entities[solarSystem.Ships[0].Entity].Body];
		//if(GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP		)) playerBody.Position.x			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
		//if(GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN	)) playerBody.Position.x			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
		//if(GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT	)) playerBody.Position.z			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
		//if(GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT	)) playerBody.Position.z			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
			 if(GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP		)) { modelPlayer.Position.x			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); solarSystem.AccelerationControl	= +1; }
		else if(GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN	)) { modelPlayer.Position.x			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); solarSystem.AccelerationControl	= -1; }
		else
			solarSystem.AccelerationControl	= 0;

		if(GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT	)) { modelPlayer.Position.z			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); }
		if(GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT	)) { modelPlayer.Position.z			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8)); }
	}

	if(solarSystem.Scene.Camera.Position.y > 0.001f)
	if(solarSystem.Scene.Camera.Position.y > 0.001f) if(GetAsyncKeyState(VK_HOME)) solarSystem.Scene.Camera.Position.RotateZ(ced::MATH_PI * secondsLastFrame);
	if(solarSystem.Scene.Camera.Position.x < 0.001f)
	if(solarSystem.Scene.Camera.Position.x < 0.001f) if(GetAsyncKeyState(VK_END	)) solarSystem.Scene.Camera.Position.RotateZ(ced::MATH_PI * -secondsLastFrame);



	if(solarSystem.Scene.Camera.Position.y < 0) solarSystem.Scene.Camera.Position.y = 0.0001f;
	if(solarSystem.Scene.Camera.Position.y < 0) solarSystem.Scene.Camera.Position.y = 0.0001f;
	if(solarSystem.Scene.Camera.Position.x > 0) solarSystem.Scene.Camera.Position.x = -0.0001f;
	if(solarSystem.Scene.Camera.Position.x > 0) solarSystem.Scene.Camera.Position.x = -0.0001f;

	if(GetAsyncKeyState(VK_NUMPAD5))
		modelPlayer.Rotation									= {0, 0, (float)-::ced::MATH_PI_2};
	else {
		if(GetAsyncKeyState(VK_NUMPAD8)) modelPlayer.Rotation.z		-= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD2)) modelPlayer.Rotation.z		+= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD6)) modelPlayer.Rotation.x		-= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD4)) modelPlayer.Rotation.x		+= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	}

	//solarSystem.Scene.Camera.Position	= modelPlayer.Position;
	//solarSystem.Scene.Camera.Position.x	-= 1.5;
	////solarSystem.Scene.Camera.Position.x	-= 50;
	////solarSystem.Scene.Camera.Position.y	+= 10;
	//solarSystem.Scene.Camera.Target		= solarSystem.Scene.Camera.Position;
	//solarSystem.Scene.Camera.Target.x	+= 1;

	::SShipScene											& scene						= solarSystem.Scene;
	::ced::SModelMatrices									matrices					= {};
	for(uint32_t iTransform = 0; iTransform < solarSystem.Scene.Transforms.size(); ++iTransform) {
		::ced::SModel3											& model				= scene.Transforms[iTransform];
		matrices.Scale		.Scale			(model.Scale	, true);
		matrices.Rotation	.Rotation		(model.Rotation);
		matrices.Position	.SetTranslation	(model.Position, true);
		scene.ModelMatricesLocal[iTransform]				= matrices.Scale * matrices.Rotation * matrices.Position;
	}
	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		const ::SEntity											& entity					= solarSystem.Entities[iEntity];
		if(-1 == entity.Parent)	// process root entities
			updateEntityTransforms(iEntity, solarSystem.Entities, solarSystem.Scene, solarSystem.ShipPhysics);
	}

	bool													playing					= false;
	static constexpr const double							frameStep				= 0.005;
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& enemyShip				= solarSystem.Ships[iShip];
		if(0 >= enemyShip.Health)
			continue;

		::ced::SModel3											& shipTransform			= solarSystem.Scene.Transforms[solarSystem.Entities[enemyShip.Entity].Transform];
		if(iShip) {
			shipTransform.Position.z							= (float)(sin(iShip + solarSystem.AnimationTime) * (iShip * 5.0) * ((iShip % 2) ? -1 : 1));
			shipTransform.Position.x							= (float)(iShip * 5.0) - (solarSystem.Stage / 2);
			double													timeWaveVertical					= .1;
			if(0 == (solarSystem.Stage % 7)) {
				if(iShip % 2)
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.AnimationTime) * ((solarSystem.Ships.size() - 1 - iShip) * 4.0) * ((iShip % 2) ? -1 : 1));
				else
					shipTransform.Position.z							= (float)(sin(iShip + solarSystem.AnimationTime) * (iShip * 4.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .65;
				else if(0 == (iShip % 3)) timeWaveVertical	= .80;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.Stage % 5)) {
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.AnimationTime) * ((solarSystem.Ships.size() - 1 - iShip) * 3.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .75;
				else if(0 == (iShip % 7)) timeWaveVertical	= .80;
			}
			else if(0 == (solarSystem.Stage % 3)) {
					shipTransform.Position.z							= (float)(cos(iShip + solarSystem.AnimationTime) * ((solarSystem.Ships.size() - 1 - iShip) * 2.0) * ((iShip % 2) ? -1 : 1));
					 if(0 == (iShip % 2)) timeWaveVertical	= .25;
				else if(0 == (iShip % 3)) timeWaveVertical	= .50;
				else if(0 == (iShip % 7)) timeWaveVertical	= .75;
			}
			else {
					 if(0 == (iShip % 2)) timeWaveVertical	= .50;
				else if(0 == (iShip % 3)) timeWaveVertical	= .25;
				else if(0 == (iShip % 7)) timeWaveVertical	= .15;
			}
			shipTransform.Position.x								+= (float)(sin(solarSystem.AnimationTime * timeWaveVertical * ::ced::MATH_2PI) * ((iShip % 2) ? -1 : 1));
		}
		if(iShip)
			playing												= true;
		double													secondsToProcess		= secondsLastFrame;

		while(secondsToProcess > frameStep) {
			for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart) {
				::SShipPart												& shipPart				= enemyShip.Parts[iPart];
				if(0 >= shipPart.Health)
					continue;
				::updateShipPart(solarSystem, shipPart, frameStep);
			}
			secondsToProcess									-= frameStep;
		}
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart) {
			::SShipPart												& shipPart				= enemyShip.Parts[iPart];
			if(0 >= shipPart.Health)
				continue;
			::updateShipPart(solarSystem, shipPart, secondsToProcess);
		}
	}
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			::SShipPart												& shipPart				= ship.Parts[iPart];
			shipPart.Shots.PositionDraw.resize(shipPart.Shots.Particles.Position.size());
			memcpy(shipPart.Shots.PositionDraw.begin(), shipPart.Shots.Particles.Position.begin(), shipPart.Shots.Particles.Position.size() * sizeof(::ced::SCoord3<float>));
		}
	}
	double													secondsToProcess		= secondsLastFrame;
	while(secondsToProcess > frameStep) {
		::updateShots(solarSystem, frameStep);
		secondsToProcess									-= frameStep;
	}
	::updateShots(solarSystem, secondsToProcess);
	solarSystem.Debris		.Update((float)secondsLastFrame);
	solarSystem.Stars		.Update(screenSize.y, (float)secondsLastFrame);
	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion)
		solarSystem.Explosions[iExplosion].Update((float)secondsLastFrame);

	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion)
		for(uint32_t iParticle = 0; iParticle < solarSystem.Explosions[iExplosion].Particles.Position.size(); ++iParticle) {
			solarSystem.Explosions[iExplosion].Particles.Position[iParticle].x	-= (float)(solarSystem.RelativeSpeedCurrent * secondsLastFrame);
		}
	for(uint32_t iParticle = 0; iParticle < solarSystem.Debris.Particles.Position.size(); ++iParticle)
		solarSystem.Debris.Particles.Position[iParticle].x	-= (float)(solarSystem.RelativeSpeedCurrent * secondsLastFrame);

	if(solarSystem.AccelerationControl == 0) {
		if(solarSystem.RelativeSpeedCurrent > solarSystem.RelativeSpeedTarget)
			--solarSystem.RelativeSpeedCurrent;
		else if(solarSystem.RelativeSpeedCurrent < solarSystem.RelativeSpeedTarget)
			++solarSystem.RelativeSpeedCurrent;
	}
	else if(solarSystem.AccelerationControl > 0) {
		++solarSystem.RelativeSpeedCurrent;
	}
	else if(solarSystem.AccelerationControl < 0) {
		--solarSystem.RelativeSpeedCurrent;
	}

	if(false == playing) {
		playing												= false;
		for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
			if(solarSystem.Explosions[iExplosion].Slices.size()) {
				playing											= true;
				break;
			}
		}
		if(false == playing)  {	// Set up enemy ships
			::stageSetup(solarSystem);
			playing												= true;
		}
	}
	//solarSystem.Scene.LightVector									= solarSystem.Scene.LightVector.RotateY(secondsLastFrame * 2);
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running = false;
	if(framework.Window.Resized) {
		::ced::SMatrix4<float>									& matrixProjection	= app.SolarSystem.Scene.MatrixProjection;
		matrixProjection.FieldOfView(::ced::MATH_PI * .25, framework.Window.Size.x / (double)framework.Window.Size.y, 0.01, 500);
		::ced::SMatrix4<float>									matrixViewport		= {};
		matrixViewport.Viewport(framework.Window.Size);
		matrixProjection									*= matrixViewport;

		::solarSystemSetupBackgroundImage(app.SolarSystem.BackgroundImage, framework.Window.Size);
		::setupStars(app.SolarSystem.Stars, framework.Window.Size);
	}
	double													secondsLastFrame	= framework.Timer.ElapsedMicroseconds * .001;
	secondsLastFrame									*= .001;
	::solarSystemUpdate(app.SolarSystem, secondsLastFrame, framework.Window.Size);
	::draw(app);
	Sleep(1);
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
