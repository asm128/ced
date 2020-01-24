#define NOMINMAX
#include <cstdint>
#include <cmath>

#ifndef CED_MATH_H_2983492837498
#define CED_MATH_H_2983492837498

namespace ced
{
	struct			SPairSinCos			{ double Sin, Cos; };

	static inline	SPairSinCos			getSinCos				(double theta)																noexcept	{ return {sin(theta), cos(theta)};						}

	template<typename _tNumber>
	constexpr			_tNumber		interpolate_linear		(const _tNumber a, const _tNumber b, const double factor)					noexcept	{ return (_tNumber)(a * (1.0 - factor) + b * factor);	}

	static constexpr	const double	MATH_PI					= 3.141519;
	static constexpr	const double	MATH_2PI				= MATH_PI * 2;
	static constexpr	const double	MATH_PI_2				= MATH_PI * .5;

	template<typename _tValue>
	struct SCoord2 {
		_tValue							x;
		_tValue							y;

		SCoord2<_tValue>				operator+				(const SCoord2<_tValue>& other)	const noexcept	{ return {x + other.x, y + other.y}; }
		SCoord2<_tValue>				operator-				(const SCoord2<_tValue>& other)	const noexcept	{ return {x - other.x, y - other.y}; }
		SCoord2<_tValue>				operator/				(double scalar)					const			{ return {(_tValue)(x / scalar), (_tValue)(y / scalar)}; }
		SCoord2<_tValue>				operator*				(double scalar)					const noexcept	{ return {(_tValue)(x * scalar), (_tValue)(y * scalar)}; }

		SCoord2<_tValue>&				operator+=				(const SCoord2<_tValue>& other)	noexcept		{ x += other.x; y += other.y; return *this; }
		SCoord2<_tValue>&				operator-=				(const SCoord2<_tValue>& other)	noexcept		{ x -= other.x; y -= other.y; return *this; }
		SCoord2<_tValue>&				operator/=				(double scalar)									{ x = (_tValue)(x / scalar); y = (_tValue)(y / scalar); return *this; }
		SCoord2<_tValue>&				operator*=				(double scalar)					noexcept		{ x = (_tValue)(x * scalar); y = (_tValue)(y * scalar); return *this; }

		template<typename _tOther>
		SCoord2<_tOther>				Cast					()								const			{ return {(_tOther)x, (_tOther)y}; }

		// https://matthew-brett.github.io/teaching/rotation_2d.html
		SCoord2<_tValue>				Rotated					(double theta)									{
			double								dsin					= ::std::sin(theta);
			double								dcos					= ::std::cos(theta);
			return {(int)(dcos * x - dsin * y), (int)(dsin * x + dcos * y)};
		}

		double							Length					()								const noexcept	{
			_tValue								lengthSquared			= x * x + y * y;
			return (0 == lengthSquared) ? 0 : sqrt(lengthSquared);
		}
	};

	template<typename _tValue>
	struct SCoord3 {
		_tValue													x;
		_tValue													y;
		_tValue													z;

		typedef													SCoord3<_tValue>		TCoord3;

		SCoord3<_tValue>										operator+				(const SCoord3<_tValue>& other)		const noexcept	{ return {x + other.x, y + other.y, z + other.z}; }
		SCoord3<_tValue>										operator-				(const SCoord3<_tValue>& other)		const noexcept	{ return {x - other.x, y - other.y, z - other.z}; }
		SCoord3<_tValue>										operator/				(double scalar)						const			{ return {(_tValue)(x / scalar), (_tValue)(y / scalar), (_tValue)(z / scalar)}; }
		SCoord3<_tValue>										operator*				(double scalar)						const noexcept	{ return {(_tValue)(x * scalar), (_tValue)(y * scalar), (_tValue)(z * scalar)}; }

		SCoord3<_tValue>&										operator+=				(const SCoord3<_tValue>& other)		noexcept		{ x += other.x; y += other.y; z += other.z; return *this; }
		SCoord3<_tValue>&										operator-=				(const SCoord3<_tValue>& other)		noexcept		{ x -= other.x; y -= other.y; z -= other.z; return *this; }
		SCoord3<_tValue>&										operator/=				(double scalar)										{ x = (_tValue)(x / scalar); y = (_tValue)(y / scalar); z = (_tValue)(z / scalar); return *this; }
		SCoord3<_tValue>&										operator*=				(double scalar)						noexcept		{ x = (_tValue)(x * scalar); y = (_tValue)(y * scalar); z = (_tValue)(z * scalar); return *this; }

