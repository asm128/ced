#include "ced_math.h"
#include <memory.h>
#include <algorithm>

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
		::ced::SCoord2<uint32_t>			Metrics				= {};

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
		uint32_t							Size				= 0;

											~container			()									{
			for(uint32_t iElement = 0; iElement < Count; ++iElement)
				Data[iElement].~_tValue();
			free(Data);
		}

											container			()									= default;
											container			(const container<_tValue>& other)			{
			const uint32_t							newCount			= other.size();
			const uint32_t							newSize				= ::std::max(4U, newCount + (newCount / 4));
			_tValue									* newData			= (_tValue*)malloc(newSize * sizeof(_tValue));
			for(uint32_t iElement = 0; iElement < newCount; ++iElement)
				new (&newData[iElement]) _tValue(other.Data[iElement]);
			Size								= newSize;
			Data								= newData;
			Count								= newCount;
		}
		container<_tValue>&					operator=			(const container<_tValue>& other)			{
			const uint32_t							newCount			= other.size();
			for(uint32_t iElement = 0; iElement < Count; ++iElement)
				Data[iElement].~_tValue();

			if(newCount <= Count) {
				for(uint32_t iElement = 0; iElement < newCount; ++iElement)
					Data[iElement]					= other.Data[iElement];
			}
			else if(newCount > Size) {
				uint32_t								newSize				= ::std::max(4U, newCount + (newCount / 4));
				_tValue									* newData			= (_tValue*)malloc(newSize * sizeof(_tValue));
				for(uint32_t iElement = 0; iElement < newCount; ++iElement)
					new (&newData[iElement]) _tValue(other.Data[iElement]);
				_tValue									* oldData			= Data;
				Size								= newSize;
				Data								= newData;
				free(oldData);
			}
			else if(newCount > Count) {
				for(uint32_t iElement = 0; iElement < newCount; ++iElement)
					new (&Data[iElement]) _tValue(other.Data[iElement]);
			}
			Count							= newCount;
			return *this;
		}
		int32_t								clear				()												{ return resize(0); }
		int32_t								resize				(uint32_t newCount)								{ return resize(newCount, {}); }
		int32_t								resize				(uint32_t newCount, const _tValue & newValue)	{
			if(newCount < Count) {
				for(uint32_t iElement = Count - 1; iElement < newCount; --iElement)
					Data[iElement].~_tValue();
				return Count = newCount;
			}
			else if(newCount > Size) {
				uint32_t								newSize				= ::std::max(4U, newCount + (newCount / 4));
				_tValue									* newData			= (_tValue*)malloc(newSize * sizeof(_tValue));
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					new (&newData[iElement]) _tValue(Data[iElement]);
				for(uint32_t iElement = Count; iElement < newCount; ++iElement)
					new (&newData[iElement]) _tValue(newValue);
				_tValue									* oldData			= Data;
				Size								= newSize;
				Data								= newData;
				for(uint32_t iElement = 0; iElement < Count; ++iElement)
					oldData[iElement].~_tValue();
				free(oldData);
			}
			else if(newCount > Count) {
				for(uint32_t iElement = Count; iElement < newCount; ++iElement)
					new (&Data[iElement]) _tValue(newValue);
			}
			return Count = newCount;
		}
		int32_t								push_back			(const _tValue & valueToPush)		{
			uint32_t								newIndex			= Count;
			resize(Count + 1, valueToPush);
			return newIndex;
		}
		int32_t								pop_back			(_tValue * valueToPop = 0)		{
			if(valueToPop)
				*valueToPop							= Data[Count - 1];
			resize(Count - 1);
			return Count - 1;
		}
		int32_t								remove_unordered	(uint32_t indexToRemove)		{ return this->pop_back(&this->operator[](indexToRemove)); }
	};

} // namespace

#endif // CED_VIEW_H_982734982734
