#include "ced_view.h"

#ifndef CED_PARTICLES_H_203984290384
#define CED_PARTICLES_H_203984290384

namespace ced
{
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
} // namespace

#endif // CED_PARTICLES_H_203984290384
