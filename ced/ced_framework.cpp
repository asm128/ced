#include "ced_framework.h"

int													ced::frameworkCleanup		(::ced::SFramework & framework)	{ return ::ced::windowCleanup(framework.Window); }
int													ced::frameworkSetup			(::ced::SFramework & framework)	{
	::ced::SWindow											& window					= framework.Window;
	::ced::windowSetup(window);
	{
		const uint32_t											pixelCount					= window.Size.x * window.Size.y;
		framework.Pixels.resize(pixelCount);
		framework.DepthBuffer.resize(pixelCount);
		framework.DoubleBuffer[0].resize(pixelCount);
		framework.DoubleBuffer[1].resize(pixelCount);
	}
	return 0;
}

int													ced::frameworkUpdate		(::ced::SFramework & framework)	{
	::ced::SWindow											& window					= framework.Window;
	double													secondsLastFrame			= framework.Timer.Tick() * .000001;
	framework.TotalTime									+= secondsLastFrame;
	++framework.TotalFrames;
	{
		if(1 == ::ced::windowUpdate(window, (framework.UseDoubleBuffer) ? framework.DoubleBuffer[(framework.CurrentRenderBuffer+1) % 2].begin() : framework.Pixels.begin()))
			return 1;
		if(window.Resized) {
			const uint32_t											pixelCount					= window.Size.x * window.Size.y;
			framework.Pixels.resize(pixelCount);
			framework.DepthBuffer.resize(pixelCount);
			framework.DoubleBuffer[0].resize(pixelCount);
			framework.DoubleBuffer[1].resize(pixelCount);
		}
	}
	//::ced::view_grid<::ced::SColorBGRA>							targetPixels		= {framework.Pixels.begin(), window.Size};
	//memset(targetPixels.begin(), 0, sizeof(::ced::SColorBGRA) * targetPixels.size());
	//memset(framework.DepthBuffer.begin(), -1, sizeof(uint32_t) * framework.DepthBuffer.size());
	return 0;
}

