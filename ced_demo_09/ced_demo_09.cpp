#include "ced_demo_09.h"
#include <time.h>

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }

static	int											setupStars			(SStars & stars, ::ced::SCoord2<uint32_t> targetSize)	{
	if(0 == targetSize.y) return 0;
	if(0 == targetSize.x) return 0;
	stars.Speed		.resize(128);
	stars.Brightness.resize(128);
	stars.Position	.resize(128);
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		stars.Speed			[iStar]						= float(16 + (rand() % 64));
		stars.Brightness	[iStar]						= float(1.0 / RAND_MAX * rand());
		stars.Position		[iStar].y					= float(rand() % targetSize.y);
		stars.Position		[iStar].x					= float(rand() % targetSize.x);
	}
	return 0;
}

static	int											modelCreate			(::SApplication & app, uint32_t partHealth)	{
	int32_t													indexModel			= app.Scene.ModelMatricesLocal.size();
	uint32_t												countParts			= 6;
	app.Health						.resize(indexModel + 1 + countParts);
	app.Scene.ModelMatricesLocal	.resize(indexModel + 1 + countParts);
	app.Scene.ModelMatricesGlobal	.resize(indexModel + 1 + countParts);
	app.Scene.Models				.resize(indexModel + 1 + countParts);
	app.Scene.Entities				.resize(indexModel + 1 + countParts);

	app.Scene.Models		[indexModel]				= {};
	app.Scene.Models		[indexModel].Scale			= {1, 1, 1};
	if(0 == indexModel)
		app.Scene.Models		[indexModel].Rotation.z		= (float)(-::ced::MATH_PI_2);
	else
		app.Scene.Models		[indexModel].Rotation.z		= (float)(::ced::MATH_PI_2);
	app.Scene.Entities		[indexModel]				= {-1};
	app.Health[indexModel]									= partHealth * countParts;
	for(uint32_t iModel = indexModel + 1; iModel < app.Scene.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Scene.Models[iModel];
		model.Scale											= {1, 1, 1};
		model.Rotation										= {0, 0, 0};
		model.Position										= {2, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / countParts * iModel);
		::ced::SEntity											& entity		= app.Scene.Entities[iModel];
		entity.Parent										= indexModel;
		app.Health[iModel]									= partHealth;
 		app.Scene.Entities[indexModel].Children.push_back(iModel);
	}
	return indexModel;
}

