#include "ced_demo_12.h"
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
	uint32_t												countParts			= 6;

	SShip													ship				= {};
	{	// Create main ship entity

		::SEntity												entity				= {-1};

		::ced::SModel3											transform			= {};
		transform.Scale										= {1 , 1, 1};

		entity												= {-1};
		entity.Geometry										= iGeometry % 5;
		entity.Transform									= scene.Transforms.push_back(transform);
		entity.Image										= iImage % 5;
		entity.Body											= -1;

		const uint32_t											entityHealth		= partHealth * countParts;
		ship.Entity											= solarSystem.Entities.push_back(entity);
		ship.Team											= teamId;
		ship.Health											= entityHealth;

		scene.ModelMatricesLocal	.push_back({});
		scene.ModelMatricesGlobal	.push_back({});
	}
	const int32_t											indexShip			= solarSystem.Ships.push_back(ship);
	ship.Parts.reserve(countParts);
	for(uint32_t iPart = 0; iPart < countParts; ++iPart) {	// Create child parts

		::ced::SModel3											transform				= {};
		transform.Scale											= {1, 1, 1};
		transform.Position										= {2.5, 0.5};
		transform.Position.RotateY(::ced::MATH_2PI / countParts * iPart);

		::SEntity												entity					= {ship.Entity};
		entity.Parent										= ship.Entity;
		entity.Geometry										= (iGeometry + (iPart % 2)) % 5;
		entity.Transform									= scene.Transforms.push_back(transform);
		entity.Image										= (iGeometry - (iPart % 2)) % 10;
		entity.Body											= -1;

		::SShipPart												shipPart				= {};
		shipPart.Health										= partHealth;
		shipPart.Entity										= solarSystem.Entities.push_back(entity);
		shipPart.Shots.Delay								= 1.0 / countParts * iPart;
		::SEntity												& parentEntity			= solarSystem.Entities[ship.Entity];
 		parentEntity.Children.push_back(shipPart.Entity);

		::SShip													& parentShip			= solarSystem.Ships[indexShip];
		parentShip.Parts.push_back(shipPart);

		scene.ModelMatricesLocal	.push_back({});
		scene.ModelMatricesGlobal	.push_back({});
	}
	return indexShip;
}