		template<typename _tOther>
										SCoord3<_tOther>		Cast					()									const			{ return {(_tOther)x, (_tOther)y, (_tOther)z}; }

		inline							TCoord3&				Normalize				()																						{ const _tValue sqLen = LengthSquared(); return sqLen ? *this /= ::sqrt(sqLen) : *this;										}
		constexpr						double					Dot						(const TCoord3& other)												const	noexcept	{ return x * other.x + y * other.y + z * other.z;																			}
		constexpr						_tValue					LengthSquared			()																	const	noexcept	{ return x * x + y * y + z * z;																								}
		constexpr						double					Length					()																	const				{ const _tValue sqLen = LengthSquared(); return sqLen ? ::sqrt(sqLen) : 0;													}
		constexpr						double					AngleWith				(const TCoord3& other)												const				{ const double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos(Dot(other) / lengthsProduct) : 0;	}
										void					AddScaled				(const TCoord3& vectorToScaleAndAdd, float scale)										{
			x														+= vectorToScaleAndAdd.x * scale;
			y														+= vectorToScaleAndAdd.y * scale;
			z														+= vectorToScaleAndAdd.z * scale;
		}

		constexpr						TCoord3					Cross					(const TCoord3& right)												const	noexcept	{ return {y * right.z - z * right.y, z * right.x - x * right.z, x * right.y - y * right.x };	}
										TCoord3&				Cross					(const TCoord3& vector1, const TCoord3& vector2)							noexcept	{
			x														= vector1.y * vector2.z - vector1.z * vector2.y;
			y														= vector1.z * vector2.x - vector1.x * vector2.z;
			z														= vector1.x * vector2.y - vector1.y * vector2.x;
			return *this;
		}
										TCoord3&				CrossAndNormalize		(const TCoord3& in_vLeft, const TCoord3& in_vRight)										{
			x														= in_vLeft->y * in_vRight->z - in_vLeft->z * in_vRight->y;
			y														= in_vLeft->z * in_vRight->x - in_vLeft->x * in_vRight->z;
			z														= in_vLeft->x * in_vRight->y - in_vLeft->y * in_vRight->x;
			Normalize();
			return *this;
		};
										TCoord3&				RotateX					(double theta)																			{
			const ::ced::SPairSinCos									pairSinCos				= ::ced::getSinCos(theta);
			const double												pz						= y * pairSinCos.Cos - z * pairSinCos.Sin;
			y														= (_tValue)(y * pairSinCos.Sin + z * pairSinCos.Cos);
			z														= (_tValue)pz;
			return *this;
		}

										TCoord3&				RotateY					(double theta)																			{
			const ::ced::SPairSinCos									pairSinCos				= ::ced::getSinCos(theta);
			const double												px						= x * pairSinCos.Cos - z * pairSinCos.Sin;
			z														= (_tValue)(x * pairSinCos.Sin + z * pairSinCos.Cos);
			x														= (_tValue)px;
			return *this;
		}

										TCoord3&				RotateZ					(double theta)																			{
			const ::ced::SPairSinCos									pairSinCos				= ::ced::getSinCos(theta);
			const double												px						= x * pairSinCos.Cos - y * pairSinCos.Sin;
			y														= (_tValue)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x														= (_tValue)px;
			return *this;
		}
	};

	template<typename _tBase>
	struct SQuaternion {
		typedef				SQuaternion	<_tBase>	TQuat;
		typedef				SCoord3		<_tBase>	TCoord3;
							_tBase					x, y, z, w;
		//
		constexpr			bool			operator ==				(const TQuat& other)										const	noexcept	{ return x == other.x && y == other.y && z == other.z && w == other.w; }
		constexpr inline	bool			operator !=				(const TQuat& other)										const	noexcept	{ return !operator==(other); }
		//
		constexpr			TQuat			operator +				(const TQuat& other)										const	noexcept	{ return { x + other.x, y + other.y, z + other.z, w + other.w }; }
		constexpr			TQuat			operator -				(const TQuat& other)										const	noexcept	{ return { x - other.x, y - other.y, z - other.z, w - other.w }; }
		constexpr			TQuat			operator *				(double scalar)												const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		constexpr			TQuat			operator /				(double scalar)												const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }

							TQuat			operator *				(const TQuat& q)											const	noexcept	{
								TQuat			r;
								r.x			= w*q.x + x*q.w + y*q.z - z*q.y;
								r.y			= w*q.y + y*q.w + z*q.x - x*q.z;
								r.z			= w*q.z + z*q.w + x*q.y - y*q.x;
								r.w			= w*q.w - x*q.x - y*q.y - z*q.z;

								return r;
							}
							TQuat			operator *				(const TCoord3& v)											const	noexcept	{
								return
									{	  w*v.x + y*v.z - z*v.y
									,	  w*v.y + z*v.x - x*v.z
									,	  w*v.z + x*v.y - y*v.x
									,	-(x*v.x + y*v.y + z*v.z)
									};
							}
							TQuat&			operator+=				(const TQuat& other)												noexcept	{ x += other.x; y += other.y; z += other.z; w += other.w; return *this;													}
							TQuat&			operator-=				(const TQuat& other)												noexcept	{ x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this;													}
							TQuat&			operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this;	}
							TQuat&			operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this;	}
		inline				TQuat&			operator*=				(const TQuat& q)													noexcept	{ return *this = operator*(q); }
		// Unary operators
		constexpr inline	TQuat			operator-				()															const	noexcept	{ return {x*-1, y*-1, z*-1, w*-1};																						}
		constexpr inline	TQuat			operator~				()															const	noexcept	{ return {-x, -y, -z, w};																								}
		template<typename _t>
		constexpr inline	SQuaternion<_t>	Cast					()															const	noexcept	{ return {(_t)x, (_t)y, (_t)z, (_t)w};																					}
		inline				void			Identity				()																	noexcept	{ x = y = z = 0.0f; w = 1.0f;																							}
		inline				TQuat&			Normalize				()																	noexcept	{ _tBase sqLen = LengthSquared(); if(sqLen) return *this /= ::sqrt(sqLen); Identity(); return *this;		}
		constexpr			double			Dot						(const TQuat& other)										const	noexcept	{ return x*other.x + y*other.y + z*other.z + w*other.w;																	}
		constexpr inline	_tBase			LengthSquared			()															const	noexcept	{ return x * x + y * y + z * z + w * w;																					}
		inline constexpr	double			Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;												}
		inline				TQuat&			LinearInterpolate		(const TQuat &p, const TQuat &q, double fTime)						noexcept	{ return *this = ((q-p)*fTime)+p;																						}
							//void			AddScaled				(const TCoord4& vector, double scale)								noexcept	{ TQuat q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
							void			AddScaled				(const TCoord3& vector, double scale)								noexcept	{ TQuat q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
		inline				void			SetRotation				(const TQuat& q1, const TQuat& q2)									noexcept	{ return *this = q1 * q2 * ~q1;																							}
							TCoord3			RotateVector			(const TCoord3 &v)											const	noexcept	{
			const TQuat								t						= {x, y, z, w};
			const TQuat								r						= t * v * (~t);
			return {r.x, r.y, r.z};
		}
							TQuat&			SLERP					(const TQuat& p, const TQuat& q, double fTime)									{
			//Calculate the dot product
			double									fDot					= Dot(q);

			TQuat									q_;
			if(fDot < 0.0f) {
				q_									= -q;
				fDot								= -fDot;
			}

			if(fDot < 1.00001f && fDot > 0.99999f)
				return ::ced::interpolate_linear(p, q_, fTime);

			//calculate the angle between the quaternions
			const	double							fTheta					= acos(fDot);
			return *this						= (p * sin(fTheta * (1 - fTime)) + q_ * sin(fTheta * fTime)) / sin(fTheta);
		}
		// Convert from Euler Angles
		inline				TQuat&			MakeFromEulerTaitBryan	(const TCoord3& v)																{ return MakeFromEulerTaitBryan(v.x, v.y, v.z);																			}
							TQuat&			MakeFromEulerTaitBryan	(double fPitch, double fYaw, double fRoll)										{
			 //Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll and multiply those together. the calculation below does the same, just shorter.
			fPitch								*= 0.5f;//
			fYaw								*= 0.5f;//
			fRoll								*= 0.5f;//

			const SPairSinCos						pairSinCosX				= ::ced::getSinCos(fPitch	);
			const SPairSinCos						pairSinCosY				= ::ced::getSinCos(fYaw	);
			const SPairSinCos						pairSinCosZ				= ::ced::getSinCos(fRoll	);

			double									cYcZ					= pairSinCosY.Cos * pairSinCosZ.Cos;
			double									sYsZ					= pairSinCosY.Sin * pairSinCosZ.Sin;
			double									cYsZ					= pairSinCosY.Cos * pairSinCosZ.Sin;
			double									sYcZ					= pairSinCosY.Sin * pairSinCosZ.Cos;

			w									= (_tBase)(pairSinCosX.Cos * cYcZ + pairSinCosX.Sin * sYsZ);
			x									= (_tBase)(pairSinCosX.Sin * cYcZ - pairSinCosX.Cos * sYsZ);
			y									= (_tBase)(pairSinCosX.Cos * sYcZ + pairSinCosX.Sin * cYsZ);
			z									= (_tBase)(pairSinCosX.Cos * cYsZ - pairSinCosX.Sin * sYcZ);

			return Normalize();
		}
							void			GetEulersTaitBryan		(double* fPitch, double* fYaw, double* fRoll)									{
			double									r11, r21, r31, r32, r33, r12, r13;
			double									q00, q11, q22, q33;
			double									tmp;

			q00									= w * w;
			q11									= x * x;
			q22									= y * y;
			q33									= z * z;

			r11									= q00 + q11 - q22 - q33;
			r21									= 2 * (x*y + w*z);
			r31									= 2 * (x*z - w*y);
			r32									= 2 * (y*z + w*x);
			r33									= q00 - q11 - q22 + q33;

			tmp									= abs(r31);
			if(tmp > 0.999999) {
				r12									= 2 * (x*y - w*z);
				r13									= 2 * (x*z + w*y);
				*fPitch								= 0.0f;
				*fYaw								= -((::ced::MATH_PI_2) * r31/tmp);
				*fRoll								= atan2(-r12, -r31*r13);
			}
			else {
				*fPitch								 = atan2(r32, r33);
				*fYaw								 = asin(-r31);
				*fRoll								 = atan2(r21, r11);
			}
		} //
	}; // struct SQuaternion

	template<typename _tValue>
	struct SRectangle {
		SCoord2<_tValue>					Position	;
		SCoord2<_tValue>					Size		;
	};

	template<typename _tValue>
	struct SCircle {
		SCoord2<_tValue>					Position	;
		double								Radius		;
	};

	template<typename _tValue>
	struct SLine {
		SCoord2<_tValue>					A;
		SCoord2<_tValue>					B;
	};

	template<typename _tValue>
	struct STriangle {
		SCoord2<_tValue>					A;
		SCoord2<_tValue>					B;
		SCoord2<_tValue>					C;
	};

	template<typename _tValue>
	struct STriangleWeights {
		_tValue								A;
		_tValue								B;
		_tValue								C;
	};

	template<typename _tValue>
	struct SRectangle3 {
		SCoord3<_tValue>					Position	;
		SCoord3<_tValue>					Size		;
	};

	template<typename _tValue>
	struct SCircle3 {
		SCoord3<_tValue>					Position	;
		double								Radius		;
	};

	template<typename _tValue>
	struct SLine3 {
		SCoord3<_tValue>					A;
		SCoord3<_tValue>					B;
	};

	template<typename _tValue>
	struct STriangle3 {
		SCoord3<_tValue>					A;
		SCoord3<_tValue>					B;
		SCoord3<_tValue>					C;

		template<typename _tOther>
		STriangle3<_tOther>					Cast					()								const	noexcept		{
			return
				{ A.Cast<_tOther>()
				, B.Cast<_tOther>()
				, C.Cast<_tOther>()
				};
		}

	};
} // namespace

#endif // CED_MATH_H_2983492837498
