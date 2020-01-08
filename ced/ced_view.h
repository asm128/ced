#include "ced_math.h"
#ifndef CED_VIEW_H_982734982734
#define CED_VIEW_H_982734982734

namespace ced
{
	template<typename _tValue>
	struct view {
		_tValue								* Data	;
		uint32_t							Count	;

		_tValue&							operator[]			(uint32_t index)					{ if(index >= Count) throw(""); return Data[index]; }
		const _tValue&						operator[]			(uint32_t index)	const			{ if(index >= Count) throw(""); return Data[index]; }

		_tValue *							begin	()								const			{ return Data; }
		_tValue *							end		()								const			{ return Data + Count; }
		uint32_t							size	()								const noexcept	{ return Count; }
	};

	template<typename _tValue>
	struct view_grid {
		_tValue								* Data	;
		::ced::SCoord2<uint32_t>			Metrics	;

		::ced::view<_tValue>				operator[]			(uint32_t index)					{ if(index >= Metrics.y) throw(""); return ::ced::view<_tValue>{&Data[Metrics.x * index], Metrics.x}; }
		::ced::view<const _tValue>			operator[]			(uint32_t index)	const			{ if(index >= Metrics.y) throw(""); return ::ced::view<_tValue>{&Data[Metrics.x * index], Metrics.x}; }

		_tValue *							begin				()					const			{ return Data; }
		_tValue *							end					()					const			{ return Data + size(); }
		uint32_t							size				()					const noexcept	{ return Metrics.x * Metrics.y; }
		::ced::SCoord2<uint32_t>			metrics				()					const noexcept	{ return Metrics; }
	};
} // namespace

#endif // CED_VIEW_H_982734982734