int													modelsSetup	(::SShipScene & scene)			{
	scene.Geometry.resize(5);
	//::ced::geometryBuildCube	(scene.Geometry[0]);
	::ced::geometryBuildSphere	(scene.Geometry[0], 8U, 5U, .7f, {0, 0});
	::ced::geometryBuildFigure0	(scene.Geometry[0], 2U, 8U, 1, {});

	::ced::geometryBuildSphere	(scene.Geometry[1], 8U, 5U, .5f, {0, 0});
	::ced::geometryBuildFigure0	(scene.Geometry[1], 2U, 8U, 1, {});
	::ced::geometryBuildCube	(scene.Geometry[2]);
	::ced::geometryBuildGrid	(scene.Geometry[2], {2U, 2U}, {1U, 1U});
	::ced::geometryBuildCube	(scene.Geometry[3]);
	::ced::geometryBuildSphere	(scene.Geometry[3], 4U, 2U, 1, {0, 0});
	::ced::geometryBuildSphere	(scene.Geometry[4], 16U, 2U, 1, {0, 0});

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
		::ced::SModel3											& shipTransform					= solarSystem.Scene.Transforms[solarSystem.Entities[solarSystem.Ships[shipIndex].Entity].Transform];
		shipTransform.Rotation.z							= (float)(-::ced::MATH_PI_2);
		shipTransform.Position								= {-30};
	}
	else {
		::SShip													& playerShip					= solarSystem.Ships[0];
		for(uint32_t iPart = 0; iPart < playerShip.Parts.size(); ++iPart) {
			playerShip.Parts[iPart].Shots.Delay					= 1.0 / playerShip.Parts.size() * iPart;
			playerShip.Parts[iPart].Health						= partHealthPlayer;
		}
		playerShip.Health									= partHealthPlayer * playerShip.Parts.size();
	}
	for(uint32_t iShip = 1; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& enemyShip					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart) {
			enemyShip.Parts[iPart].Shots.Delay					= 1.0 / enemyShip.Parts.size() * iPart;
			enemyShip.Parts[iPart].Health						= partHealthEnemy;
		}
		enemyShip.Health									= partHealthEnemy * enemyShip.Parts.size();
	}
	int32_t													indexShip				= ::shipCreate(solarSystem, 1, partHealthEnemy, solarSystem.Stage + solarSystem.Ships.size(), solarSystem.Stage + solarSystem.Ships.size());
	::ced::SModel3											& shipTransform			= solarSystem.Scene.Transforms[solarSystem.Entities[solarSystem.Ships[indexShip].Entity].Transform];
	shipTransform.Rotation.z							= (float)(::ced::MATH_PI_2);
	shipTransform.Position								= {5.0f + 5 * solarSystem.Ships.size()};
	++solarSystem.Stage;
	solarSystem.Slowing									= true;
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
	newExplosion.Slices.reserve(triangleCount / 6);
	for(uint32_t iSlice = 0, countSlices = newExplosion.Slices.size(); iSlice < countSlices; ++iSlice) {
		newExplosion.Slices.push_back({(uint16_t)iSlice, (uint16_t)(rand() % 4 + 3)});
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
	healthParent										-= finalDamage;
	healthPart											-= finalDamage;
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
	PlaySoundA((LPCSTR)soundAlias, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	const ::ced::SCoord3<float>							bounceVector		= (collisionPoint - sphereCenter).Normalize();
	solarSystem.Debris.SpawnDirected(5, bounceVector, collisionPoint, 50, 1);
	if(::applyDamage(weaponDamage, damagedPart.Health, damagedShip.Health)) {	// returns true if health reaches zero
		const ::SEntity										& entityParent		= solarSystem.Entities[damagedShip.Entity];
		const int32_t										indexMesh			= entityParent.Geometry;
		const uint32_t										countTriangles		= solarSystem.Scene.Geometry[indexMesh].Triangles.size();
		::explosionAdd(solarSystem.Explosions, indexMesh, countTriangles, collisionPoint, 50);
		solarSystem.Debris.SpawnSpherical(30, collisionPoint, 60, 2);
		if(0 >= damagedShip.Health) {
			const ::ced::SCoord3<float>							& parentPosition	= solarSystem.Scene.Transforms[entityParent.Transform].Position;
			::explosionAdd(solarSystem.Explosions, indexMesh, countTriangles, parentPosition, 10);
			solarSystem.Debris.SpawnSpherical(150, parentPosition, 13, 2.8f);
			solarSystem.Slowing								= true;
			solarSystem.TimeScale							= 1.0;
		}
		return 1;
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
	::ced::SModelMatrices									matrices;
	solarSystem.Scene.ModelMatricesLocal.resize(solarSystem.Scene.Transforms.size());
	for(uint32_t iTransform = 0; iTransform < solarSystem.Scene.Transforms.size(); ++iTransform) {
		::ced::SModel3											& model				= solarSystem.Scene.Transforms[iTransform];
		matrices.Scale		.Scale			(model.Scale	, true);
		matrices.Rotation	.Rotation		(model.Rotation);
		matrices.Position	.SetTranslation	(model.Position, true);
		solarSystem.Scene.ModelMatricesLocal[iTransform]				= matrices.Scale * matrices.Rotation * matrices.Position;
	}

	::ced::SModelMatrices									matricesParent;
	::ced::SModel3											& modelPlayer			= solarSystem.Scene.Transforms[solarSystem.Entities[0].Transform];

	bool													playing					= false;
	for(uint32_t iShip = 1; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& enemyShip				= solarSystem.Ships[iShip];
		::ced::SModel3											& shipTransform			= solarSystem.Scene.Transforms[solarSystem.Entities[enemyShip.Entity].Transform];
		shipTransform.Position.z							= (float)(sin(iShip + solarSystem.AnimationTime) * (iShip * 5.0) * ((iShip % 2) ? -1 : 1));
		if(0 >= enemyShip.Health)
			continue;
		playing												= true;
		for(uint32_t iPart = 0; iPart < enemyShip.Parts.size(); ++iPart) {
			::SShipPart												& shipPart				= enemyShip.Parts[iPart];
			::SEntity												& entityPart			= solarSystem.Entities[shipPart.Entity];
			const int32_t											indexParent				= entityPart.Parent;
			if(0 >= shipPart.Health)
				continue;
			::ced::SModel3											& partTransform			= solarSystem.Scene.Transforms[entityPart.Transform];
			partTransform.Rotation.y							+= (float)(secondsLastFrame * 1);
			matricesParent										= {};
			shipPart.Shots.Delay								+= secondsLastFrame * .1 * (1 + iPart);
			::ced::SCoord3<float>									positionGlobal			= solarSystem.Scene.ModelMatricesLocal[indexParent].Transform(partTransform.Position);
			if(1 < (modelPlayer.Position - positionGlobal).LengthSquared()) {
				::ced::SCoord3<float>									direction			= modelPlayer.Position - positionGlobal;
				direction.RotateY(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
				shipPart.Shots.Spawn(positionGlobal, direction.Normalize(), 25, 1);
			}
		}
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

	//if(GetAsyncKeyState(VK_SPACE))
	if(playing)
	{
		for(uint32_t iPart = 0; iPart < solarSystem.Ships[0].Parts.size(); ++iPart) {
			::SShipPart												& shipPart				= solarSystem.Ships[0].Parts[iPart];
			if(0 >= shipPart.Health)
				continue;
			matricesParent										= {};
			const int32_t											indexParent				= solarSystem.Entities[shipPart.Entity].Parent;
			::ced::SModel3											& modelEnemy			= solarSystem.Scene.Transforms[shipPart.Entity];
			::ced::SCoord3<float>									positionGlobal			= solarSystem.Scene.ModelMatricesLocal[indexParent].Transform(modelEnemy.Position);
			shipPart.Shots.Delay								+= secondsLastFrame * 7.5;// * (1 + iPart);;
			::ced::SCoord3<float>									direction				= {1, 0, 0};
			//direction.RotateY(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			//direction.RotateZ(rand() * (1.0 / RAND_MAX) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			shipPart.Shots.Spawn(positionGlobal, direction, 100, .2f);
		}
	}
	//solarSystem.Scene.Camera.Position								= solarSystem.Scene.Transforms[0].Position;
	//solarSystem.Scene.Camera.Position.y								+= 10 * 4;
	//solarSystem.Scene.Camera.Position.x								-= 20 * 4;
	//solarSystem.Scene.Camera.Target.z								= solarSystem.Scene.Camera.Position.z;
	//solarSystem.Scene.Camera.Target.x								= solarSystem.Scene.Transforms[0].Position.x + 10;


	if(GetAsyncKeyState('Q')) solarSystem.Scene.Camera.Position.y	-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) solarSystem.Scene.Camera.Position.y	+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W') || GetAsyncKeyState(VK_UP		)) modelPlayer.Position.x			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('S') || GetAsyncKeyState(VK_DOWN	)) modelPlayer.Position.x			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('A') || GetAsyncKeyState(VK_LEFT	)) modelPlayer.Position.z			+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('D') || GetAsyncKeyState(VK_RIGHT	)) modelPlayer.Position.z			-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));

	if(solarSystem.Scene.Camera.Position.y > 0.001f)
	if(solarSystem.Scene.Camera.Position.y > 0.001f) if(GetAsyncKeyState(VK_HOME	)) solarSystem.Scene.Camera.Position.RotateZ(ced::MATH_PI * secondsLastFrame);
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

	if(solarSystem.Ships[0].Health)
		modelPlayer.Rotation.y								+= (float)secondsLastFrame * .5f;
	for(uint32_t iEnemy = 1; iEnemy < solarSystem.Scene.Transforms.size(); ++iEnemy)
		solarSystem.Scene.Transforms[iEnemy].Rotation.y						+= (float)secondsLastFrame * (.1f * iEnemy);

	solarSystem.Scene.LightVector									= solarSystem.Scene.LightVector.RotateY(secondsLastFrame * 2);

	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart)
			ship.Parts[iPart].Shots.Update((float)secondsLastFrame);
	}
	solarSystem.Debris		.Update((float)secondsLastFrame);
	solarSystem.Stars		.Update(screenSize.y, (float)secondsLastFrame);
	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion)
		solarSystem.Explosions[iExplosion].Update((float)secondsLastFrame);

	if(playing) {
		::ced::container<::ced::SCoord3<float>>					collisionPoints;
		for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
			::SShip													& ship					= solarSystem.Ships[iShip];
			//if(solarSystem.Health[ship.Entity] <= 0)
			//	continue;
			for(uint32_t iShip2 = 0; iShip2 < solarSystem.Ships.size(); ++iShip2) {
				::SShip													& ship2					= solarSystem.Ships[iShip2];
				if(ship2.Health <= 0 || ship.Team == ship2.Team)
					continue;
				for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
					::SShipPart												& shipPart				= ship.Parts[iPart];
					//if(solarSystem.Health[shipPart.Entity] <= 0)
					//	continue;
					void													* soundAlias			= (iShip2 ? (void*)SND_ALIAS_SYSTEMHAND : (void*)SND_ALIAS_SYSTEMEXCLAMATION);
					for(uint32_t iPart2 = 0; iPart2 < ship2.Parts.size(); ++iPart2) {
						::SShipPart												& shipPart2				= ship2.Parts[iPart2];
						if(shipPart2.Health <= 0)
							continue;
						const ::ced::SMatrix4<float>							matrixTransform			= solarSystem.Scene.ModelMatricesLocal[shipPart2.Entity] * solarSystem.Scene.ModelMatricesLocal[ship2.Entity];
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
