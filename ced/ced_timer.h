#include <chrono>

#ifndef CED_TIMER_H_2908347298347
#define CED_TIMER_H_2908347298347

namespace ced
{
	struct STimer {
		::std::chrono::time_point<::std::chrono::system_clock>			TimeCurrent				= ::std::chrono::system_clock::now();
		::std::chrono::time_point<::std::chrono::system_clock>			TimePrevious			= ::std::chrono::system_clock::now();
		uint64_t														ElapsedMicroseconds		= 0;

		uint64_t														Tick					()	{
			TimeCurrent														= ::std::chrono::system_clock::now();
			auto																diff					= TimeCurrent - TimePrevious;
			TimePrevious													= TimeCurrent;
			return ElapsedMicroseconds = ::std::chrono::duration_cast<::std::chrono::microseconds>(diff).count();
		}
	};
} // namespace

#endif // CED_TIMER_H_2908347298347
