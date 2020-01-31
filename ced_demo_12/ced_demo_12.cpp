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

static	int											shipCreate			(::SSolarSystem & solarSystem, uint32_t partHealth, int32_t iGeometry, int32_t iImage)	{
	::SShipScene											& scene				= solarSystem.Scene;
	int32_t													indexEntity			= scene.ModelMatricesLocal.size();
	uint32_t												countParts			= 6;
	solarSystem.Health			.resize(indexEntity + 1 + countParts);
	scene.ModelMatricesLocal	.resize(indexEntity + 1 + countParts);
	scene.ModelMatricesGlobal	.resize(indexEntity + 1 + countParts);
	scene.Transforms			.resize(indexEntity + 1 + countParts);
	solarSystem.Entities		.resize(indexEntity + 1 + countParts);

	scene.Transforms	[indexEntity]					= {};
	scene.Transforms	[indexEntity].Scale				= {1, 1, 1};
	if(0 == indexEntity)
		scene.Transforms	[indexEntity].Rotation.z			= (float)(-::ced::MATH_PI_2);
	else
		scene.Transforms	[indexEntity].Rotation.z			= (float)(::ced::MATH_PI_2);
	solarSystem.Entities[indexEntity]					= {-1};
	solarSystem.Entities[indexEntity].Geometry			= iGeometry;
	solarSystem.Entities[indexEntity].Transform			= indexEntity;
	solarSystem.Entities[indexEntity].Image				= iImage;
	solarSystem.Entities[indexEntity].Body				= -1;
	solarSystem.Health	[indexEntity]					= partHealth * countParts;
	for(uint32_t iModel = indexEntity + 1; iModel < scene.Transforms.size(); ++iModel) {
		::ced::SModel3											& model			= scene.Transforms[iModel];
		model.Scale											= {1, 1, 1};
		model.Rotation										= {0, 0, 0};
		model.Position										= {2, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / countParts * iModel);
		::SEntity												& entity		= solarSystem.Entities[iModel];
		entity.Parent										= indexEntity;
		entity.Geometry										= iGeometry;
		entity.Transform									= iModel;
		entity.Image										= iImage;
		entity.Body											= -1;
		solarSystem.Health[iModel]									= partHealth;
 		solarSystem.Entities[indexEntity].Children.push_back(iModel);
	}
	return indexEntity;
}

int													solarSystemSetup	(::SSolarSystem & solarSystem, ::ced::SCoord2<uint32_t> windowSize)	{
	::setupStars(solarSystem.Stars, windowSize);
	::SShipScene											& scene				= solarSystem.Scene;


	scene.Geometry.resize(5);
	//::ced::geometryBuildCube	(scene.Geometry[0]);
	::ced::geometryBuildSphere	(scene.Geometry[0], 8U, 5U, 1, {0, 0});
	//::ced::geometryBuildFigure0	(scene.Geometry[0], 2U, 8U, 1, {});
	::ced::geometryBuildSphere	(scene.Geometry[1], 16U, 5U, .5f, {0, 0});
	::ced::geometryBuildFigure0	(scene.Geometry[1], 2U, 8U, 1, {});
	::ced::geometryBuildCube	(scene.Geometry[2]);
	::ced::geometryBuildGrid	(scene.Geometry[2], {2U, 2U}, {1U, 1U});
	::ced::geometryBuildCube	(scene.Geometry[3]);
	::ced::geometryBuildSphere	(scene.Geometry[3], 4U, 2U, 1, {0, 0});
	::ced::geometryBuildSphere	(scene.Geometry[4], 16U, 2U, 1, {0, 0});

	const uint32_t											partHealthPlayer		= 200;
	const uint32_t											partHealthEnemy			= 2000;
	scene.Transforms[solarSystem.Entities[::shipCreate(solarSystem, partHealthPlayer, 0, 0)].Transform].Position	= {-30};
	scene.Transforms[solarSystem.Entities[::shipCreate(solarSystem, partHealthEnemy , 1, 1)].Transform].Position	= {+20};
	scene.Transforms[solarSystem.Entities[::shipCreate(solarSystem, partHealthEnemy , 2, 2)].Transform].Position	= {+25};
	scene.Transforms[solarSystem.Entities[::shipCreate(solarSystem, partHealthEnemy , 3, 3)].Transform].Position	= {+30};
	scene.Transforms[solarSystem.Entities[::shipCreate(solarSystem, partHealthEnemy , 4, 4)].Transform].Position	= {+35};

	::ced::SColorFloat										baseColor	[4]			=
		{ ::ced::LIGHTGREEN
		, ::ced::LIGHTBLUE
		, ::ced::LIGHTRED
		, ::ced::LIGHTCYAN
		};

	scene.Image.resize(5);
	//::ced::bmpFileLoad("../ced_data/mars_color.bmp", solarSystem.Scene.Image[0], true);
	for(uint32_t iImage = 0; iImage < scene.Image.size(); ++iImage) {
		if(scene.Image[iImage].Pixels.size())
			continue;
		scene.Image[iImage].Metrics						= {24, 6};
		scene.Image[iImage].Pixels.resize(scene.Image[iImage].Metrics.x * scene.Image[iImage].Metrics.y);
		for(uint32_t y = 0; y < scene.Image[iImage].Metrics.y; ++y) {// Generate noise color for planet texture
			bool													xAffect						= (y % 2);
			::ced::SColorFloat										lineColor					= baseColor[rand() % ::std::size(baseColor)];
			for(uint32_t x = 0; x < scene.Image[iImage].Metrics.x; ++x) {
				scene.Image[iImage].Pixels[y * scene.Image[iImage].Metrics.x + x] = lineColor * (xAffect ? ::std::max(.5, sin(x)) : 1);
			}
		}
	}
	scene.Camera.Target								= {};
	scene.Camera.Position							= {-0.000001f, 100, 0};
	scene.Camera.Up									= {0, 1, 0};
	return 0;
}