int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);
	::setupStars(app.Stars, framework.Window.Size);

	srand((uint32_t)time(0));

	app.Scene.Geometry.resize(5);
	//::ced::geometryBuildGrid(app.Scene.Geometry[0], {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Scene.Geometry[0],  8U, 7U, 1, {0, 1});
	//::ced::geometryBuildCube(app.Scene.Geometry[0]);

	//::ced::geometryBuildSphere(app.Scene.Geometry[1],  4U, 3U, 1, {0, 1});
	::ced::geometryBuildCube(app.Scene.Geometry[1]);
	::ced::geometryBuildCube(app.Scene.Geometry[2]);
	//::ced::geometryBuildCube(app.Scene.Geometry[3]);
	::ced::geometryBuildCube(app.Scene.Geometry[4]);
	::ced::geometryBuildGrid(app.Scene.Geometry[1], {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Scene.Geometry[2], 3U, 2U, 1, {0, 0});
	::ced::geometryBuildSphere(app.Scene.Geometry[3], 4U, 2U, 1, {0, 0});
	::ced::geometryBuildSphere(app.Scene.Geometry[4], 16U, 2U, 1, {0, 0});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});

	const uint32_t												partHealthPlayer		= 200;
	const uint32_t												partHealthEnemy			= 1000;
	app.Scene.Models[::modelCreate(app, partHealthPlayer)].Position		= {-30};
	app.Scene.Models[::modelCreate(app, partHealthEnemy )].Position		= {+20};
	app.Scene.Models[::modelCreate(app, partHealthEnemy )].Position		= {+25};
	//app.Scene.Models[::modelCreate(app, partHealthEnemy )].Position		= {+30};
	//app.Scene.Models[::modelCreate(app, partHealthEnemy )].Position		= {+35};

	::ced::SColorFloat										baseColor	[4]	=
		{ ::ced::LIGHTGREEN
		, ::ced::LIGHTBLUE
		, ::ced::LIGHTRED
		, ::ced::LIGHTCYAN
		};

	app.Scene.Image.resize(5);
	for(uint32_t iImage = 0; iImage < app.Scene.Image.size(); ++iImage) {
		app.Scene.Image[iImage].Metrics								= {24, 6};
		app.Scene.Image[iImage].Pixels.resize(app.Scene.Image[iImage].Metrics.x * app.Scene.Image[iImage].Metrics.y);
		for(uint32_t y = 0; y < app.Scene.Image[iImage].Metrics.y; ++y) {// Generate noise color for planet texture
			//bool yAffect = rand() % 3;
			bool xAffect = (y % 2);
			::ced::SColorFloat										lineColor					= baseColor[rand() % ::std::size(baseColor)];
			//bool zAffect = 0 == y % 2;
			for(uint32_t x = 0; x < app.Scene.Image[iImage].Metrics.x; ++x) {
				app.Scene.Image[iImage].Pixels[y * app.Scene.Image[iImage].Metrics.x + x] = lineColor * (xAffect ? ::std::max(.5, sin(x)) : 1);
			}
		}
		//::ced::bmpFileLoad("../ced_data/cp437_12x12.bmp", app.Scene.Image[iImage]);
	}

	app.Scene.Camera.Target				= {};
	app.Scene.Camera.Position			= {-0.000001f, 100, 0};
	app.Scene.Camera.Up					= {0, 1, 0};


	return 0;
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
static	int											intersectRaySphere		(const ::ced::SCoord3<float> & p, const ::ced::SCoord3<float> & d, const ::ced::SCoord3<float> & sphereCenter, double sphereRadius, float &t, ::ced::SCoord3<float> &q) {
	const ::ced::SCoord3<float>								m						= p - sphereCenter;
	double													b						= m.Dot(d);
	double													c						= m.Dot(m) - sphereRadius * sphereRadius;

	if (c > 0.0f && b > 0.0f)	// Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
		return 0;
	double													discr					= b * b - c;

	if (discr < 0.0f)	// A negative discriminant corresponds to ray missing sphere
		return 0;

	t					= (float)(-b - sqrt(discr));	// Ray now found to intersect sphere, compute smallest t value of intersection
	if (t < 0.0f)	// If t is negative, ray started inside sphere so clamp t to zero
		t					= 0.0f;

	q					= p + d * t;
	return 1;
}

