#include "ced_view.h"

#ifndef CED_DEMO_08_GAME_H_293874239874
#define CED_DEMO_08_GAME_H_293874239874

struct SStars	{
	::ced::container<::ced::SCoord2<float>>		Position			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};

	int											Update				(uint32_t yMax, float lastFrameSeconds)	{
		for(uint32_t iStar = 0; iStar < Brightness.size(); ++iStar) {
			::ced::SCoord2<float>							 & starPos			= Position[iStar];
			float											starSpeed			= Speed[iStar];
			starPos.y									+= starSpeed * lastFrameSeconds;
			Brightness[iStar]							= float(1.0 / 65535.0 * rand());
			if(starPos.y >= yMax) {
				Speed		[iStar]						= float(16 + (rand() % 64));
				starPos.y									= 0;
			}
		}
		return 0;
	}
};

struct SDebris	{
	::ced::container<::ced::SCoord3<float>>		Position			= {};
	::ced::container<::ced::SCoord3<float>>		Direction			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed, float brightness)	{
		Position		.push_back(position);
		Direction		.push_back(direction);
		Speed			.push_back(speed);
		Brightness		.push_back(brightness);
		return 0;
	}
	int											SpawnSpherical			(uint32_t countDebris, const ::ced::SCoord3<float> & position, float speedDebris, float brightness)	{
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
		static constexpr	const uint32_t				maxFar				= 50;
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			::ced::SCoord3<float>							& direction						= Direction	[iShot];
			::ced::SCoord3<float>							& position						= Position	[iShot];
			float											& speed							= Speed		[iShot];
			float											& brightness 					= Brightness[iShot];
			position									+= direction * (speed * (double)lastFrameSeconds);
			brightness									-= lastFrameSeconds;
			 speed										-= lastFrameSeconds *  (rand() % 16);
			if(position.Length() > maxFar || brightness < 0) {
				direction									= Direction	[Position.size() - 1];
				position									= Position	[Position.size() - 1];
				speed										= Speed		[Position.size() - 1];
				brightness									= Brightness[Position.size() - 1];
				Direction	.resize(Direction	.size() - 1);
				Position	.resize(Position	.size() - 1);
				Speed		.resize(Speed		.size() - 1);
				Brightness	.resize(Brightness	.size() - 1);
			}
		}
		return 0;
	}
};

struct SShots	{
	double										Delay				= 0;
	::ced::container<::ced::SCoord3<float>>		PositionPrev		= {};
	::ced::container<::ced::SCoord3<float>>		Position			= {};
	::ced::container<::ced::SCoord3<float>>		Direction			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed)	{
		if(Delay < 1)
			return 0;
		PositionPrev	.push_back(position);
		Position		.push_back(position);
		Direction		.push_back(direction);
		Speed			.push_back(speed);
		Brightness		.push_back(1);
		Delay										= 0;
		return 0;
	}

	int											Update				(float lastFrameSeconds)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			PositionPrev[iShot]							= Position	[iShot];
			Position	[iShot]							+= Direction[iShot] * (Speed[iShot] * (double)lastFrameSeconds);
			if (Position[iShot].Length() > 100)
				Remove(iShot);
		}
		return 0;
	}
	int											Remove			(uint32_t iShot) {
		Direction		[iShot]						= Direction		[Position.size() - 1];
		PositionPrev	[iShot]						= PositionPrev	[Position.size() - 1];
		Position		[iShot]						= Position		[Position.size() - 1];
		Speed			[iShot]						= Speed			[Position.size() - 1];
		Brightness		[iShot]						= Brightness	[Position.size() - 1];
		PositionPrev	.resize(PositionPrev	.size() - 1);
		Position		.resize(Position		.size() - 1);
		Speed			.resize(Speed			.size() - 1);
		Brightness		.resize(Brightness		.size() - 1);
		Direction		.resize(Direction		.size() - 1);
		return Position.size();
	}
};

#endif // CED_DEMO_08_GAME_H_293874239874
