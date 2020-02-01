#include "ced_view.h"
#include "ced_color.h"

#ifndef CED_DEMO_08_GAME_H_293874239874
#define CED_DEMO_08_GAME_H_293874239874

struct SStars	{
	::ced::container<::ced::SCoord2<float>>		Position			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};

	int											Update				(uint32_t yMax, float secondsLastFrame)	{
		static constexpr const double					randUnit			= 1.0 / RAND_MAX;
		for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
			::ced::SCoord2<float>							& starPos			= Position[iStar];
			float											& starSpeed			= Speed[iStar];
			starPos.y									+= starSpeed * secondsLastFrame;
			Brightness[iStar]							= float(randUnit * rand());
			if(starPos.y >= yMax) {
				starSpeed									= float(16 + (rand() % 64));
				starPos.y									= 0;
			}
		}
		return 0;
	}
};

struct SParticles3 {
	::ced::container<::ced::SCoord3<float>>		Position			= {};
	::ced::container<::ced::SCoord3<float>>		Direction			= {};
	::ced::container<float>						Speed				= {};

	int											IntegrateSpeed		(double secondsLastFrame)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			::ced::SCoord3<float>							& direction			= Direction	[iShot];
			::ced::SCoord3<float>							& position			= Position	[iShot];
			float											& speed				= Speed		[iShot];
			position									+= direction * speed * secondsLastFrame;
		}
		return 0;
	}
	int											Remove				(int32_t iParticle)	{
		Position	.remove_unordered(iParticle);
		Direction	.remove_unordered(iParticle);
		return Speed.remove_unordered(iParticle);
	}
	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed)	{
		Position	.push_back(position);
		Direction	.push_back(direction);
		return Speed.push_back(speed);
	}
};

struct SDebris	{
	::ced::SColorBGRA							Colors[4]			=
		{ {0x80, 0xAF, 0xFF, }
		, {0x40, 0x80, 0xFF, }
		, {0x20, 0x80, 0xFF, }
		, {0x00, 0x00, 0xFF, }
		};
	::ced::container<float>						Brightness			= {};
	::SParticles3								Particles			= {};

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed, float brightness)	{
		Particles.Spawn(position, direction, speed);
		return Brightness.push_back(brightness);
	}
	int											SpawnSpherical		(uint32_t countDebris, const ::ced::SCoord3<float> & position, float speedDebris, float brightness)	{
		static constexpr const double					randUnit			= ::ced::MATH_2PI / RAND_MAX;
		for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
			::ced::SCoord3<float>							direction			= {0, 1, 0};
			direction.RotateX(rand() * randUnit);
			direction.RotateY(rand() * randUnit);
			direction.Normalize();
			Spawn(position, direction, speedDebris, brightness);
		}
		return 0;
	}
	int											SpawnDirected		(uint32_t countDebris, const ::ced::SCoord3<float> & direction, const ::ced::SCoord3<float> & position, float speedDebris, float brightness)	{
		static constexpr const double					randUnit			= ::ced::MATH_2PI / RAND_MAX;
		for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
			::ced::SCoord3<float>							finalDirection	= {0, 1, 0};
			finalDirection.RotateX(rand() * randUnit);
			finalDirection.RotateY(rand() * randUnit);
			finalDirection.Normalize();
			Spawn(position, (direction * .7) + (finalDirection * .3), speedDebris, brightness);
		}
		return 0;
	}
	int											Update				(float secondsLastFrame)	{
		Particles.IntegrateSpeed(secondsLastFrame);
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			float											& speed				= Particles.Speed		[iShot];
			float											& brightness 		= Brightness			[iShot];
			brightness									-= secondsLastFrame;
			speed										-= secondsLastFrame * ((0 > speed) ? (rand() % 16) * 5 : (rand() % 16));

			if(0 > brightness) {
				Particles.Remove(iShot);
				Brightness.remove_unordered(iShot--);
			}
		}
		return 0;
	}
};

struct SShots	{
	double										Delay				= 0;
	int32_t										Damage				= 1;
	::ced::container<float>						Brightness			= {};
	::ced::container<::ced::SCoord3<float>>		PositionPrev		= {};
	::SParticles3								Particles;

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed, float brightness)	{
		if(Delay < 1)
			return 0;
		Delay										= 0;
		Particles.Spawn(position, direction, speed);
		PositionPrev.push_back(position);
		return Brightness.push_back(brightness);
	}

	int											Update				(float secondsLastFrame)	{
		static constexpr	const uint32_t				maxRange			= 200;
		static constexpr	const uint32_t				maxRangeSquared		= maxRange * maxRange;
		memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::ced::SCoord3<float>));
		Particles.IntegrateSpeed(secondsLastFrame);
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			if (Particles.Position[iShot].LengthSquared() > maxRangeSquared)
				Remove(iShot--);
		}
		return 0;
	}
	int											Remove				(uint32_t iShot)			{
		Particles.Remove(iShot);
		PositionPrev		.remove_unordered(iShot);
		return Brightness	.remove_unordered(iShot);
	}
};

struct SExplosion {
	int32_t										IndexMesh;
	::ced::container<::ced::SSlice<uint16_t>>	Slices;
	::SParticles3								Particles;

	int											Update				(float secondsLastFrame)	{
		Particles.IntegrateSpeed(secondsLastFrame);
		for(uint32_t iShot = 0; iShot < Particles.Speed.size(); ++iShot) {
			float											& speed				= Particles.Speed		[iShot];
			speed										-= secondsLastFrame * (rand() % 16);
			if (speed < -10)
				Remove(iShot--);
		}
		return 0;
	}
	int											Remove				(uint32_t iSlice)			{
		Particles.Remove(iSlice);
		return Slices.remove_unordered(iSlice);
	}
};

#endif // CED_DEMO_08_GAME_H_293874239874
