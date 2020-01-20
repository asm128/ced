#include "ced_window.h"
#include "ced_timer.h"

#ifndef CED_FRAMEWORK_H_239847239847
#define CED_FRAMEWORK_H_239847239847

namespace ced
{
	struct SFramework {
		::ced::SWindow										Window				= {};
		::ced::SColorBGRA										* Pixels			= 0;
		::ced::container<uint32_t>							DepthBuffer			= {};
		::ced::STimer										Timer				= {};
		uint64_t											TotalFrames			= 0;
		double												TotalTime			= 0;
		bool												Running				= true;
	};

	int													frameworkSetup		(::ced::SFramework & framework);
	int													frameworkUpdate		(::ced::SFramework & framework);
	int													frameworkCleanup	(::ced::SFramework & framework);
} // namespace

#endif // CED_FRAMEWORK_H_239847239847
