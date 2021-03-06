#include "ced_demo_13_game.h"
#include "ced_draw.h"

#include <algorithm>

static constexpr	const uint32_t					MAX_LIGHT_RANGE		= 10;

static	int											drawStars			(const ::ssg::SStars & stars, ::ced::view_grid<::ced::SColorBGRA> targetPixels)	{
	::ced::SColorBGRA										colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0xC0, 0xC0, 0xfF, }
		, {0xfF, 0xC0, 0xC0, }
		, {0xC0, 0xD0, 0xC0, }
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::ced::SCoord2<float>									starPos				= stars.Position[iStar];
		::ced::SColorBGRA										starFinalColor		= colors[iStar % ::std::size(colors)] * stars.Brightness[iStar];
		::ced::setPixel(targetPixels, starPos.Cast<int32_t>(), starFinalColor);
		const	int32_t											brightRadius		= 1 + (iStar % 3) + (rand() % 2);
		const	double											brightRadiusSquared	= brightRadius * (double)brightRadius;
		double													brightUnit			= 1.0 / brightRadiusSquared;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::ced::SCoord2<int32_t>									pixelPos			= (starPos + brightPos).Cast<int32_t>();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
 				)
					::ced::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}

static	int											drawDebris
	( ::ced::view_grid<::ced::SColorBGRA>		targetPixels
	, const ::ssg::SDebris						& debris
	, const ::ced::SMatrix4<float>				& matrixVPV
	, ::ced::view_grid<uint32_t>				depthBuffer
	)	{
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		const ::ced::SColorFloat								& colorShot			= debris.Colors[iParticle % ::std::size(debris.Colors)];
		::ced::SCoord3<float>									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		const ::ced::SCoord2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::ced::SColorFloat										starFinalColor	= colorShot * debris.Brightness[iParticle];
		starFinalColor.g									= ::std::max(0.0f, starFinalColor.g - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b									= ::std::max(0.0f, starFinalColor.b - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		//::ced::setPixel(targetPixels, pixelCoord, starFinalColor);
		static constexpr	const double						brightRadius		= 1.5;
		static constexpr	const double						brightRadiusSquared	= brightRadius * brightRadius;
		static constexpr	const double						brightUnit			= 1.0 / brightRadiusSquared;

		for(int32_t y = (int32_t)-brightRadius - 1; y < (int32_t)brightRadius + 1; ++y)
		for(int32_t x = (int32_t)-brightRadius - 1; x < (int32_t)brightRadius + 1; ++x) {
			::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.LengthSquared();
			if(brightDistance <= brightRadiusSquared) {
				::ced::SCoord2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
				 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
				)
					continue;
				uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
				if(depth > blendVal)
					continue;
				blendVal											= depth;
				double													finalBrightness					= 1.0-(brightDistance * brightUnit);
				::ced::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
				pixelVal											= starFinalColor * finalBrightness + pixelVal;
			}
		}
	}
	return 0;
}

static	int											drawShots			(::ced::view_grid<::ced::SColorBGRA> targetPixels, const ::ssg::SShots & shots
	, const ::ced::SMatrix4<float>				& matrixVPV
	, ::ced::SColorFloat						colorShot
	, const	double								brightRadius
	, const	double								intensity
	, const	bool								line
	, ::ced::view_grid<uint32_t>				depthBuffer
	, ::ced::container<::ced::SCoord3<float>>	pixelCoordsCache
	) {
	const ::ced::SCoord2<int32_t>						targetMetrics			= targetPixels.metrics().Cast<int32_t>();
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		float													brightness			= shots.Brightness[iShot];
		pixelCoordsCache.clear();
		const ::ced::SCoord3<float>								& starPosPrev		= shots.PositionDraw[iShot];
		const ::ced::SCoord3<float>								& starPos			= shots.Particles.Position[iShot];
		//::ced::SLine3<float>									raySegment			= {starPosPrev, starPos};

		::ced::SLine3<float>									raySegment			= {starPos, starPosPrev}; //raySegmentWorld;
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);
		if(raySegment.A.z < 0 || raySegment.A.z > 1) continue;
		if(raySegment.B.z < 0 || raySegment.B.z > 1) continue;
		if(line)
			::ced::drawLine(targetPixels, raySegment, pixelCoordsCache, depthBuffer);
		else
			pixelCoordsCache.push_back(raySegment.B);
		const double											pixelCoordUnit		= 1.0 / pixelCoordsCache.size();
		for(uint32_t iPixelCoord = 0, countPixelCoords = pixelCoordsCache.size(); iPixelCoord < countPixelCoords; ++iPixelCoord) {
			const ::ced::SCoord3<float>							& pixelCoord		= pixelCoordsCache[iPixelCoord];
			if(pixelCoord.z < 0 || pixelCoord.z > 1)
				continue;
			if( pixelCoord.y < 0 || pixelCoord.y >= targetMetrics.y
			 || pixelCoord.x < 0 || pixelCoord.x >= targetMetrics.x
			)
				continue;
			targetPixels[(uint32_t)pixelCoord.y][(uint32_t)pixelCoord.x]	= colorShot;
			const uint32_t											depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			const	double											brightRadiusSquared	= brightRadius * brightRadius;
			double													brightUnit			= 1.0 / brightRadiusSquared * brightness;
			for(int32_t y = (int32_t)-brightRadius, brightCount = (int32_t)brightRadius; y < brightCount; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < brightCount; ++x) {
				::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::ced::SCoord2<int32_t>									blendPos			= ::ced::SCoord2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).Cast<int32_t>();
					if( blendPos.y < 0 || blendPos.y >= targetMetrics.y
					 || blendPos.x < 0 || blendPos.x >= targetMetrics.x
					)
						continue;
					uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
					if(depth > blendVal)
						continue;
					blendVal											= depth;
					::ced::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
					double													finalBrightness
						= line
						? (		 (brightDistance * brightUnit)) * (pixelCoordUnit * (countPixelCoords - 1 - iPixelCoord))
						: (1.0 - (brightDistance * brightUnit));
					::ced::SColorFloat										pixelColor						= ::ced::interpolate_linear(::ced::SColorFloat{pixelVal}, colorShot, finalBrightness * intensity);
					pixelVal											= pixelColor;
				}
			}
		}

	}
	return 0;
}

