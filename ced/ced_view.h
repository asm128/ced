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
		using	view<_tValue>				::size;

											~container			()									{ free(Data); }
		int32_t								resize				(uint32_t newSize)					{
			if(newSize == Count)
				return Count;
			else if(newSize < Count)
				return Count = newSize;
			else { // if(newSize > Count)
				_tValue									* newData			= (_tValue*)malloc(newSize * sizeof(_tValue));
				memcpy(newData, Data, sizeof(_tValue) * Count);
				_tValue									* oldData			= Data;
				Data								= newData;
				free(oldData);
				return Count = newSize;
			}
		}
		int32_t								push_back			(const _tValue & valueToAdd)					{
			int32_t									newIndex			= size();
			resize(size() + 1);
			Data[newIndex]						= valueToAdd;
			return newIndex;
		}
		int32_t								pop_back			(_tValue * valueRemoved = 0)					{
			if(valueRemoved)
				*valueRemoved						= Data[Count - 1];
			resize(Count - 1);
			return Count - 1;
		}
	};

	template<typename _tValue>
	struct container_obj : view<_tValue> {
		using	view<_tValue>				::Data;
		using	view<_tValue>				::Count;
		using	view<_tValue>				::size;

											~container_obj			()								{ resize(0); free(Data); }

		int32_t								resize				(uint32_t newSize)					{
			if(newSize == Count)
				return Count;
			else if(newSize < Count) {
				for(uint32_t iElement = newSize; iElement < Count; ++iElement)
					Data[iElement].~_tValue();
				return Count = newSize;
			}
			else if(newSize > Count) {
				_tValue									* newData			= (_tValue*)malloc(newSize * sizeof(_tValue));
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					new (&newData[iElement]) _tValue(Data[iElement]);
				for(uint32_t iElement = Count; iElement < newSize; ++iElement)
					new (&newData[iElement]) _tValue();
				_tValue									* oldData			= Data;
				Data								= newData;
				free(oldData);
				return Count = newSize;
			}
			return Count = newSize;
		}

		int32_t								push_back			(const _tValue & valueToAdd)					{
			int32_t									newIndex			= size();
			resize(size() + 1);
			new (&Data[newIndex]) _tValue(valueToAdd);
			return newIndex;
		}

		int32_t								pop_back			(_tValue * valueRemoved = 0)					{
			if(valueRemoved)
				*valueRemoved						= Data[Count - 1];
			resize(Count - 1);
			return Count - 1;
		}
	};

} // namespace

#endif // CED_VIEW_H_982734982734