int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);
	srand((uint32_t)time(0));
	::solarSystemSetup(app.SolarSystem, framework.Window.Size);
	return 0;
}

static	int											explosionSlicesSetup		(::ced::container<::SExplosion> & explosions, int32_t indexMesh, uint32_t triangleCount, const ::ced::SCoord3<float> &collisionPoint, double debrisSpeed) {
	::SExplosion											newExplosion				= {};
	newExplosion.IndexMesh								= indexMesh;
	for(uint32_t iQuad = 0, countQuads = triangleCount / 6; iQuad < countQuads; ++iQuad) {
		newExplosion.Slices.push_back({(uint16_t)iQuad, (uint16_t)(rand() % 4 + 3)});
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
			explosion										= newExplosion;
			return iExplosion;
		}
	}
	return explosions.push_back(newExplosion);
}

static	int											applyDamage
	( int32_t							& healthPart
	, int32_t							& healthParent
	) {
	healthPart											-= 100;
	healthParent										-= 100;
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

int													handleCollisionPoint	(SSolarSystem & solarSystem, int32_t iEntity, int32_t indexParent, const ::ced::SCoord3<float> & collisionPoint, void* soundAlias)	{
	PlaySoundA((LPCSTR)soundAlias, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	solarSystem.Debris.SpawnSpherical(5, collisionPoint, 50, 1);
	if(::applyDamage(solarSystem.Health[iEntity], solarSystem.Health[indexParent])) {	// returns true if health reaches zero
		const int32_t										indexMesh			= indexParent / 7;
		const uint32_t										countTriangles		= solarSystem.Scene.Geometry[indexMesh].Triangles.size();
		::explosionSlicesSetup(solarSystem.Explosions, indexMesh, countTriangles, collisionPoint, 50);
		solarSystem.Debris.SpawnSpherical(40, collisionPoint, 60, 2);
		if(0 >= solarSystem.Health[indexParent]) {
			const ::ced::SCoord3<float>							& parentPosition	= solarSystem.Scene.Transforms[indexParent].Position;
			::explosionSlicesSetup(solarSystem.Explosions, indexMesh, countTriangles, parentPosition, 10);
			solarSystem.Debris.SpawnSpherical(150, parentPosition, 10, 3);
		}
		return 1;
	}
	return 0;
}
int													solarSystemUpdate				(SSolarSystem & solarSystem, double secondsLastFrame, ::ced::SCoord2<uint32_t> screenSize)	{
	//------------------------------------------- Handle input
	double													speed				= 10;
	secondsLastFrame									= ::std::min(secondsLastFrame, 0.200);
	secondsLastFrame									*= solarSystem.TimeScale;

	solarSystem.AnimationTime									+= secondsLastFrame;
	//solarSystem.ShotsPlayer.Delay								+= secondsLastFrame * 20;
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
	for(uint32_t iEnemy = 7; iEnemy < solarSystem.Entities.size(); ++iEnemy) {
		const int32_t											indexParent				= solarSystem.Entities[iEnemy].Parent;
		if(0 >= solarSystem.Health[iEnemy])
			continue;
		::ced::SModel3											& modelEnemy			= solarSystem.Scene.Transforms[iEnemy];
		if(-1 == indexParent) {
			modelEnemy.Position.z								= (float)(sin(solarSystem.AnimationTime) * iEnemy * 3) * ((iEnemy % 2) ? -1 : 1);
		}
		else {
			solarSystem.Scene.Transforms[iEnemy].Rotation.y		+= (float)secondsLastFrame * 1;
			matricesParent										= {};
			solarSystem.ShotsEnemy.Delay						+= secondsLastFrame * .5;
			::ced::SCoord3<float>									positionGlobal			= solarSystem.Scene.ModelMatricesLocal[indexParent].Transform(modelEnemy.Position);
			if(1 < (modelPlayer.Position - positionGlobal).LengthSquared()) {
				::ced::SCoord3<float>									direction			= modelPlayer.Position - positionGlobal;
				direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
				solarSystem.ShotsEnemy.Spawn(positionGlobal, direction.Normalize(), 25, 1);
			}
		}
	}
	if(GetAsyncKeyState(VK_SPACE)) {
		for(uint32_t iEntity = 1; iEntity < 7; ++iEntity) {
			if(0 >= solarSystem.Health[iEntity])
				continue;
			matricesParent										= {};
			const int32_t											indexParent				= solarSystem.Entities[iEntity].Parent;
			::ced::SModel3											& modelEnemy			= solarSystem.Scene.Transforms[iEntity];
			::ced::SCoord3<float>									positionGlobal			= solarSystem.Scene.ModelMatricesLocal[indexParent].Transform(modelEnemy.Position);
			//positionGlobal.x									+= 1.5;
			solarSystem.ShotsPlayer.Delay								+= secondsLastFrame * 10;
			::ced::SCoord3<float>									direction				= {1, 0, 0};
			direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			direction.RotateZ(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			solarSystem.ShotsPlayer.Spawn(positionGlobal, direction, 100, .2f);
		}
	}

	if(GetAsyncKeyState('Q')) solarSystem.Scene.Camera.Position.y	-= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) solarSystem.Scene.Camera.Position.y	+= (float)secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) modelPlayer.Position.x		+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('S')) modelPlayer.Position.x		-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('A')) modelPlayer.Position.z		+= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('D')) modelPlayer.Position.z		-= (float)(secondsLastFrame * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));

	if(GetAsyncKeyState(VK_NUMPAD5))
		modelPlayer.Rotation									= {0, 0, (float)-::ced::MATH_PI_2};
	else {
		if(GetAsyncKeyState(VK_NUMPAD8)) modelPlayer.Rotation.z		-= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD2)) modelPlayer.Rotation.z		+= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD6)) modelPlayer.Rotation.x		-= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD4)) modelPlayer.Rotation.x		+= (float)(secondsLastFrame * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	}

	if(solarSystem.Health[0])
		modelPlayer.Rotation.y								+= (float)secondsLastFrame * .5f;
	for(uint32_t iEnemy = 1; iEnemy < solarSystem.Scene.Transforms.size(); ++iEnemy) {
		if(0 == solarSystem.Health[iEnemy])
			continue;
		solarSystem.Scene.Transforms[iEnemy].Rotation.y						+= (float)secondsLastFrame * (.1f * iEnemy);
	}

	solarSystem.Scene.LightVector									= solarSystem.Scene.LightVector.RotateY(secondsLastFrame * 2);

	solarSystem.ShotsPlayer	.Update((float)secondsLastFrame);
	solarSystem.ShotsEnemy	.Update((float)secondsLastFrame);
	solarSystem.Debris		.Update((float)secondsLastFrame);
	solarSystem.Stars		.Update(screenSize.y, (float)secondsLastFrame);
	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion)
		solarSystem.Explosions[iExplosion].Update((float)secondsLastFrame);

	::ced::container<::ced::SCoord3<float>>					collisionPoints;
	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		const int32_t											iEntityParent				= solarSystem.Entities[iEntity].Parent;
		if(-1 == iEntityParent)
			continue;
		if(solarSystem.Health[iEntity] <= 0)
			continue;
		::ced::SMatrix4<float>									matrixTransform			= solarSystem.Scene.ModelMatricesLocal[iEntity] * solarSystem.Scene.ModelMatricesLocal[iEntityParent];
		void													* soundAlias;
		if(iEntity < 7) {
			::collisionDetect(solarSystem.ShotsEnemy, matrixTransform.GetTranslation(), collisionPoints);
			soundAlias										= (void*)SND_ALIAS_SYSTEMEXCLAMATION;
		}
		else {
			::collisionDetect(solarSystem.ShotsPlayer, matrixTransform.GetTranslation(), collisionPoints);
			soundAlias										= (void*)SND_ALIAS_SYSTEMHAND;
		}
		for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
			if(::handleCollisionPoint(solarSystem, iEntity, iEntityParent, collisionPoints[iCollisionPoint], soundAlias))	// returns true if part health reaches zero.
				break;
	}
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running = false;
	if(framework.Window.Resized)
		::setupStars(app.SolarSystem.Stars, framework.Window.Size);

	::solarSystemUpdate(app.SolarSystem, framework.Timer.ElapsedMicroseconds * .000001, framework.Window.Size);
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
