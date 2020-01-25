#include "ced_demo_09.h"
#include <time.h>

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }

int													setupStars			(SStars & stars, ::ced::SCoord2<uint32_t> targetSize)	{
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

int													modelCreate			(::SApplication & app)	{
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
		app.Scene.Models		[indexModel].Rotation.z		= (float)(::ced::MATH_PI_2);
	else
		app.Scene.Models		[indexModel].Rotation.z		= (float)(-::ced::MATH_PI_2);
	app.Scene.Entities		[indexModel]				= {-1};
	const uint32_t												partHealth			= 1000;
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


 	//::ced::geometryBuildCube(app.Geometry);
	//::ced::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	app.Scene.Geometry.resize(4);
	::ced::geometryBuildSphere(app.Scene.Geometry[0],  4U, 2U, 1, {0, 1});
	::ced::geometryBuildSphere(app.Scene.Geometry[1],  4U, 3U, 1, {0, 1});
	::ced::geometryBuildSphere(app.Scene.Geometry[2],  4U, 4U, 1, {0, 1});
	::ced::geometryBuildSphere(app.Scene.Geometry[3],  5U, 5U, 1, {0, 1});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});

	app.Scene.Models[::modelCreate(app)].Position		= {-30};
	app.Scene.Models[::modelCreate(app)].Position		= {+25};
	app.Scene.Models[::modelCreate(app)].Position		= {+30};
	app.Scene.Models[::modelCreate(app)].Position		= {+35};

	app.Scene.Image.resize(4);
	for(uint32_t iImage = 0; iImage < app.Scene.Image.size(); ++iImage) {
		app.Scene.Image[iImage].Metrics								= {32, 32};
		app.Scene.Image[iImage].Pixels.resize(app.Scene.Image[iImage].Metrics.x * app.Scene.Image[iImage].Metrics.y);
		for(uint32_t y = 0; y < app.Scene.Image[iImage].Metrics.y; ++y) {// Generate noise color for planet texture
			//::ced::SColorFloat										baseColor					= {1.0f / 65535.0f * rand(), 1.0f / 65535.0f * rand(), 1.0f / 65535.0f * rand(), 0xFF};;
			bool yAffect = 0 == ((rand() / (1024 * 16)) % (y ? y : 1));
			bool xAffect = 0 == ((rand() / (1024 * 16)) % (y ? y : 1));
			::ced::SColorFloat										baseColor
				= yAffect
				? xAffect
				? ::ced::CYAN	* ::std::max(0.0f, (1.0f / RAND_MAX * rand()))
				: ::ced::RED	* ::std::max(0.0f, (1.0f / RAND_MAX * rand()))
				: ::ced::WHITE	* ::std::max(0.0f, (1.0f / RAND_MAX * rand()))
				;
			bool zAffect = 0 == y % 4;
			for(uint32_t x = 0; x < app.Scene.Image[iImage].Metrics.x; ++x) {
				if(zAffect)
					app.Scene.Image[iImage].Pixels[y * app.Scene.Image[iImage].Metrics.x + x] = yAffect ? ::ced::DARKCYAN : baseColor * ::std::max(.5, sin(x));
				else
					app.Scene.Image[iImage].Pixels[y * app.Scene.Image[iImage].Metrics.x + x] = baseColor;
			}
		}
	}

	app.Scene.Camera.Target				= {};
	app.Scene.Camera.Position			= {-0.000001f, 100, 0};
	app.Scene.Camera.Up					= {0, 1, 0};


	return 0;
}

// Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
// returns t value of intersection and intersection point q
int													intersectRaySphere		(const ::ced::SCoord3<float> & p, const ::ced::SCoord3<float> & d, const ::ced::SCoord3<float> & sphereCenter, double sphereRadius, float &t, ::ced::SCoord3<float> &q) {
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
	::ced::SModelTransform									matricesParent;
	::ced::SModel3D											& modelPlayer		= app.Scene.Models[0];
	for(uint32_t iEnemy = 7; iEnemy < app.Scene.Models.size(); ++iEnemy) {
		const int32_t											indexParent				= app.Scene.Entities[iEnemy].Parent;
		if(0 >= app.Health[iEnemy])
			continue;
		::ced::SModel3D											& modelEnemy		= app.Scene.Models[iEnemy];
		if(-1 == indexParent) {
			modelEnemy.Position.z								= (float)(sin(app.AnimationTime) * 30) * ((iEnemy % 2) ? -1 : 1);
		}
		else {
			matricesParent										= {};
			app.ShotsEnemy.Delay								+= lastFrameSeconds * .3333333;
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);
			::ced::SCoord3<float>									positionGlobal			= (matricesParent.Scale * matricesParent.Rotation * matricesParent.Position).Transform(modelEnemy.Position);
			if(1 < (modelPlayer.Position - positionGlobal).Length()) {
				::ced::SCoord3<float>									direction			= modelPlayer.Position - positionGlobal;
				direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
				app.ShotsEnemy.Spawn(positionGlobal, direction.Normalize(), 20);
			}
		}
	}

	if(GetAsyncKeyState(VK_SPACE)) {
		for(uint32_t iEnemy = 1; iEnemy < 7; ++iEnemy) {
			matricesParent										= {};
			const int32_t											indexParent				= app.Scene.Entities[iEnemy].Parent;
			::ced::SModel3D											& modelEnemy			= app.Scene.Models[iEnemy];
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);
			::ced::SCoord3<float>									positionGlobal			= (matricesParent.Scale * matricesParent.Rotation * matricesParent.Position).Transform(modelEnemy.Position);
			positionGlobal.x									+= 1.5;
			app.ShotsPlayer.Delay								+= lastFrameSeconds * 5;
			::ced::SCoord3<float>									direction			= {1, 0, 0};
			//direction.RotateY(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			//direction.RotateZ(rand() * (1.0 / 65535) * ced::MATH_PI * .0185 * ((rand() % 2) ? -1 : 1));
			app.ShotsPlayer.Spawn(positionGlobal, direction, 200);
		}
	}

	if(GetAsyncKeyState('Q')) app.Scene.Camera.Position.y			-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.Scene.Camera.Position.y			+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) app.Scene.Models[0].Position.x		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('S')) app.Scene.Models[0].Position.x		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('A')) app.Scene.Models[0].Position.z		+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));
	if(GetAsyncKeyState('D')) app.Scene.Models[0].Position.z		-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 2 : 8));

	if(GetAsyncKeyState(VK_NUMPAD8)) app.Scene.Models[0].Rotation.x	-= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD2)) app.Scene.Models[0].Rotation.x	+= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD6)) app.Scene.Models[0].Rotation.z	-= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD4)) app.Scene.Models[0].Rotation.z	+= (float)(lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD5)) app.Scene.Models[0].Rotation	= {0, 0, (float)::ced::MATH_PI_2};

	app.Scene.Models[0].Rotation.y							+= (float)lastFrameSeconds * .5f;
	for(uint32_t iModel = 1; iModel < app.Scene.Models.size(); ++iModel)
		app.Scene.Models[iModel].Rotation.y						+= (float)lastFrameSeconds * 1;

	app.Scene.LightVector										= app.Scene.LightVector.RotateY(lastFrameSeconds * 2);
	if(framework.Window.Resized)
		::setupStars(app.Stars, framework.Window.Size);
	app.ShotsPlayer	.Update((float)lastFrameSeconds);
	app.ShotsEnemy	.Update((float)lastFrameSeconds);
	app.Debris		.Update((float)lastFrameSeconds);
	app.Stars		.Update(framework.Window.Size.y, (float)lastFrameSeconds);

	for(uint32_t iShot = 0; iShot < app.ShotsPlayer.Position.size(); ++iShot) {
		const ::ced::SLine3<float>								shotSegment			= {app.ShotsPlayer.PositionPrev[iShot], app.ShotsPlayer.Position[iShot]};
		for(uint32_t iModel = 7; iModel < app.Scene.Models.size(); ++iModel) {
			const int32_t											indexParent				= app.Scene.Entities[iModel].Parent;
			if(-1 == indexParent)
				continue;
			if(app.Health[iModel] <= 0)
				continue;
			::ced::SCoord3<float>									sphereCenter		= app.Scene.Models[iModel].Position;
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);

			::ced::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
			sphereCenter										= matrixTransformParent.Transform(sphereCenter);

			float													t				= 0;
			::ced::SCoord3<float>									q				= {};

			if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), sphereCenter, 1, t, q)
			 && t < 1
			) {
				PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMHAND, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
				app.Health[iModel]									-= 100;
				app.Health[indexParent]								-= 100;
				for(uint32_t i = 0; i < 10; ++i) {
					::ced::SCoord3<float>			direction			= {0, 1, 0};
					direction.RotateX(rand() * (::ced::MATH_2PI / 65535));
					direction.RotateY(rand() * (::ced::MATH_2PI / 65535));
					direction.RotateZ(rand() * (::ced::MATH_2PI / 65535));
					direction.Normalize();
					app.Debris.Spawn(q, direction, 50);
				}
				app.ShotsPlayer.Remove(iShot);
				--iShot;
				break;
			}
		}
	}

	for(uint32_t iShot = 0; iShot < app.ShotsEnemy.Position.size(); ++iShot) {
		const ::ced::SLine3<float>								shotSegment				= {app.ShotsEnemy.PositionPrev[iShot], app.ShotsEnemy.Position[iShot]};
		for(uint32_t iModel = 0; iModel < 7; ++iModel) {
			const int32_t											indexParent				= app.Scene.Entities[iModel].Parent;
			if(-1 == indexParent)
				continue;
			if(app.Health[iModel] <= 0)
				continue;
			::ced::SCoord3<float>									sphereCenter			= app.Scene.Models[iModel].Position;
			const ::ced::SModel3D									& modelParent			= app.Scene.Models[indexParent];
			matricesParent.Scale	.Scale			(modelParent.Scale, true);
			matricesParent.Rotation	.Rotation		(modelParent.Rotation);
			matricesParent.Position	.SetTranslation	(modelParent.Position, true);

			::ced::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
			sphereCenter										= matrixTransformParent.Transform(sphereCenter);

			float													t						= 0;
			::ced::SCoord3<float>									q						= {};

			if( intersectRaySphere(shotSegment.A, (shotSegment.B - shotSegment.A).Normalize(), sphereCenter, 1, t, q)
			 && t < 1
			) {
				PlaySoundA((LPCSTR)SND_ALIAS_SYSTEMEXCLAMATION, GetModuleHandle(0), SND_ALIAS_ID | SND_ASYNC);
				app.Health[iModel]									-= 100;
				app.Health[indexParent]								-= 100;
				for(uint32_t i = 0; i < 10; ++i) {
					::ced::SCoord3<float>									direction			= {0, 1, 0};
					direction.RotateX(rand() * (::ced::MATH_2PI / 65535));
					direction.RotateY(rand() * (::ced::MATH_2PI / 65535));
					direction.RotateZ(rand() * (::ced::MATH_2PI / 65535));
					direction.Normalize();
					app.Debris.Spawn(q, direction, 50);
				}
				app.ShotsEnemy.Remove(iShot);
				--iShot;
				break;
			}
		}
	}

	::draw(app);
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
