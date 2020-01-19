#include "ced_demo_08.h"


int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }

int													setupStars			(SStars & stars, ::ced::SCoord2<uint32_t> targetSize)	{
	if(0 == targetSize.y) return 0;
	if(0 == targetSize.x) return 0;
	stars.Speed		.resize(128);
	stars.Brightness.resize(128);
	stars.Position	.resize(128);
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		stars.Speed			[iStar]						= float(16 + (rand() % 64));
		stars.Brightness	[iStar]						= float(1.0 / 65535 * rand());
		stars.Position		[iStar].y					= float(rand() % targetSize.y);
		stars.Position		[iStar].x					= float(rand() % targetSize.x);
	}
	return 0;
}
int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);
	::setupStars(app.Stars, framework.Window.Size);

	//::ced::geometryBuildCube(app.Geometry);
	//::ced::geometryBuildGrid(app.Geometry, {2U, 2U}, {1U, 1U});
	::ced::geometryBuildSphere(app.Geometry, 4U, 2U, 1, {0, 1});
	//::ced::geometryBuildFigure0(app.Geometry, 10U, 10U, 1, {});
	app.ModelMatricesLocal	.resize(7);
	app.ModelMatricesGlobal	.resize(7);
	app.Models				.resize(7);
	app.Entities			.resize(7);
	app.Models		[0]									= {};
	app.Models		[0].Scale							= {1, 1, 1};
	app.Models		[0].Rotation.z						= (float)(::ced::MATH_PI_2);
	app.Entities	[0]									= {-1};
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel) {
		::ced::SModel3D											& model			= app.Models[iModel];
		model.Scale											= {1, 1, 1};
		//model.Rotation										= {0, 1, 0};
		model.Position										= {2, 0.5};
		model.Position.RotateY(::ced::MATH_2PI / (app.Models.size() - 1)* iModel);
		::ced::SEntity											& entity		= app.Entities[iModel];
		entity.Parent										= 0;
 		app.Entities[0].Children.push_back(iModel);
	}

	app.Image.Metrics										= {256, 256};
	app.Image.Pixels.resize(app.Image.Metrics.x * app.Image.Metrics.y);
	for(uint32_t y = 0; y < app.Image.Metrics.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < app.Image.Metrics.x; ++x) {
		app.Image.Pixels[y * app.Image.Metrics.x + x] = {(uint8_t)rand(), (uint8_t)rand(), (uint8_t)rand(), 0xFF};
	}

	return 0;
}

int													updateShots			(::SShots & shots, float lastFrameSeconds)	{
	for(uint32_t iShot = 0; iShot < shots.Position.size(); ++iShot) {
		shots.PositionPrev	[iShot]						= shots.Position	[iShot];
		shots.Position		[iShot].x					+= float(shots.Speed[iShot] * lastFrameSeconds);
		if(shots.Position[iShot].x > 50) {
			shots.PositionPrev	[iShot]						= shots.PositionPrev	[shots.Position.size() - 1];
			shots.Position		[iShot]						= shots.Position		[shots.Position.size() - 1];
			shots.Speed			[iShot]						= shots.Speed			[shots.Position.size() - 1];
			shots.Brightness	[iShot]						= shots.Brightness		[shots.Position.size() - 1];
			shots.PositionPrev	.resize(shots.PositionPrev	.size() - 1);
			shots.Position		.resize(shots.Position		.size() - 1);
			shots.Speed			.resize(shots.Speed			.size() - 1);
			shots.Brightness	.resize(shots.Brightness	.size() - 1);
		}
	}
	return 0;
}

int													updateStars			(::SStars & stars, uint32_t yMax, float lastFrameSeconds)	{
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::ced::SCoord2<float>									 & starPos			= stars.Position[iStar];
		starPos.y											+= stars.Speed[iStar] * lastFrameSeconds;
		stars.Brightness[iStar]								= float(1.0 / 65535 * rand());
		if(starPos.y >= yMax) {
			stars.Speed		[iStar]								= float(16 + (rand() % 64));
			starPos.y											= 0;
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
	app.Shots.Delay										+= lastFrameSeconds;
	if(GetAsyncKeyState(VK_SPACE)) {
		if(app.Shots.Delay >= 0.1) {
			app.Shots.PositionPrev	.push_back(app.Models[0].Position);
			app.Shots.Position		.push_back(app.Models[0].Position);
			app.Shots.Speed			.push_back(200);
			app.Shots.Brightness	.push_back(1);
			app.Shots.Delay									= 0;
		}
	}
	if(GetAsyncKeyState('Q')) app.Camera.Position.y				-= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);
	if(GetAsyncKeyState('E')) app.Camera.Position.y				+= (float)lastFrameSeconds * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2);

	if(GetAsyncKeyState('W')) app.Models[0].Position.x			+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('S')) app.Models[0].Position.x			-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('A')) app.Models[0].Position.z			+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState('D')) app.Models[0].Position.z			-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	if(GetAsyncKeyState(VK_NUMPAD8)) app.Models[0].Rotation.x	-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD2)) app.Models[0].Rotation.x	+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD6)) app.Models[0].Rotation.z	-= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));
	if(GetAsyncKeyState(VK_NUMPAD4)) app.Models[0].Rotation.z	+= (float)(lastFrameSeconds * speed * (GetAsyncKeyState(VK_SHIFT) ? 8 : 2));

	app.Models[0].Rotation.y							+= (float)lastFrameSeconds;
	for(uint32_t iModel = 1; iModel < app.Models.size(); ++iModel)
		app.Models[iModel].Rotation.y						+= (float)lastFrameSeconds * 5;

	app.LightVector										= app.LightVector.RotateY(lastFrameSeconds * 2);
	if(framework.Window.Resized)
		::setupStars(app.Stars, framework.Window.Size);
	::updateStars(app.Stars, framework.Window.Size.y, (float)lastFrameSeconds);
	::updateShots(app.Shots, (float)lastFrameSeconds);

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