static	int											getLightArrays
	( const ::ssg::SSolarSystem					& solarSystem
	, ::ced::container<::ced::SCoord3<float>>	& lightPoints
	, ::ced::container<::ced::SColorBGRA>		& lightColors
	)						{
	::ced::SColorBGRA										colorLightPlayer		= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF};
	::ced::SColorBGRA										colorLightEnemy			= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF};
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		const ::ssg::SShip										& ship					= solarSystem.Ships[iShip];
		lightPoints.push_back(solarSystem.ShipPhysics.Transforms[solarSystem.Entities[ship.Entity].Body].Position);
		lightColors.push_back((0 == solarSystem.Ships[iShip].Team) ? colorLightPlayer : colorLightEnemy);
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			const ::ssg::SShipPart									& shipPart				= ship.Parts[iPart];
			const ::ced::SColorFloat								colorShot
				= (::ssg::MUNITION_TYPE_RAY		== ship.Parts[iPart].Type) ? ::ced::SColorFloat{1.0f, 0.1f, 0.0f}
				: (::ssg::MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? ship.Team ? ::ced::SColorFloat{1.0f, 0.125f, 0.25f} : ::ced::TURQUOISE
				: (::ssg::MUNITION_TYPE_BULLET	== ship.Parts[iPart].Type) ? ::ced::GRAY
				: ::ced::SColorFloat{::ced::SColorBGRA{0xFF, 0xFF, 0xFF}}
				;
			for(uint32_t iShot = 0; iShot < shipPart.Shots.Particles.Position.size(); ++iShot) {
				lightPoints.push_back(shipPart.Shots.Particles.Position[iShot]);
				lightColors.push_back(colorShot);
			}
		}
	}
	for(uint32_t iParticle = 0; iParticle < solarSystem.Debris.Particles.Position.size(); ++iParticle) {
		lightPoints.push_back(solarSystem.Debris.Particles.Position[iParticle]);
		::ced::SColorFloat										colorShot			= solarSystem.Debris.Colors[iParticle % ::std::size(solarSystem.Debris.Colors)];
		lightColors.push_back(colorShot * solarSystem.Debris.Brightness[iParticle]);
	}
	return 0;
}

static	int											getLightArrays
	( const ::ced::SCoord3<float>							& modelPosition
	, const ::ced::container<::ced::SCoord3<float>>			& lightPointsWorld
	, const ::ced::container<::ced::SColorBGRA>				& lightColorsWorld
	, ::ced::container<::ced::SCoord3<float>>				& lightPointsModel
	, ::ced::container<::ced::SColorBGRA>					& lightColorsModel
	) {
	lightPointsModel.clear();
	lightColorsModel.clear();
	for(uint32_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
		const ::ced::SCoord3<float>								& lightPoint		=	lightPointsWorld[iLightPoint];
		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE)) {
			lightPointsModel.push_back(lightPoint);
			lightColorsModel.push_back(lightColorsWorld[iLightPoint]);
		}
	}
	return 0;
}
//
//static	int											getLightArrays
//	( const ::ced::SCoord3<float>							& modelPosition
//	, const ::ced::container<::ced::SCoord3<float>>			& lightPointsWorld
//	, ::ced::container<uint16_t>							& indicesPointLights
//	) {
//	indicesPointLights.clear();
//	for(uint32_t iLightPoint = 0; iLightPoint < lightPointsWorld.size(); ++iLightPoint) {
//		const ::ced::SCoord3<float>								& lightPoint		=	lightPointsWorld[iLightPoint];
//		if((lightPoint - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE))
//			indicesPointLights.push_back((uint16_t)iLightPoint);
//	}
//	return 0;
//}

