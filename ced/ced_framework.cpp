#include "ced_framework.h"

int													ced::frameworkUpdate		(::ced::SFramework & framework)	{
	::ced::SWindow											& window					= framework.Window;
	double													lastFrameSeconds			= framework.Timer.Tick() * .000001;
	framework.TotalTime									+= lastFrameSeconds;
	++framework.TotalFrames;
	if(1 == ::ced::windowUpdate(window, framework.Pixels))
		return 1;
	if(window.Resized) {
		free(framework.Pixels);
		const uint32_t											pixelCount			= window.Size.x * window.Size.y;
		framework.Pixels									= (::ced::SColor*)malloc(sizeof(::ced::SColor) * pixelCount);
		framework.DepthBuffer.resize(pixelCount);
	}
	::ced::view_grid<::ced::SColor>							targetPixels		= {framework.Pixels, window.Size};
	memset(targetPixels.begin(), 0, sizeof(::ced::SColor) * targetPixels.size());
	memset(framework.DepthBuffer.begin(), 0, sizeof(::ced::SColor) * framework.DepthBuffer.size());
	return 0;
}

int													ced::frameworkSetup			(::ced::SFramework & framework)	{
	::ced::SWindow											& window					= framework.Window;
	::ced::windowSetup(window);
	const uint32_t											pixelCount					= window.Size.x * window.Size.y;
	framework.Pixels									= (::ced::SColor*)malloc(sizeof(::ced::SColor) * pixelCount);
	framework.DepthBuffer.resize(pixelCount);
	return 0;
}

int													ced::frameworkCleanup		(::ced::SFramework & framework)	{
	free(framework.Pixels);
	::ced::windowCleanup(framework.Window);
	return 0;
}
