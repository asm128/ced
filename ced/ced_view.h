#include "ced_math.h"
#include <memory.h>

#ifndef CED_VIEW_H_982734982734
#define CED_VIEW_H_982734982734

namespace ced
{
	template<typename _tValue>
	struct view {
		_tValue								* Data				= 0;
		uint32_t							Count				= 0;

											view				(_tValue * data, uint32_t count)					: Data(data), Count(count) { if(Count && 0 == Data) throw(""); }
		inline constexpr					view				()									noexcept		= default;

		_tValue&							operator[]			(uint32_t index)									{ if(index >= Count) throw(""); return Data[index]; }
		const _tValue&						operator[]			(uint32_t index)					const			{ if(index >= Count) throw(""); return Data[index]; }

		_tValue *							begin				()									const			{ return Data; }
		_tValue *							end					()									const			{ return Data + Count; }
		uint32_t							size				()									const noexcept	{ return Count; }
	};

	template<typename _tValue>
	struct view_grid {
		_tValue								* Data				= 0;
		::ced::SCoord2<uint32_t>			Metrics				= 0;

											view_grid			(_tValue * data, ::ced::SCoord2<uint32_t> metrics)	: Data(data), Metrics(metrics) { if(metrics.x && metrics.y && 0 == Data) throw(""); }
		inline constexpr					view_grid			()									noexcept		= default;


		::ced::view<_tValue>				operator[]			(uint32_t index)									{ if(index >= Metrics.y) throw(""); return ::ced::view<_tValue>{&Data[Metrics.x * index], Metrics.x}; }
		::ced::view<const _tValue>			operator[]			(uint32_t index)					const			{ if(index >= Metrics.y) throw(""); return ::ced::view<_tValue>{&Data[Metrics.x * index], Metrics.x}; }

		_tValue *							begin				()									const			{ return Data; }
		_tValue *							end					()									const			{ return Data + size(); }
		uint32_t							size				()									const noexcept	{ return Metrics.x * Metrics.y; }
		::ced::SCoord2<uint32_t>			metrics				()									const noexcept	{ return Metrics; }
	};

	template<typename _tValue>
	struct container : view<_tValue> {
		using	view<_tValue>				::Data;
		using	view<_tValue>				::Count;

											~container			()									{ free(Data); }

		int32_t								resize				(uint32_t newSize)					{
			if(newSize >= Count) {
				_tValue									* newData			= (_tValue*)malloc(newSize * sizeof(_tValue));
				memcpy(newData, Data, sizeof(_tValue) * Count);
				_tValue									* oldData			= Data;
				Data								= newData;
				free(oldData);
			}
			return Count = newSize;
		}
	};

} // namespace

#endif // CED_VIEW_H_982734982734