static	int											handleShotCollision
	( ::ced::SGeometryQuads				& meshShip
	, int32_t							indexShip
	, int32_t							indexEntityPart
	, const ::ced::SCoord3<float>		& collisionPoint
	, int32_t							& healthParth
	, int32_t							& healthParent
	, ::SDebris							& debris
	, ::ced::container<::SExplosion>	& explosions
	, void * soundAlias
	) {
	PlaySoundA((LPCSTR)soundAlias, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
	healthParth											-= 100;
	healthParent										-= 100;
	float													debrisSpeed					= 50;
	float													debrisBright				= 1;
	uint32_t												debrisCount					= 10;
	bool													exploded					= false;
	if(0 >= healthParth) {
		exploded											= true	;
		if(0 >= healthParent) {
			debrisSpeed											= 10	;
			debrisCount											= 100	;
			debrisBright										= 3		;
		}
		else {
			debrisSpeed											= 60	;
			debrisCount											= 150	;
			debrisBright										= 2		;
		}
	}
	debris.SpawnSpherical(debrisCount, collisionPoint, debrisSpeed, debrisBright);

	if(exploded) {
		::SExplosion											newExplosion				= {};
		newExplosion.IndexMesh								= indexShip;
		newExplosion.IndexEntity							= indexEntityPart;
		for(uint32_t iQuad = 0, countQuads = meshShip.Triangles.size() / 4; iQuad < countQuads; ++iQuad) {
			newExplosion.Slices.push_back({(uint16_t)(iQuad), 2});
			::ced::SCoord3<float>									direction					= {0, 1, 0};
			direction.RotateX(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.RotateY(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.RotateZ(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.Normalize();
			newExplosion.Particles.Spawn(collisionPoint, direction, debrisSpeed);
		}
		bool													add							= true;
		for(uint32_t iExplosion = 0; iExplosion < explosions.size(); ++iExplosion) {
			SExplosion												& explosion					= explosions[iExplosion];
			if(0 == explosion.Slices.size()) {
				explosion										= newExplosion;
				add												= false;
				break;
			}
		}
		if(add)
			explosions.push_back(newExplosion);
	}
	return exploded ? 1 : 0;
}

static	int											collisionDetect		(::SShots & shots, const ::ced::SCoord3<float> & modelPosition, ::ced::container<::ced::SCoord3<float>> & collisionPoints)	{
	bool													detected			= false;
	collisionPoints.clear();
	for(uint32_t iShot = 0; iShot < shots.Particles.Position.size(); ++iShot) {
		const ::ced::SLine3<float>								shotSegment			= {shots.PositionPrev[iShot], shots.Particles.Position[iShot]};
		float													t				= 0;
		::ced::SCoord3<float>									collisionPoint	= {};
		if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), modelPosition, 1.2, t, collisionPoint)
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

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	if(1 == ::ced::frameworkUpdate(app.Framework))
		framework.Running = false;
	//------------------------------------------- Handle input
	double													speed				= 10;
	double													lastFrameSeconds	= framework.Timer.ElapsedMicroseconds * .000001;
	lastFrameSeconds									= ::std::min(lastFrameSeconds, 0.200);

	app.AnimationTime									+= lastFrameSeconds;
	//app.ShotsPlayer.Delay								+= lastFrameSeconds * 20;
	::ced::SModelTransform									matrices;
	app.Scene.ModelMatricesLocal.resize(app.Scene.Models.size());
	for(uint32_t iModel = 0; iModel < app.Scene.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Scene.Models[iModel];
		matrices.Scale		.Scale			(model.Scale	, true);
		matrices.Rotation	.Rotation		(model.Rotation);
		matrices.Position	.SetTranslation	(model.Position, true);
		app.Scene.ModelMatricesLocal[iModel]				= matrices.Scale * matrices.Rotation * matrices.Position;
	}

	::ced::SModelTransform									matricesParent;
	::ced::SModel3D											& modelPlayer		= app.Scene.Models[0];
	for(uint32_t iEnemy = 7; iEnemy < app.Scene.Models.size(); ++iEnemy) {
		const int32_t											indexParent				= app.Scene.Entities[iEnemy].Parent;
		if(0 >= app.Health[iEnemy])
			continue;
		::ced::SModel3D											& modelEnemy		= app.Scene.Models[iEnemy];
		if(-1 == indexParent) {
			modelEnemy.Position.z								= (float)(sin(app.AnimationTime) * iEnemy * 3) * ((iEnemy % 2) ? -1 : 1);
		}
		else {
			app.Scene.Models[iEnemy].Rotation.y					+= (float)lastFrameSeconds * 1;
			matricesParent										= {};
			app.ShotsEnemy.Delay								+= lastFrameSeconds * .5;
			::ced::SCoord3<float>									positionGlobal			= app.Scene.ModelMatricesLocal[indexParent].Transform(modelEnemy.Position);
			if(1 < (modelPlayer.Position - positionGlobal).LengthSquared()) {
				::ced::SCoord3<float>									direction			= modelPlayer.Position - positionGlobal;
				direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
				app.ShotsEnemy.Spawn(positionGlobal, direction.Normalize(), 25, 1);
			}
		}
	}

	if(GetAsyncKeyState(VK_SPACE)) {
		for(uint32_t iEntity = 1; iEntity < 7; ++iEntity) {
			if(0 >= app.Health[iEntity])
				continue;
			matricesParent										= {};
			const int32_t											indexParent				= app.Scene.Entities[iEntity].Parent;
			::ced::SModel3D											& modelEnemy			= app.Scene.Models[iEntity];
			::ced::SCoord3<float>									positionGlobal			= app.Scene.ModelMatricesLocal[indexParent].Transform(modelEnemy.Position);
			//positionGlobal.x									+= 1.5;
			app.ShotsPlayer.Delay								+= lastFrameSeconds * 10;
			::ced::SCoord3<float>									direction			= {1, 0, 0};
			direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			direction.RotateZ(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			app.ShotsPlayer.Spawn(positionGlobal, direction, 200, .2f);
		}
	}

	if(GetAsyncKeyState('Q')) app.Scene.Camera.Position.y	-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.Scene.Camera.Position.y	+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) modelPlayer.Position.x		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('S')) modelPlayer.Position.x		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('A')) modelPlayer.Position.z		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('D')) modelPlayer.Position.z		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));

	if(GetAsyncKeyState(VK_NUMPAD5))
		modelPlayer.Rotation								= {0, 0, (float)-::ced::MATH_PI_2};
	else {
		if(GetAsyncKeyState(VK_NUMPAD8)) modelPlayer.Rotation.z	-= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD2)) modelPlayer.Rotation.z	+= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD6)) modelPlayer.Rotation.x -= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
		if(GetAsyncKeyState(VK_NUMPAD4)) modelPlayer.Rotation.x += (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	}

	if(app.Health[0])
		modelPlayer.Rotation.y								+= (float)lastFrameSeconds * .5f;
	for(uint32_t iEnemy = 1; iEnemy < app.Scene.Models.size(); ++iEnemy) {
		if(0 == app.Health[iEnemy])
			continue;
		app.Scene.Models[iEnemy].Rotation.y						+= (float)lastFrameSeconds * (.1f * iEnemy);
	}

	app.Scene.LightVector									= app.Scene.LightVector.RotateY(lastFrameSeconds * 2);
	if(framework.Window.Resized)
		::setupStars(app.Stars, framework.Window.Size);

	app.ShotsPlayer	.Update((float)lastFrameSeconds);
	app.ShotsEnemy	.Update((float)lastFrameSeconds);
	app.Debris		.Update((float)lastFrameSeconds);
	app.Stars		.Update(framework.Window.Size.y, (float)lastFrameSeconds);
	for(uint32_t iExplosion = 0; iExplosion < app.Explosions.size(); ++iExplosion)
		app.Explosions[iExplosion].Update((float)lastFrameSeconds);

	::ced::container<::ced::SCoord3<float>>					collisionPoints;
	for(uint32_t iModel = 0; iModel < app.Scene.Models.size(); ++iModel) {
		const int32_t											indexParent				= app.Scene.Entities[iModel].Parent;
		if(-1 == indexParent)
			continue;
		if(app.Health[iModel] <= 0)
			continue;
		::ced::SMatrix4<float>									matrixTransform			= app.Scene.ModelMatricesLocal[iModel] * app.Scene.ModelMatricesLocal[indexParent];
		if(iModel < 7) {
			::collisionDetect(app.ShotsEnemy, matrixTransform.GetTranslation(), collisionPoints);
			for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
				::handleShotCollision(app.Scene.Geometry[indexParent / 7], indexParent / 7, iModel, collisionPoints[iCollisionPoint], app.Health[iModel], app.Health[indexParent], app.Debris, app.Explosions, (void*)SND_ALIAS_SYSTEMEXCLAMATION);
		}
		else {
			::collisionDetect(app.ShotsPlayer, matrixTransform.GetTranslation(), collisionPoints);
			for(uint32_t iCollisionPoint = 0; iCollisionPoint < collisionPoints.size(); ++iCollisionPoint)
				::handleShotCollision(app.Scene.Geometry[indexParent / 7], indexParent / 7, iModel, collisionPoints[iCollisionPoint], app.Health[iModel], app.Health[indexParent], app.Debris, app.Explosions, (void*)SND_ALIAS_SYSTEMHAND);
		}
	}

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
