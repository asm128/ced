#include "ced_view.h"
#include "ced_color.h"

#ifndef CED_DEMO_08_GAME_H_293874239874
#define CED_DEMO_08_GAME_H_293874239874

struct SStars	{
	::ced::container<::ced::SCoord2<float>>		Position			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};

	int											Update				(uint32_t yMax, float lastFrameSeconds)	{
		for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
			::ced::SCoord2<float>							& starPos			= Position[iStar];
			float											& starSpeed			= Speed[iStar];
			starPos.y									+= starSpeed * lastFrameSeconds;
			Brightness[iStar]							= float(1.0 / RAND_MAX * rand());
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

	int											IntegrateSpeed		(double lastFrameSeconds)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			::ced::SCoord3<float>							& direction			= Direction	[iShot];
			::ced::SCoord3<float>							& position			= Position	[iShot];
			float											& speed				= Speed		[iShot];
			position									+= direction * speed * lastFrameSeconds;
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
		Particles	.Spawn(position, direction, speed);
		Brightness	.push_back(brightness);
		return 0;
	}
	int											SpawnSpherical		(uint32_t countDebris, const ::ced::SCoord3<float> & position, float speedDebris, float brightness)	{
		for(uint32_t iDebris = 0; iDebris < countDebris; ++iDebris) {
			::ced::SCoord3<float>									direction				= {0, 1, 0};
			direction.RotateX(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.RotateY(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.RotateZ(rand() * (::ced::MATH_2PI / RAND_MAX));
			direction.Normalize();
			Spawn(position, direction, speedDebris, brightness);
		}
		return 0;
	}
	int											Update				(float lastFrameSeconds)	{
		Particles.IntegrateSpeed(lastFrameSeconds);
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			float											& speed				= Particles.Speed		[iShot];
			float											& brightness 		= Brightness			[iShot];
			brightness									-= lastFrameSeconds;
			speed										-= lastFrameSeconds * (rand() % 16);
			if(brightness < 0) {
				Particles.Remove(iShot);
				Brightness.remove_unordered(iShot--);
			}
		}
		return 0;
	}
};

struct SShots	{
	double										Delay				= 0;
	::ced::container<float>						Brightness			= {};
	::ced::container<::ced::SCoord3<float>>		PositionPrev		= {};
	::SParticles3								Particles;

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed, float brightness)	{
		if(Delay < 1)
			return 0;
		Delay										= 0;
		Particles	.Spawn(position, direction, speed);
		Brightness	.push_back(brightness);
		PositionPrev.push_back(position);
		return 0;
	}

	int											Update				(float lastFrameSeconds)	{
		memcpy(PositionPrev.begin(), Particles.Position.begin(), Particles.Position.size() * sizeof(::ced::SCoord3<float>));
		Particles.IntegrateSpeed(lastFrameSeconds);
		for(uint32_t iShot = 0; iShot < Particles.Position.size(); ++iShot) {
			if (Particles.Position[iShot].LengthSquared() > (100 * 100))
				Remove(iShot--);
		}
		return 0;
	}
	int											Remove				(uint32_t iShot)			{
		Particles.Remove(iShot);
		PositionPrev	.remove_unordered(iShot);
		Brightness		.remove_unordered(iShot);
		return Brightness.size();
	}
};

struct SSlice {
	uint16_t									Offset;
	uint16_t									Count;
};

struct SExplosion {
	int32_t										IndexMesh;
	int32_t										IndexEntity;
	::ced::container<SSlice>					Slices;
	::SParticles3								Particles;

	int											Update				(float lastFrameSeconds)	{
		Particles.IntegrateSpeed(lastFrameSeconds);
		for(uint32_t iShot = 0; iShot < Particles.Speed.size(); ++iShot) {
			float											& speed				= Particles.Speed		[iShot];
			speed										-= lastFrameSeconds * (rand() % 16);
			if (speed < -10)
				Remove(iShot--);
		}
		return 0;
	}
	int											Remove				(uint32_t iSlice)			{
		Slices			.remove_unordered(iSlice);
		Particles		.Remove(iSlice);
		return Slices.size();
	}
};

#endif // CED_DEMO_08_GAME_H_293874239874
