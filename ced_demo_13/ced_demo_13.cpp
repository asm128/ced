#include "ced_demo_13.h"
#include <time.h>
#include <process.h>

int													cleanup				(SApplication & app)	{ return ::ced::frameworkCleanup(app.Framework); }
int													setup				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	::ced::frameworkSetup(framework);
	srand((uint32_t)time(0));
	::ssg::solarSystemSetup(app.SolarSystem, framework.Window.Size);
	framework.UseDoubleBuffer							= true;
	return 0;
}

int													update				(SApplication & app)	{
	::ced::SFramework										& framework			= app.Framework;
	{
		::std::lock_guard<::std::mutex>							lockUpdate			(app.SolarSystem.LockUpdate);
		if(1 == ::ced::frameworkUpdate(app.Framework))
			framework.Running = false;
	}
	if(framework.Window.Resized) {
		::ced::SMatrix4<float>									& matrixProjection	= app.SolarSystem.Scene.MatrixProjection;
		matrixProjection.FieldOfView(::ced::MATH_PI * .25, framework.Window.Size.x / (double)framework.Window.Size.y, 0.01, 500);
		::ced::SMatrix4<float>									matrixViewport		= {};
		matrixViewport.Viewport(framework.Window.Size);
		matrixProjection									*= matrixViewport;

		{
			::std::lock_guard<::std::mutex>							lockUpdate			(app.SolarSystem.LockUpdate);
			::ssg::solarSystemSetupBackgroundImage(app.SolarSystem.BackgroundImage, framework.Window.Size);
			::ssg::setupStars(app.SolarSystem.Stars, framework.Window.Size);
		}
	}
	double													secondsLastFrame	= framework.Timer.ElapsedMicroseconds * .001;
	secondsLastFrame									*= .001;
	::ssg::solarSystemUpdate(app.SolarSystem, secondsLastFrame, framework.Window.Size);
	Sleep(1);
	return framework.Running ? 0 : 1;
}

int													draw					(SApplication & app) {
	::ced::view_grid<::ced::SColorBGRA>						targetPixels			= {app.Framework.DoubleBuffer[app.Framework.CurrentRenderBuffer % 2].begin(), app.Framework.Window.Size};
	::ced::view_grid<uint32_t>								depthBuffer				= {app.Framework.DepthBuffer.begin(), app.Framework.Window.Size};
	return ::ssg::solarSystemDraw(app.SolarSystem, app.SolarSystem.DrawCache, app.SolarSystem.LockUpdate, targetPixels, depthBuffer);
}

void												threadDraw				(void * pApp) {
	SApplication											& app					= *(SApplication*)pApp;
	while(InterlockedCompareExchange64(&app.ThreadSignal, 0, 1)) {
		draw(app);
		++app.Framework.CurrentRenderBuffer;
	}
	Sleep(1);
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
	if(1 == ::update(app))
		return -1;
	InterlockedIncrement64(&app.ThreadSignal);
	InterlockedIncrement64(&app.ThreadSignal);
	_beginthread(threadDraw, 0, &app);
	while(app.Framework.Running) {
		if(1 == ::update(app)) {
			break;
		}
		//::draw(app);
	}
	InterlockedDecrement64(&app.ThreadSignal);
	while(0 != InterlockedCompareExchange64(&app.ThreadSignal, 0, 0))
		Sleep(10);
	::cleanup(app);
	return 0;
}
