#include <cstdint>

#ifndef CED_COLOR_H_23904872398475
#define CED_COLOR_H_23904872398475

namespace ced
{
#pragma pack( push, 1 )
	template <typename _tBase> struct color_bgr		{ _tBase b, g, r;		};
	template <typename _tBase> struct color_rgb		{ _tBase r, g, b;		};
	template <typename _tBase> struct color_bgra	{ _tBase b, g, r, a;	};
	template <typename _tBase> struct color_rgba	{ _tBase r, g, b, a;	};

	template<typename _tValue>
	inline			constexpr	const _tValue	&		min		(const _tValue & a, const _tValue & b)						{ return (a < b) ? a : b; }
	template<typename _tValue>
	inline			constexpr	const _tValue	&		max		(const _tValue & a, const _tValue & b)						{ return (a > b) ? a : b; }
	template<typename _tValue>
	inline			constexpr	const _tValue	&		clamp	(const _tValue & value, const _tValue & min, const _tValue & max)					{ return ::ced::min(max, ::ced::max(value, min)); }

	// Stores RGBA color channels
	struct SColorRGBA : public ::ced::color_rgba<uint8_t> {
		constexpr									SColorRGBA		(const SColorRGBA& other)											noexcept	= default;
		constexpr									SColorRGBA		(uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0, uint8_t a_=0xff)	noexcept	: color_rgba<uint8_t>{b_, g_, r_, a_}																																										{}
		constexpr									SColorRGBA		(uint32_t other)													noexcept	: color_rgba<uint8_t>
			{ ((uint8_t)(((other & 0x000000FF) >> 0)))
			, ((uint8_t)(((other & 0x0000FF00) >> 8)))
			, ((uint8_t)(((other & 0x00FF0000) >> 16)))
			, ((uint8_t)(((other & 0xFF000000) >> 24)))
			}
		{}

