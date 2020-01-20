#include "ced_view.h"

#ifndef CED_DEMO_08_GAME_H_293874239874
#define CED_DEMO_08_GAME_H_293874239874

struct SStars	{
	::ced::container<::ced::SCoord2<float>>		Position			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};
};

struct SDebris	{
	::ced::container<::ced::SCoord3<float>>		Position			= {};
	::ced::container<::ced::SCoord3<float>>		Direction			= {};
	::ced::container<float>						Speed				= {};
	::ced::container<float>						Brightness			= {};

	int											Spawn				(const ::ced::SCoord3<float> & position, const ::ced::SCoord3<float> & direction, float speed)	{
		Position		.push_back(position);
		Direction		.push_back(direction);
		Speed			.push_back(speed);
		Brightness		.push_back(1);
		return 0;
	}
	int											Update				(float lastFrameSeconds)	{
		for(uint32_t iShot = 0; iShot < Position.size(); ++iShot) {
			Position	[iShot]							+= Direction[iShot] * (Speed[iShot] * lastFrameSeconds);
			Brightness 	[iShot]							-= lastFrameSeconds;
			if(Position[iShot].Length() > 50) {
				Direction	[iShot]							= Direction	[Position.size() - 1];
				Position	[iShot]							= Position	[Position.size() - 1];
				Speed		[iShot]							= Speed		[Position.size() - 1];
				Brightness	[iShot]							= Brightness[Position.size() - 1];
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
			Position	[iShot]							+= Direction[iShot] * (Speed[iShot] * lastFrameSeconds);
			Brightness 	[iShot]							-= lastFrameSeconds ;
			if (Position[iShot].Length() > 100)
				Remove(iShot);
		}
		return 0;
	}
	int											Remove			(uint32_t iShot) {
		PositionPrev	[iShot]						= PositionPrev	[Position.size() - 1];
		Position		[iShot]						= Position		[Position.size() - 1];
		Speed			[iShot]						= Speed			[Position.size() - 1];
		Brightness		[iShot]						= Brightness	[Position.size() - 1];
		PositionPrev	.resize(PositionPrev	.size() - 1);
		Position		.resize(Position		.size() - 1);
		Speed			.resize(Speed			.size() - 1);
		Brightness		.resize(Brightness		.size() - 1);
		return Position.size();
	}
};

#endif // CED_DEMO_08_GAME_H_293874239874