static	int											drawShip
	( const ::ssg::SSolarSystem							& solarSystem
	, const ::ssg::SShip								& ship
	, const ::ced::SMatrix4<float>						& matrixView
	, ::ced::view_grid<::ced::SColorBGRA>				& targetPixels
	, ::ced::view_grid<uint32_t>						depthBuffer
	, ::ssg::SSolarSystemDrawCache						& drawCache
	) {
	for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
		const ::ssg::SShipPart									& shipPart				= ship.Parts[iPart];
		if(shipPart.Health <= 0)
			continue;
		const ::ssg::SEntity									& entity					= solarSystem.Entities[shipPart.Entity];
		for(uint32_t iEntity = 0; iEntity < entity.Children.size(); ++iEntity) {
			const ::ssg::SEntity									& entityChild				= solarSystem.Entities[entity.Children[iEntity]];
			if(-1 == entityChild.Parent)
				continue;
			if(-1 == entityChild.Geometry)
				continue;
			::ced::SMatrix4<float>									matrixTransform				= solarSystem.Scene.ModelMatricesGlobal[entityChild.Transform];
			::ced::SMatrix4<float>									matrixTransformView			= matrixTransform * matrixView;
			::getLightArrays(matrixTransform.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
			const ::ced::SGeometryQuads								& mesh						= solarSystem.Scene.Geometry[entityChild.Geometry];
			const ::ced::view_grid<const ::ced::SColorBGRA>			image						= solarSystem.Scene.Image	[entityChild.Image];
			for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
				drawCache.PixelCoords			.clear();
				drawCache.PixelVertexWeights	.clear();
				::ced::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformView, solarSystem.Scene.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel, depthBuffer);
			}
		}
	}
	return 0;
}

static	int											drawExplosion
	( const ::ssg::SSolarSystem							& solarSystem
	, const ::ssg::SExplosion							& explosion
	, const ::ced::SMatrix4<float>						& matrixView
	, ::ced::view_grid<::ced::SColorBGRA>				& targetPixels
	, ::ced::view_grid<uint32_t>						depthBuffer
	, ::ssg::SSolarSystemDrawCache						& drawCache
	) {
	::ced::view_grid<const ::ced::SColorBGRA>				image					= solarSystem.Scene.Image	[explosion.IndexMesh];
	const ::ced::SGeometryQuads								& mesh					= solarSystem.Scene.Geometry[explosion.IndexMesh];
	for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
		const ::ced::SSlice<uint16_t>							& sliceMesh				= explosion.Slices[iExplosionPart];
		::ced::SMatrix4<float>									matrixPart				= {};
		matrixPart.Identity();
		if(iExplosionPart % 5) matrixPart.RotationY(solarSystem.AnimationTime * 2);
		if(iExplosionPart % 3) matrixPart.RotationX(solarSystem.AnimationTime * 2);
		if(iExplosionPart % 2) matrixPart.RotationZ(solarSystem.AnimationTime * 2);
		matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
		::ced::SMatrix4<float>									matrixTransformView		= matrixPart * matrixView;
		::getLightArrays(matrixPart.GetTranslation(), drawCache.LightPointsWorld, drawCache.LightColorsWorld, drawCache.LightPointsModel, drawCache.LightColorsModel);
		for(uint32_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			const uint32_t											iActualTriangle		= sliceMesh.Offset + iTriangle;
			::ced::STriangle3	<float>								triangle			= mesh.Triangles	[iActualTriangle];
			::ced::STriangle3	<float>								triangleWorld		= mesh.Triangles	[iActualTriangle];
			::ced::SCoord3		<float>								normal				= mesh.Normals		[iActualTriangle / 2];
			::ced::STriangle2	<float>								triangleTexCoords	= mesh.TextureCoords[iActualTriangle];
			::ced::STriangle3	<float>								triangleScreen		= triangleWorld;
			::ced::transform(triangleScreen, matrixTransformView);
			if(triangleScreen.ClipZ())
				continue;

			::ced::transform(triangleWorld, matrixPart);
			normal												= matrixPart.TransformDirection(normal).Normalize();
 			::ced::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
 			::ced::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.Scene.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
			drawCache.PixelCoords			.clear();
			drawCache.PixelVertexWeights	.clear();
			triangle											= {triangle.A, triangle.C, triangle.B};
			triangleWorld										= {triangleWorld.A, triangleWorld.C, triangleWorld.B};
			triangleTexCoords									= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
			normal												*= -1;
			::ced::drawQuadTriangle(targetPixels.metrics(), triangle, matrixTransformView, drawCache.PixelCoords, drawCache.PixelVertexWeights, depthBuffer);
  			::ced::drawPixels(targetPixels, triangleWorld, normal, triangleTexCoords, solarSystem.Scene.LightVector, drawCache.PixelCoords, drawCache.PixelVertexWeights, image, drawCache.LightPointsModel, drawCache.LightColorsModel);
		}
	}
	return 0;
}