		constexpr					operator		uint32_t		()															const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)b) << 16) | (((uint32_t)g) << 8) | (((uint32_t)r) << 0);																								}
									SColorRGBA&		operator=		(const SColorRGBA& color)											noexcept	= default;

		constexpr					bool			operator ==		(uint32_t other)											const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		constexpr					bool			operator ==		(const SColorRGBA& other)									const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		constexpr					bool			operator !=		(const SColorRGBA& other)									const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		constexpr					SColorRGBA		operator *		(const SColorRGBA& color)									const	noexcept	{ return SColorRGBA((uint8_t)::ced::clamp(r * (uint16_t)color.r, 0, 255)	, (uint8_t)::ced::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::ced::clamp(b * (uint16_t)color.b, 0, 255)	, a);	}
		constexpr					SColorRGBA		operator +		(const SColorRGBA& color)									const	noexcept	{ return SColorRGBA((uint8_t)::ced::clamp(r + (uint16_t)color.r, 0, 255)	, (uint8_t)::ced::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::ced::clamp(b + (uint16_t)color.b, 0, 255)	, a);	}
		constexpr					SColorRGBA		operator *		(double scalar)												const	noexcept	{ return SColorRGBA((uint8_t)::ced::clamp(r * scalar, 0.0 , 255.0)			, (uint8_t)::ced::clamp(g * scalar, 0.0 , 255.0 )		, (uint8_t)::ced::clamp(b * scalar, 0.0,  255.0 )		, a);	}
		constexpr					SColorRGBA		operator /		(double scalar)												const				{ return SColorRGBA((uint8_t)::ced::clamp(r / scalar, 0.0 , 255.0)			, (uint8_t)::ced::clamp(g / scalar, 0.0 , 255.0 )		, (uint8_t)::ced::clamp(b / scalar, 0.0,  255.0 )		, a);	}
	};	// struct

	// Stores BGRA color channels
	struct SColorBGRA {
									uint8_t			b = 0, g = 0, r = 0, a = 0xff;
									//uint8_t			b, g, r, a;

		constexpr									SColorBGRA		()																noexcept	= default;
		constexpr									SColorBGRA		(const SColorRGBA& other)										noexcept	: b(other.b), g(other.g), r(other.r), a(other.a)																																										{}
		constexpr									SColorBGRA		(const SColorBGRA& other)										noexcept	= default;
		constexpr									SColorBGRA		(uint8_t b_, uint8_t g_, uint8_t r_, uint8_t a_=0xff)			noexcept	: b(b_), g(g_), r(r_), a(a_)																																										{}
		constexpr									SColorBGRA		(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0))), g((uint8_t)(((other & 0x0000FF00) >> 8))), r((uint8_t)(((other & 0x00FF0000) >> 16))), a((uint8_t)(((other & 0xFF000000) >> 24)))						{}

		constexpr					operator		SColorRGBA		()														const	noexcept	{ return {r, g, b, a};																																												}
		constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)a) << 24) | (((uint32_t)r) << 16) | (((uint32_t)g) << 8) | (((uint32_t)b) << 0);																								}
									SColorBGRA&		operator=		(const SColorBGRA& color)										noexcept	= default;
		constexpr					bool			operator ==		(uint32_t other)										const	noexcept	{ return other == *((const uint32_t*)this);																																							}
		constexpr					bool			operator ==		(const SColorBGRA& other)								const	noexcept	{ return b == other.b && g == other.g && r == other.r && a == other.a;																																}
		constexpr					bool			operator !=		(const SColorBGRA& other)								const	noexcept	{ return b != other.b || g != other.g || r != other.r || a != other.a;																																}
		constexpr					SColorBGRA		operator *		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA{(uint8_t)::std::min((uint16_t)b * color.b, 255)	, (uint8_t)::std::min((uint16_t)g * color.g, 255)	,(uint8_t)::std::min((uint16_t)r * color.r, 255), a};			}
		constexpr					SColorBGRA		operator +		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA{(uint8_t)::std::min((uint16_t)b + color.b, 255)	, (uint8_t)::std::min((uint16_t)g + color.g, 255)	,(uint8_t)::std::min((uint16_t)r + color.r, 255), a};			}
		constexpr					SColorBGRA		operator -		(const SColorBGRA& color)								const	noexcept	{ return SColorBGRA{(uint8_t)::ced::clamp(b - (int16_t)color.b, 0, 255), (uint8_t)::ced::clamp(g - (int16_t)color.g, 0, 255),(uint8_t)::ced::clamp(r - (int16_t)color.r, 0, 255), a};	}
		constexpr					SColorBGRA		operator *		(float scalar)											const	noexcept	{ return SColorBGRA{(uint8_t)::ced::clamp(b * scalar, 0.0f, 255.0f)	, (uint8_t)::ced::clamp(g * scalar, 0.0f, 255.0f)	,(uint8_t)::ced::clamp(r * scalar, 0.0f, 255.0f), a};			}
		constexpr					SColorBGRA		operator /		(float scalar)											const				{ return SColorBGRA{(uint8_t)::ced::clamp(b / scalar, 0.0f, 255.0f)	, (uint8_t)::ced::clamp(g / scalar, 0.0f, 255.0f)	,(uint8_t)::ced::clamp(r / scalar, 0.0f, 255.0f), a};			}
		constexpr					SColorBGRA		operator *		(double scalar)											const	noexcept	{ return SColorBGRA{(uint8_t)::ced::clamp(b * scalar, 0.0,  255.0 )	, (uint8_t)::ced::clamp(g * scalar, 0.0 , 255.0 )	,(uint8_t)::ced::clamp(r * scalar, 0.0, 255.0),	a};				}
		constexpr					SColorBGRA		operator /		(double scalar)											const				{ return SColorBGRA{(uint8_t)::ced::clamp(b / scalar, 0.0,  255.0 )	, (uint8_t)::ced::clamp(g / scalar, 0.0 , 255.0 )	,(uint8_t)::ced::clamp(r / scalar, 0.0, 255.0),	a};				}
									SColorBGRA&		operator *=		(float scalar)													noexcept	{ b = (uint8_t)::ced::clamp(b * scalar, 0.0f, 255.0f); g = (uint8_t)::ced::clamp(g * scalar, 0.0f, 255.0f); r = (uint8_t)::ced::clamp(r * scalar, 0.0f, 255.0f);	return *this; }
									SColorBGRA&		operator /=		(float scalar)																{ b = (uint8_t)::ced::clamp(b / scalar, 0.0f, 255.0f); g = (uint8_t)::ced::clamp(g / scalar, 0.0f, 255.0f); r = (uint8_t)::ced::clamp(r / scalar, 0.0f, 255.0f);	return *this; }
									SColorBGRA&		operator *=		(double scalar)													noexcept	{ b = (uint8_t)::ced::clamp(b * scalar, 0.0,  255.0 ); g = (uint8_t)::ced::clamp(g * scalar, 0.0 , 255.0 ); r = (uint8_t)::ced::clamp(r * scalar, 0.0, 255.0);		return *this; }
									SColorBGRA&		operator /=		(double scalar)																{ b = (uint8_t)::ced::clamp(b / scalar, 0.0,  255.0 ); g = (uint8_t)::ced::clamp(g / scalar, 0.0 , 255.0 ); r = (uint8_t)::ced::clamp(r / scalar, 0.0, 255.0);		return *this; }
	};	// struct

	typedef						uint16_t		SColor16;

	// Stores BGR color channels
	struct SColorBGR {
									uint8_t							b = 0, g = 0, r = 0;

		constexpr					SColorBGR						()																noexcept	= default;
		constexpr					SColorBGR						(const SColorBGR& otherColorInt)								noexcept	= default;
		constexpr					SColorBGR						(uint8_t b_, uint8_t g_, uint8_t r_)							noexcept	: b(b_), g(g_), r(r_)																																														{}
		constexpr					SColorBGR						(const SColorBGRA& other)										noexcept	: b(other.b), g(other.b), r(other.r)																																										{}
		//constexpr					SColorBGR						(const SColor16& other)											noexcept	: b((uint8_t)((other & 0x001F) / ((float)(0x1F))*255))	, g((uint8_t)(((other & 0x07E0) >>5) / ((float)(0x3F))*255)), r((uint8_t)(((other & 0xF800) >>11) / ((float)(0x1F))*255))							{}
		constexpr					SColorBGR						(uint32_t other)												noexcept	: b((uint8_t)(((other & 0x000000FF) >> 0)))				, g((uint8_t)((other & 0x0000FF00) >> 8))					, r((uint8_t)(((other & 0x00FF0000) >> 16)))											{}

		constexpr					operator		uint32_t		()														const	noexcept	{ return				0xFF000000 | (((uint32_t)r) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)b) << 0);																			}
		constexpr					operator		SColorBGRA		()														const	noexcept	{ return {b, g, r, 0xFF};																																													}
		constexpr					operator		SColor16		()														const	noexcept	{ return (((uint16_t)(b * (1/255.0f) * 0x001F)) << 0)	| (((uint16_t)(g * (1/255.0f) * 0x003F)) << 5)	| (((uint16_t)(r * (1/255.0f) * 0x001F)) << 11 );													}
									SColorBGR&		operator=		(const SColorBGR& color)										noexcept	= default;
		constexpr					bool			operator ==		(const SColorBGRA& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																						}
		constexpr					bool			operator ==		(const SColorBGR& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																						}
		constexpr					bool			operator ==		(const SColor16& other)									const	noexcept	{ return operator == (SColorBGR(other));																																									}
		constexpr					bool			operator !=		(const SColorBGRA& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																						}
		constexpr					bool			operator !=		(const SColorBGR& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																						}
		constexpr					bool			operator !=		(const SColor16& other)									const	noexcept	{ return operator != (SColorBGR(other));																																									}
		constexpr					SColorBGR		operator *		(float scalar)											const	noexcept	{ return ::ced::SColorBGR {(uint8_t)::ced::clamp(b * scalar, 0.0f, 255.0f)		, (uint8_t)::ced::clamp(g * scalar, 0.0f, 255.0f)		, (uint8_t)::ced::clamp(r * scalar, 0.0f, 255.0f)};					}
		constexpr					SColorBGR		operator /		(float scalar)											const				{ return ::ced::SColorBGR {(uint8_t)::ced::clamp(b / scalar, 0.0f, 255.0f)		, (uint8_t)::ced::clamp(g / scalar, 0.0f, 255.0f)		, (uint8_t)::ced::clamp(r / scalar, 0.0f, 255.0f)};					}
		constexpr					SColorBGR		operator *		(double scalar)											const	noexcept	{ return ::ced::SColorBGR {(uint8_t)::ced::clamp(b * scalar, 0.0,  255.0)		, (uint8_t)::ced::clamp(g * scalar, 0.0,  255.0)		, (uint8_t)::ced::clamp(r * scalar, 0.0,  255.0)};					}
		constexpr					SColorBGR		operator /		(double scalar)											const				{ return ::ced::SColorBGR {(uint8_t)::ced::clamp(b / scalar, 0.0,  255.0)		, (uint8_t)::ced::clamp(g / scalar, 0.0,  255.0)		, (uint8_t)::ced::clamp(r / scalar, 0.0,  255.0)};					}
		constexpr					SColorBGR		operator *		(const SColorBGR& color)								const	noexcept	{ return ::ced::SColorBGR {(uint8_t)::ced::clamp(b * (uint16_t)color.b, 0, 255) , (uint8_t)::ced::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::ced::clamp(r * (uint16_t)color.r, 0, 255)};			}
		constexpr					SColorBGR		operator +		(const SColorBGR& color)								const	noexcept	{ return ::ced::SColorBGR {(uint8_t)::ced::clamp(b + (uint16_t)color.b, 0, 255) , (uint8_t)::ced::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::ced::clamp(r + (uint16_t)color.r, 0, 255)};			}
		constexpr					SColorBGRA		operator *		(const SColorBGRA& color)								const	noexcept	{ return ::ced::SColorBGRA{(uint8_t)::ced::clamp(b * (uint16_t)color.b, 0, 255) , (uint8_t)::ced::clamp(g * (uint16_t)color.g, 0, 255)	, (uint8_t)::ced::clamp(r * (uint16_t)color.r, 0, 255), color.a};	}
		constexpr					SColorBGRA		operator +		(const SColorBGRA& color)								const	noexcept	{ return ::ced::SColorBGRA{(uint8_t)::ced::clamp(b + (uint16_t)color.b, 0, 255) , (uint8_t)::ced::clamp(g + (uint16_t)color.g, 0, 255)	, (uint8_t)::ced::clamp(r + (uint16_t)color.r, 0, 255), color.a};	}
	};	// struct

	// Stores RGB color channels
	struct SColorRGB {
									uint8_t							r = 0, g = 0, b = 0;

		constexpr					SColorRGB						()																noexcept	= default;
		constexpr					SColorRGB						(const SColorRGB& otherColorInt)								noexcept	= default;
		constexpr					SColorRGB						(uint8_t b_, uint8_t g_, uint8_t r_)							noexcept	: r(r_), g(g_), b(b_)																																													{}
		constexpr					SColorRGB						(const SColorRGBA& other)										noexcept	: r(other.r), g(other.g), b(other.b)																																									{}
		//constexpr					SColorRGB						(const SColor16& other)											noexcept	: r((uint8_t)((other & 0x001F) / ((float)(0x1F))*255))	, g((uint8_t)(((other & 0x07E0) >>5) / ((float)(0x3F))*255)), b((uint8_t)(((other & 0xF800) >>11) / ((float)(0x1F))*255))						{}
		constexpr					SColorRGB						(uint32_t other)												noexcept	: r((uint8_t)(((other & 0x000000FF) >> 0)))				, g((uint8_t)((other & 0x0000FF00) >> 8))					, b((uint8_t)(((other & 0x00FF0000) >> 16)))										{}

		constexpr					operator		uint32_t		()														const	noexcept	{ return				0xFF000000 | (((uint32_t)b) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)r) << 0);																			}
		constexpr					operator		SColorRGBA		()														const	noexcept	{ return SColorRGBA(	0xFF000000 | (((uint32_t)b) << 16)	| (((uint32_t)g) << 8)							| (((uint32_t)r) << 0));																	}
		constexpr					operator		SColor16		()														const	noexcept	{ return (((uint16_t)(r * (1/255.0f) * 0x001F)) << 0)	| (((uint16_t)(g * (1/255.0f) * 0x003F)) << 5)	| (((uint16_t)(b * (1/255.0f) * 0x001F)) << 11 );												}
									SColorRGB&		operator=		(const SColorRGB& color)										noexcept	= default;
		constexpr					bool			operator ==		(const SColorRGBA& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																					}
		constexpr					bool			operator ==		(const SColorRGB& other)								const	noexcept	{ return r == other.r && g == other.g && b == other.b;																																					}
		constexpr					bool			operator ==		(const SColor16& other)									const	noexcept	{ return operator == (SColorRGB(other));																																								}
		constexpr					bool			operator !=		(const SColorRGBA& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																					}
		constexpr					bool			operator !=		(const SColorRGB& other)								const	noexcept	{ return r != other.r || g != other.g || b != other.b;																																					}
		constexpr					bool			operator !=		(const SColor16& other)									const	noexcept	{ return operator != (SColorRGB(other));																																								}
		constexpr					SColorRGB		operator *		(float scalar)											const	noexcept	{ return ::ced::SColorRGB((uint8_t)::ced::clamp(r * scalar, 0.0f, 255.0f),	(uint8_t)::ced::clamp(g * scalar, 0.0f, 255.0f),	(uint8_t)::ced::clamp(b * scalar, 0.0f, 255.0f));						}
		constexpr					SColorRGB		operator /		(float scalar)											const				{ return ::ced::SColorRGB((uint8_t)::ced::clamp(r / scalar, 0.0f, 255.0f),	(uint8_t)::ced::clamp(g / scalar, 0.0f, 255.0f),	(uint8_t)::ced::clamp(b / scalar, 0.0f, 255.0f));						}
		constexpr					SColorRGB		operator *		(double scalar)											const	noexcept	{ return ::ced::SColorRGB((uint8_t)::ced::clamp(r * scalar, 0.0,  255.0), 	(uint8_t)::ced::clamp(g * scalar, 0.0,  255.0),	(uint8_t)::ced::clamp(b * scalar, 0.0,  255.0));							}
		constexpr					SColorRGB		operator /		(double scalar)											const				{ return ::ced::SColorRGB((uint8_t)::ced::clamp(r / scalar, 0.0,  255.0), 	(uint8_t)::ced::clamp(g / scalar, 0.0,  255.0),	(uint8_t)::ced::clamp(b / scalar, 0.0,  255.0));							}
		constexpr					SColorRGB		operator *		(const SColorRGB& color)								const	noexcept	{ return ::ced::SColorRGB((uint8_t)::ced::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::ced::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::ced::clamp(b * (uint16_t)color.b, 0, 255));			}
		constexpr					SColorRGB		operator +		(const SColorRGB& color)								const	noexcept	{ return ::ced::SColorRGB((uint8_t)::ced::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::ced::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::ced::clamp(b + (uint16_t)color.b, 0, 255));			}
		constexpr					SColorRGBA		operator *		(const SColorRGBA& color)								const	noexcept	{ return ::ced::SColorRGBA((uint8_t)::ced::clamp(r * (uint16_t)color.r, 0, 255), (uint8_t)::ced::clamp(g * (uint16_t)color.g, 0, 255), (uint8_t)::ced::clamp(b * (uint16_t)color.b, 0, 255), color.a);	}
		constexpr					SColorRGBA		operator +		(const SColorRGBA& color)								const	noexcept	{ return ::ced::SColorRGBA((uint8_t)::ced::clamp(r + (uint16_t)color.r, 0, 255), (uint8_t)::ced::clamp(g + (uint16_t)color.g, 0, 255), (uint8_t)::ced::clamp(b + (uint16_t)color.b, 0, 255), color.a);	}
	};	// struct

	// Stores RGBA floating point color channels
	struct SColorFloat {
									float			r = 0, g = 0, b = 0, a = 0;		// store the color values as floating point ranged in the values (0,1)

		constexpr									SColorFloat		()																noexcept	= default;
		constexpr									SColorFloat		(const SColorFloat& color)										noexcept	= default;
		constexpr									SColorFloat		(float _r, float _g, float _b, float _a=1.0f)					noexcept	: r(::ced::clamp(_r, 0.0f, 1.0f)) ,g(::ced::clamp(_g, 0.0f, 1.0f)) ,b(::ced::clamp(_b, 0.0f, 1.0f)) ,a(::ced::clamp(_a, 0.0f, 1.0f))																{}
		//constexpr									SColorFloat		(uint16_t Color)												noexcept	: SColorFloat(SColorBGR(Color))																																										{}
		//constexpr									SColorFloat		(uint32_t Color)												noexcept	: r(((Color & 0x00FF0000L)>>16)	*(1/255.0f)), g(((Color & 0x0000FF00L)>>8)*(1/255.0f)), b(((Color & 0x000000FFL)>>0)*(1/255.0f)), a(((Color & 0xFF000000L)>>24)*(1/255.0f))							{}
		constexpr									SColorFloat		(SColorBGRA Color)												noexcept	: r(Color.r * (1/255.0f)), g(Color.g * (1/255.0f)), b(Color.b * (1/255.0f)), a(Color.a * (1/255.0f))																								{}
		//constexpr									SColorFloat		(SColorBGR Color)												noexcept	: r(Color.r	* (1/255.0f)), g(Color.g * (1/255.0f)), b(Color.b * (1/255.0f)), a(1.0f)																												{}
		constexpr									SColorFloat		(const float* rgbaColor)													: r(rgbaColor[0]), g(rgbaColor[1]), b(rgbaColor[2]), a(rgbaColor[3])																																{}

									SColorFloat&	operator=		(const SColorFloat& color)													= default;
		//constexpr					operator		uint32_t		()														const	noexcept	{ return (((uint32_t)(a * 255.0f)) << 24) | (((uint32_t)(r * 255.0f)) << 16) | (((uint32_t)(g * 255.0f)) << 8) | ((uint32_t)(b*255.0f));															}
		constexpr					operator		SColorBGRA		()														const	noexcept	{ return {(uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255), (uint8_t)(a * 255)};																											}
		//constexpr					operator		SColorBGR		()														const	noexcept	{ return SColorBGR ((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));																													}
		//constexpr					operator		SColor16		()														const	noexcept	{ return ((((uint16_t)(r * 0x1F)) << 11) | (((uint16_t)(g * 0x3F)) << 5) | (((uint16_t)(b * 0x1F)) << 0));																							}
		constexpr					operator		const float*	()														const	noexcept	{ return &r;																																														}
		constexpr					bool			operator ==		(const SColorFloat& color)								const	noexcept	{ return SColorBGR((uint8_t)(color.b * 255), (uint8_t)(color.g * 255), (uint8_t)(color.r * 255)) == SColorBGR((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));											}
		constexpr					bool			operator !=		(const SColorFloat& color)								const	noexcept	{ return SColorBGR((uint8_t)(color.b * 255), (uint8_t)(color.g * 255), (uint8_t)(color.r * 255)) != SColorBGR((uint8_t)(b * 255), (uint8_t)(g * 255), (uint8_t)(r * 255));											}
									SColorFloat&	operator +=		(const SColorFloat& color)										noexcept	{ r = r + color.r;					g = g + color.g;				b = b + color.b;				return Clamp();																					}
									SColorFloat&	operator *=		(const SColorFloat& color)										noexcept	{ r = r * color.r;					g = g * color.g;				b = b * color.b;				return Clamp();																					}
									SColorFloat&	operator *=		(double scalar)													noexcept	{ r = (float)(r * scalar);			g = (float)(g * scalar);		b = (float)(b * scalar);		return Clamp();																					}
									SColorFloat&	operator /=		(double scalar)																{ r = (float)(r / scalar);			g = (float)(g / scalar);		b = (float)(b / scalar);		return Clamp();																					}
									SColorFloat&	operator *=		(float scalar)													noexcept	{ r = r * scalar;					g = g * scalar;					b = b * scalar;					return Clamp();																					}
									SColorFloat&	operator /=		(float scalar)																{ r = r / scalar;					g = g / scalar;					b = b / scalar;					return Clamp();																					}
									SColorFloat&	operator *=		(const SColorBGRA& color)										noexcept	{ r = r * (color.r * (1/255.0f));	g = g * (color.g * (1/255.0f));	b = b * (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat&	operator +=		(const SColorBGRA& color)										noexcept	{ r = r + (color.r * (1/255.0f));	g = g + (color.g * (1/255.0f));	b = b + (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat&	operator *=		(const SColorBGR& color)										noexcept	{ r = r * (color.r * (1/255.0f));	g = g * (color.g * (1/255.0f));	b = b * (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat&	operator +=		(const SColorBGR& color)										noexcept	{ r = r + (color.r * (1/255.0f));	g = g + (color.g * (1/255.0f));	b = b + (color.b * (1/255.0f));	return Clamp();																					}
									SColorFloat		operator *		(const SColorBGRA& color)								const	noexcept	{ return ::ced::SColorFloat{r * (color.r * (1/255.0f)), g * (color.g * (1/255.0f)), b * (color.b * (1/255.0f)), a};																				}
									SColorFloat		operator +		(const SColorBGRA& color)								const	noexcept	{ return ::ced::SColorFloat{r + (color.r * (1/255.0f)), g + (color.g * (1/255.0f)), b + (color.b * (1/255.0f)), a};																				}
									SColorFloat		operator *		(const SColorBGR& color)								const	noexcept	{ return ::ced::SColorFloat{r * (color.r * (1/255.0f)), g * (color.g * (1/255.0f)), b * (color.b * (1/255.0f)), a};																				}
									SColorFloat		operator +		(const SColorBGR& color)								const	noexcept	{ return ::ced::SColorFloat{r + (color.r * (1/255.0f)), g + (color.g * (1/255.0f)), b + (color.b * (1/255.0f)), a};																				}
		constexpr					SColorFloat		operator *		(const SColorFloat& color)								const	noexcept	{ return ::ced::SColorFloat{r * color.r, g * color.g, b * color.b, a};	}
		constexpr					SColorFloat		operator +		(const SColorFloat& color)								const	noexcept	{ return ::ced::SColorFloat{r + color.r, g + color.g, b + color.b, a};	}
		constexpr					SColorFloat		operator -		(const SColorFloat& color)								const	noexcept	{ return ::ced::SColorFloat(r - color.r, g - color.g, b - color.b, a);	}
		constexpr					SColorFloat		operator *		(double scalar)											const	noexcept	{ return ::ced::SColorFloat{(float)(r * scalar), (float)(g * scalar), (float)(b * scalar), a};											}
		constexpr					SColorFloat		operator *		(float scalar)											const	noexcept	{ return ::ced::SColorFloat{(r * scalar), (g * scalar),	(b * scalar)};													}
		constexpr					SColorFloat		operator /		(double scalar)											const				{ return ::ced::SColorFloat{(float)(r / scalar), (float)(g / scalar), (float)(b / scalar), a};	}
		constexpr					SColorFloat		operator /		(float scalar)											const				{ return ::ced::SColorFloat(r / scalar,	g / scalar,	b / scalar);								}

		SColorFloat&	Clamp			()																noexcept	{ r = ::ced::clamp(r, 0.0f, 1.0f); g = ::ced::clamp(g, 0.0f, 1.0f); b = ::ced::clamp(b, 0.0f, 1.0f); return *this;																				}
	};	// struct

	static constexpr	const ::ced::SColorFloat	BLACK			= {0.0f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	WHITE			= {1.0f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	RED				= {1.0f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	GREEN			= {0.0f, 1.0f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	BLUE			= {0.0f, 0.0f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	YELLOW			= {1.0f, 1.0f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	MAGENTA			= {1.0f, 0.0f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	CYAN			= {0.0f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	ORANGE			= {1.0f, 0.647f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	PANOCHE			= {0.25f, 0.5f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	PURPLE			= {0.5f, 0.1f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	TURQUOISE		= {0.2f, 1.0, 0.65f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	BROWN			= {0.4f, 0.223f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	GRAY			= {0.5f, 0.5f, 0.5f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKGRAY		= {0.25f, 0.25f, 0.25f, 1.0f				};
	static constexpr	const ::ced::SColorFloat	DARKRED			= {0.5f, 0.0f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKGREEN		= {0.0f, 0.5f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKBLUE		= {0.0f, 0.0f, 0.5f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKYELLOW		= {0.5f, 0.5f, 0.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKMAGENTA		= {0.5f, 0.0f, 0.5f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKCYAN		= {0.0f, 0.5f, 0.5f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	DARKORANGE		= {1.0f, 0.5490196078431373f, 0.0f, 1.0f	};
	static constexpr	const ::ced::SColorFloat	LIGHTGRAY		= {0.75f, 0.75f, 0.75f, 1.0f				};
	static constexpr	const ::ced::SColorFloat	LIGHTRED		= {1.0f, 0.25f, 0.25f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	LIGHTGREEN		= {0.5f,  1.0f, 0.5f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	LIGHTBLUE		= {0.25f, 0.25f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	LIGHTYELLOW		= {1.0f, 1.0f, 0.25f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	LIGHTMAGENTA	= {1.0f, 0.25f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	LIGHTCYAN		= {0.25f, 1.0f, 1.0f, 1.0f					};
	static constexpr	const ::ced::SColorFloat	LIGHTORANGE		= {1.0f, 0.780f, 0.25f, 1.0f				};

	static constexpr	const ::ced::SColorFloat	COLOR_TABLE	[]	=
		{ ::ced::SColorBGRA{249, 235, 234    }
		, ::ced::SColorBGRA{242, 215, 213    }
		, ::ced::SColorBGRA{230, 176, 170    }
		, ::ced::SColorBGRA{217, 136, 128    }
		, ::ced::SColorBGRA{205, 97, 85      }
		, ::ced::SColorBGRA{192, 57, 43      }
		, ::ced::SColorBGRA{169, 50, 38      }
		, ::ced::SColorBGRA{146, 43, 33      }
		, ::ced::SColorBGRA{123, 36, 28      }
		, ::ced::SColorBGRA{100, 30, 22      }
		, ::ced::SColorBGRA{253, 237, 236    }
		, ::ced::SColorBGRA{250, 219, 216    }
		, ::ced::SColorBGRA{245, 183, 177    }
		, ::ced::SColorBGRA{241, 148, 138    }
		, ::ced::SColorBGRA{236, 112, 99     }
		, ::ced::SColorBGRA{231, 76, 60      }
		, ::ced::SColorBGRA{203, 67, 53      }
		, ::ced::SColorBGRA{176, 58, 46      }
		, ::ced::SColorBGRA{148, 49, 38      }
		, ::ced::SColorBGRA{120, 40, 31      }
		, ::ced::SColorBGRA{245, 238, 248    }
		, ::ced::SColorBGRA{235, 222, 240    }
		, ::ced::SColorBGRA{215, 189, 226    }
		, ::ced::SColorBGRA{195, 155, 211    }
		, ::ced::SColorBGRA{175, 122, 197    }
		, ::ced::SColorBGRA{155, 89, 182     }
		, ::ced::SColorBGRA{136, 78, 160     }
		, ::ced::SColorBGRA{118, 68, 138     }
		, ::ced::SColorBGRA{99, 57, 116      }
		, ::ced::SColorBGRA{81, 46, 95       }
		, ::ced::SColorBGRA{244, 236, 247    }
		, ::ced::SColorBGRA{232, 218, 239    }
		, ::ced::SColorBGRA{210, 180, 222    }
		, ::ced::SColorBGRA{187, 143, 206    }
		, ::ced::SColorBGRA{165, 105, 189    }
		, ::ced::SColorBGRA{142, 68, 173     }
		, ::ced::SColorBGRA{125, 60, 152     }
		, ::ced::SColorBGRA{108, 52, 131     }
		, ::ced::SColorBGRA{91, 44, 111      }
		, ::ced::SColorBGRA{74, 35, 90       }
		, ::ced::SColorBGRA{234, 242, 248    }
		, ::ced::SColorBGRA{212, 230, 241    }
		, ::ced::SColorBGRA{169, 204, 227    }
		, ::ced::SColorBGRA{127, 179, 213    }
		, ::ced::SColorBGRA{84, 153, 199     }
		, ::ced::SColorBGRA{41, 128, 185     }
		, ::ced::SColorBGRA{36, 113, 163     }
		, ::ced::SColorBGRA{31, 97, 141      }
		, ::ced::SColorBGRA{26, 82, 118      }
		, ::ced::SColorBGRA{21, 67, 96       }
		, ::ced::SColorBGRA{235, 245, 251    }
		, ::ced::SColorBGRA{214, 234, 248    }
		, ::ced::SColorBGRA{174, 214, 241    }
		, ::ced::SColorBGRA{133, 193, 233    }
		, ::ced::SColorBGRA{93, 173, 226     }
		, ::ced::SColorBGRA{52, 152, 219     }
		, ::ced::SColorBGRA{46, 134, 193     }
		, ::ced::SColorBGRA{40, 116, 166     }
		, ::ced::SColorBGRA{33, 97, 140      }
		, ::ced::SColorBGRA{27, 79, 114      }
		, ::ced::SColorBGRA{232, 248, 245    }
		, ::ced::SColorBGRA{209, 242, 235    }
		, ::ced::SColorBGRA{163, 228, 215    }
		, ::ced::SColorBGRA{118, 215, 196    }
		, ::ced::SColorBGRA{72, 201, 176     }
		, ::ced::SColorBGRA{26, 188, 156     }
		, ::ced::SColorBGRA{23, 165, 137     }
		, ::ced::SColorBGRA{20, 143, 119     }
		, ::ced::SColorBGRA{17, 120, 100     }
		, ::ced::SColorBGRA{14, 98, 81       }
		, ::ced::SColorBGRA{232, 246, 243    }
		, ::ced::SColorBGRA{208, 236, 231    }
		, ::ced::SColorBGRA{162, 217, 206    }
		, ::ced::SColorBGRA{115, 198, 182    }
		, ::ced::SColorBGRA{69, 179, 157     }
		, ::ced::SColorBGRA{22, 160, 133     }
		, ::ced::SColorBGRA{19, 141, 117     }
		, ::ced::SColorBGRA{17, 122, 101     }
		, ::ced::SColorBGRA{14, 102, 85      }
		, ::ced::SColorBGRA{11, 83, 69       }
		, ::ced::SColorBGRA{233, 247, 239    }
		, ::ced::SColorBGRA{212, 239, 223    }
		, ::ced::SColorBGRA{169, 223, 191    }
		, ::ced::SColorBGRA{125, 206, 160    }
		, ::ced::SColorBGRA{82, 190, 128     }
		, ::ced::SColorBGRA{39, 174, 96      }
		, ::ced::SColorBGRA{34, 153, 84      }
		, ::ced::SColorBGRA{30, 132, 73      }
		, ::ced::SColorBGRA{25, 111, 61      }
		, ::ced::SColorBGRA{20, 90, 50       }
		, ::ced::SColorBGRA{234, 250, 241    }
		, ::ced::SColorBGRA{213, 245, 227    }
		, ::ced::SColorBGRA{171, 235, 198    }
		, ::ced::SColorBGRA{130, 224, 170    }
		, ::ced::SColorBGRA{88, 214, 141     }
		, ::ced::SColorBGRA{46, 204, 113     }
		, ::ced::SColorBGRA{40, 180, 99      }
		, ::ced::SColorBGRA{35, 155, 86      }
		, ::ced::SColorBGRA{29, 131, 72      }
		, ::ced::SColorBGRA{24, 106, 59      }
		, ::ced::SColorBGRA{254, 249, 231    }
		, ::ced::SColorBGRA{252, 243, 207    }
		, ::ced::SColorBGRA{249, 231, 159    }
		, ::ced::SColorBGRA{247, 220, 111    }
		, ::ced::SColorBGRA{244, 208, 63     }
		, ::ced::SColorBGRA{241, 196, 15     }
		, ::ced::SColorBGRA{212, 172, 13     }
		, ::ced::SColorBGRA{183, 149, 11     }
		, ::ced::SColorBGRA{154, 125, 10     }
		, ::ced::SColorBGRA{125, 102, 8      }
		, ::ced::SColorBGRA{254, 245, 231    }
		, ::ced::SColorBGRA{253, 235, 208    }
		, ::ced::SColorBGRA{250, 215, 160    }
		, ::ced::SColorBGRA{248, 196, 113    }
		, ::ced::SColorBGRA{245, 176, 65     }
		, ::ced::SColorBGRA{243, 156, 18     }
		, ::ced::SColorBGRA{214, 137, 16     }
		, ::ced::SColorBGRA{185, 119, 14     }
		, ::ced::SColorBGRA{156, 100, 12     }
		, ::ced::SColorBGRA{126, 81, 9       }
		, ::ced::SColorBGRA{253, 242, 233    }
		, ::ced::SColorBGRA{250, 229, 211    }
		, ::ced::SColorBGRA{245, 203, 167    }
		, ::ced::SColorBGRA{240, 178, 122    }
		, ::ced::SColorBGRA{235, 152, 78     }
		, ::ced::SColorBGRA{230, 126, 34     }
		, ::ced::SColorBGRA{202, 111, 30     }
		, ::ced::SColorBGRA{175, 96, 26      }
		, ::ced::SColorBGRA{147, 81, 22      }
		, ::ced::SColorBGRA{120, 66, 18      }
		, ::ced::SColorBGRA{251, 238, 230    }
		, ::ced::SColorBGRA{246, 221, 204    }
		, ::ced::SColorBGRA{237, 187, 153    }
		, ::ced::SColorBGRA{229, 152, 102    }
		, ::ced::SColorBGRA{220, 118, 51     }
		, ::ced::SColorBGRA{211, 84, 0       }
		, ::ced::SColorBGRA{186, 74, 0       }
		, ::ced::SColorBGRA{160, 64, 0       }
		, ::ced::SColorBGRA{135, 54, 0       }
		, ::ced::SColorBGRA{110, 44, 0       }
		, ::ced::SColorBGRA{253, 254, 254    }
		, ::ced::SColorBGRA{251, 252, 252    }
		, ::ced::SColorBGRA{247, 249, 249    }
		, ::ced::SColorBGRA{244, 246, 247    }
		, ::ced::SColorBGRA{240, 243, 244    }
		, ::ced::SColorBGRA{236, 240, 241    }
		, ::ced::SColorBGRA{208, 211, 212    }
		, ::ced::SColorBGRA{179, 182, 183    }
		, ::ced::SColorBGRA{151, 154, 154    }
		, ::ced::SColorBGRA{123, 125, 125    }
		, ::ced::SColorBGRA{248, 249, 249    }
		, ::ced::SColorBGRA{242, 243, 244    }
		, ::ced::SColorBGRA{229, 231, 233    }
		, ::ced::SColorBGRA{215, 219, 221    }
		, ::ced::SColorBGRA{202, 207, 210    }
		, ::ced::SColorBGRA{189, 195, 199    }
		, ::ced::SColorBGRA{166, 172, 175    }
		, ::ced::SColorBGRA{144, 148, 151    }
		, ::ced::SColorBGRA{121, 125, 127    }
		, ::ced::SColorBGRA{98, 101, 103     }
		, ::ced::SColorBGRA{244, 246, 246    }
		, ::ced::SColorBGRA{234, 237, 237    }
		, ::ced::SColorBGRA{213, 219, 219    }
		, ::ced::SColorBGRA{191, 201, 202    }
		, ::ced::SColorBGRA{170, 183, 184    }
		, ::ced::SColorBGRA{149, 165, 166    }
		, ::ced::SColorBGRA{131, 145, 146    }
		, ::ced::SColorBGRA{113, 125, 126    }
		, ::ced::SColorBGRA{95, 106, 106     }
		, ::ced::SColorBGRA{77, 86, 86       }
		, ::ced::SColorBGRA{242, 244, 244    }
		, ::ced::SColorBGRA{229, 232, 232    }
		, ::ced::SColorBGRA{204, 209, 209    }
		, ::ced::SColorBGRA{178, 186, 187    }
		, ::ced::SColorBGRA{153, 163, 164    }
		, ::ced::SColorBGRA{127, 140, 141    }
		, ::ced::SColorBGRA{112, 123, 124    }
		, ::ced::SColorBGRA{97, 106, 107     }
		, ::ced::SColorBGRA{81, 90, 90       }
		, ::ced::SColorBGRA{66, 73, 73       }
		, ::ced::SColorBGRA{235, 237, 239    }
		, ::ced::SColorBGRA{214, 219, 223    }
		, ::ced::SColorBGRA{174, 182, 191    }
		, ::ced::SColorBGRA{133, 146, 158    }
		, ::ced::SColorBGRA{93, 109, 126     }
		, ::ced::SColorBGRA{52, 73, 94       }
		, ::ced::SColorBGRA{46, 64, 83       }
		, ::ced::SColorBGRA{40, 55, 71       }
		, ::ced::SColorBGRA{33, 47, 60       }
		, ::ced::SColorBGRA{27, 38, 49       }
		, ::ced::SColorBGRA{234, 236, 238    }
		, ::ced::SColorBGRA{213, 216, 220    }
		, ::ced::SColorBGRA{171, 178, 185    }
		, ::ced::SColorBGRA{128, 139, 150    }
		, ::ced::SColorBGRA{86, 101, 115     }
		, ::ced::SColorBGRA{44, 62, 80       }
		, ::ced::SColorBGRA{39, 55, 70       }
		, ::ced::SColorBGRA{33, 47, 61       }
		, ::ced::SColorBGRA{28, 40, 51       }
		, ::ced::SColorBGRA{23, 32, 42       }
		};
#pragma pack( pop )
} // namespace

#endif // CED_COLOR_H_23904872398475