int													ssg::solarSystemDraw		(const ::ssg::SSolarSystem & solarSystem, ::ssg::SSolarSystemDrawCache & drawCache, ::std::mutex & mutexUpdate, ::ced::view_grid<::ced::SColorBGRA> & targetPixels, ::ced::view_grid<uint32_t> depthBuffer)	{
	//------------------------------------------- Transform and Draw
	if(0 == targetPixels.size())
		return 1;
	memset(depthBuffer.begin(), -1, sizeof(uint32_t) * depthBuffer.size());
	{
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		memcpy(targetPixels.begin(), solarSystem.BackgroundImage.Pixels.begin(), sizeof(::ced::SColorBGRA) * targetPixels.size());
		::drawStars(solarSystem.Stars, targetPixels);
	}
	::ced::SMatrix4<float>									matrixView			= {};
	const ::ced::SCamera									& camera			= solarSystem.Scene.Camera[solarSystem.Scene.CameraMode];
	matrixView.LookAt(camera.Position, camera.Target, camera.Up);
	matrixView											*= solarSystem.Scene.MatrixProjection;
	drawCache.LightPointsWorld.clear();
	drawCache.LightColorsWorld.clear();
	{
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		::getLightArrays(solarSystem, drawCache.LightPointsWorld, drawCache.LightColorsWorld);
	}
	drawCache.LightPointsModel.reserve(drawCache.LightPointsWorld.size());
	drawCache.LightColorsModel.reserve(drawCache.LightColorsWorld.size());

	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		const ::ssg::SShip										& ship					= solarSystem.Ships[iShip];
		if(ship.Health <= 0)
			continue;
		::drawShip(solarSystem, ship, matrixView, targetPixels, depthBuffer, drawCache);
	}
#pragma pack(push, 1)
	struct SRenderNode	{
		uint32_t					Mesh				;
		int32_t						Image				;
		::ced::SSlice<uint16_t>		Slice				;
		::ced::container<uint16_t>	IndicesPointLight	;
	};
#pragma pack(pop)

	::ced::container<uint16_t>								indicesPointLight;
	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		const ::ssg::SExplosion									& explosion				= solarSystem.Explosions[iExplosion];
		if(0 == explosion.Slices.size())
			continue;
		::drawExplosion(solarSystem, explosion, matrixView, targetPixels, depthBuffer, drawCache);
	}

	::ced::container<::ced::SCoord3<float>>					pixelCoordsCache;
	pixelCoordsCache.reserve(512);
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		const ::ssg::SShip										& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			::ced::SColorFloat									colorShot				= ::ced::WHITE;
			double												brightRadius			= 1;
			double												intensity				= 1;
			bool												line					= true;
				 if(::ssg::MUNITION_TYPE_RAY	== ship.Parts[iPart].Shots.Type) { colorShot = ::ced::SColorFloat{1.0f, 0.1f, 0.0f}		; brightRadius =  2.0; intensity =  1; line = true ;}
			else if(::ssg::MUNITION_TYPE_BULLET	== ship.Parts[iPart].Shots.Type) { colorShot = ::ced::DARKGRAY							; brightRadius =  2.0; intensity =  1; line = true ;}
			else if(::ssg::MUNITION_TYPE_SHELL	== ship.Parts[iPart].Shots.Type) {
				colorShot										= ship.Team ? ::ced::SColorFloat{1.0f, 0.125f, 0.25f} : ::ced::TURQUOISE;
				brightRadius									= 7;
				intensity										= 9;
				line											= false;
			}
			::drawShots(targetPixels, ship.Parts[iPart].Shots, matrixView, colorShot, brightRadius, intensity, line, depthBuffer, drawCache.LightPointsModel);
		}
	}
	{
		::std::lock_guard<::std::mutex>							lockUpdate					(mutexUpdate);
		::drawDebris(targetPixels, solarSystem.Debris, matrixView, depthBuffer);
	}
	return 0;
}
