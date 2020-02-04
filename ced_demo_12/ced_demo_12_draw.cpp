#include "ced_demo_12.h"
#include <algorithm>

static constexpr	const uint32_t					MAX_LIGHT_RANGE		= 10;

static	int											drawStars			(SStars & stars, ::ced::view_grid<::ced::SColorBGRA> targetPixels)	{
	::ced::SColorBGRA										colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0x40, 0x80, 0xfF, }
		, {0xfF, 0x80, 0x40, }
		, {0x80, 0x80, 0x40, }
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
	, SDebris									& debris
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

static	int											drawShots			(::ced::view_grid<::ced::SColorBGRA> targetPixels, const SShots & shots
	, const ::ced::SMatrix4<float>				& matrixVPV
	, ::ced::SColorFloat						colorShot
	, const	double								brightRadius
	, const	double								intensity
	, const	bool								line
	, ::ced::view_grid<uint32_t>				depthBuffer
	, ::ced::container<::ced::SCoord3<float>>	pixelCoordsCache
	) {
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		pixelCoordsCache.clear();
		const ::ced::SCoord3<float>								& starPosPrev		= shots.PositionDraw[iShot];
		const ::ced::SCoord3<float>								& starPos			= shots.Particles.Position[iShot];
		::ced::SLine3<float>									raySegmentWorld	= {starPosPrev, starPos};

		::ced::SLine3<float>									raySegment		= raySegmentWorld;
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);
		if(line)
			::ced::drawLine(targetPixels, raySegment, pixelCoordsCache, depthBuffer);
		else
			pixelCoordsCache.push_back(raySegment.A);

		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoordsCache.size(); ++iPixelCoord) {
			const ::ced::SCoord3<float>							& pixelCoord		= pixelCoordsCache[iPixelCoord];
			if(pixelCoord.z < 0 || pixelCoord.z > 1)
				continue;
			if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
			 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
			)
				continue;
			targetPixels[(uint32_t)pixelCoord.y][(uint32_t)pixelCoord.x]	= colorShot;
			const uint32_t											depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			const	double											brightRadiusSquared	= brightRadius * brightRadius;
			double													brightUnit			= 1.0 / brightRadiusSquared;
			for(int32_t y = (int32_t)-brightRadius, brightCount = (int32_t)brightRadius; y < brightCount; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < brightCount; ++x) {
				::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::ced::SCoord2<int32_t>									blendPos			= ::ced::SCoord2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).Cast<int32_t>();
					if( blendPos.y < 0 || blendPos.y >= (int32_t)targetPixels.metrics().y
					 || blendPos.x < 0 || blendPos.x >= (int32_t)targetPixels.metrics().x
					)
						continue;
					uint32_t												& blendVal			= depthBuffer[blendPos.y][blendPos.x];
					if(depth > blendVal)
						continue;
					blendVal											= depth;
					::ced::SColorBGRA										& pixelVal						= targetPixels[blendPos.y][blendPos.x];
					double													finalBrightness					= (1.0 - (brightDistance * brightUnit)) * (line ? (1.0 / pixelCoordsCache.size() * iPixelCoord) : 1);
					::ced::SColorFloat										pixelColor						= colorShot * finalBrightness * intensity + pixelVal;
					pixelVal											= pixelColor;
				}
			}
		}

	}
	return 0;
}

static	int											getLightArrays
	( const ::SSolarSystem						& solarSystem
	, ::ced::container<::ced::SCoord3<float>>	& lightPoints
	, ::ced::container<::ced::SColorBGRA>		& lightColors
	)						{
	::ced::SColorBGRA										colorLightPlayer		= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF};
	::ced::SColorBGRA										colorLightEnemy			= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF};
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		const ::SShip											& ship					= solarSystem.Ships[iShip];
		lightPoints.push_back(solarSystem.Scene.Transforms[ship.Entity].Position);
		lightColors.push_back((0 == solarSystem.Ships[iShip].Team) ? colorLightPlayer : colorLightEnemy);
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			const ::SShipPart										& shipPart				= ship.Parts[iPart];
			const ::ced::SColorFloat								colorShot
				= (MUNITION_TYPE_RAY	== ship.Parts[iPart].Type) ? ::ced::RED
				: (MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? ::ced::SColorFloat{::ced::SColorBGRA{0x40, 0x20, 0xfF}}
				: (MUNITION_TYPE_BULLET == ship.Parts[iPart].Type) ? ::ced::DARKGRAY	//::ced::SColorFloat{::ced::SColorBGRA{0x40, 0xfF, 0x80}}
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

int													draw				(SApplication & app)	{
	//------------------------------------------- Transform and Draw
	::ced::SFramework										& framework			= app.Framework;
	::ced::view_grid<::ced::SColorBGRA>						targetPixels		= {framework.Pixels, framework.Window.Size};
	if(0 == targetPixels.size())
		return 1;
	memcpy(targetPixels.begin(), app.SolarSystem.BackgroundImage.Pixels.begin(), sizeof(::ced::SColorBGRA) * targetPixels.size());

	::SSolarSystem											& solarSystem		= app.SolarSystem;

	::drawStars(app.SolarSystem.Stars, {framework.Pixels, framework.Window.Size});

	solarSystem.Scene.LightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	matrixView.LookAt(solarSystem.Scene.Camera.Position, solarSystem.Scene.Camera.Target, solarSystem.Scene.Camera.Up);
	matrixView											*= solarSystem.Scene.MatrixProjection;
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords					= {};
	::ced::container<::ced::STriangleWeights<float>>		pixelVertexWeights			= {};

	::ced::container<::ced::SCoord3<float>>					lightPointsWorld			= {};
	::ced::container<::ced::SColorBGRA>						lightColorsWorld			= {};
	::ced::container<::ced::SCoord3<float>>					lightPointsModel			= {};
	::ced::container<::ced::SColorBGRA>						lightColorsModel			= {};
	lightPointsWorld.reserve(2048);
	lightColorsWorld.reserve(2048);
	::getLightArrays(solarSystem, lightPointsWorld, lightColorsWorld);
	lightPointsModel.reserve(lightPointsWorld.size());
	lightColorsModel.reserve(lightColorsWorld.size());

	::ced::view_grid<uint32_t>								depthBuffer					= {framework.DepthBuffer.begin(), framework.Window.Size};
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		::SShip													& ship					= solarSystem.Ships[iShip];
		if(ship.Health <= 0)
			continue;
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			::SShipPart												& shipPart				= ship.Parts[iPart];
			if(shipPart.Health <= 0)
				continue;
			::SEntity												& entity					= solarSystem.Entities[shipPart.Entity];
			for(uint32_t iEntity = 0; iEntity < entity.Children.size(); ++iEntity) {
				::SEntity												& entityChild				= solarSystem.Entities[entity.Children[iEntity]];
				if(-1 == entityChild.Parent)
					continue;
				if(-1 == entityChild.Geometry)
					continue;
				::ced::SMatrix4<float>									matrixTransform				= solarSystem.Scene.ModelMatricesGlobal[entityChild.Transform];
				::ced::SMatrix4<float>									matrixTransformView			= matrixTransform * matrixView;
				::getLightArrays(matrixTransform.GetTranslation(), lightPointsWorld, lightColorsWorld, lightPointsModel, lightColorsModel);
				::ced::SGeometryQuads									& mesh						= solarSystem.Scene.Geometry[entityChild.Geometry];
				::ced::view_grid<::ced::SColorBGRA>						image						= solarSystem.Scene.Image	[entityChild.Image];
				for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
					pixelCoords			.clear();
					pixelVertexWeights	.clear();
					::ced::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformView, solarSystem.Scene.LightVector, pixelCoords, pixelVertexWeights, image, lightPointsModel, lightColorsModel, depthBuffer);
				}
			}
		}
	}

	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
		const ::SExplosion							& explosion				= solarSystem.Explosions[iExplosion];

		::ced::view_grid<::ced::SColorBGRA>			image					= solarSystem.Scene.Image	[explosion.IndexMesh];
		const ::ced::SGeometryQuads					& mesh					= solarSystem.Scene.Geometry[explosion.IndexMesh];
		for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
			const ::ced::SSlice<uint16_t>				& sliceMesh				= explosion.Slices[iExplosionPart];
			::ced::SMatrix4<float>						matrixPart				= {};
			matrixPart.Identity();
			matrixPart.RotationX(solarSystem.AnimationTime * 2);
			matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
			::ced::SMatrix4<float>									matrixTransformView		= matrixPart * matrixView;
			::getLightArrays(matrixPart.GetTranslation(), lightPointsWorld, lightColorsWorld, lightPointsModel, lightColorsModel);
			for(uint32_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
				pixelCoords			.clear();
				pixelVertexWeights	.clear();
				const uint32_t											iActualTriangle		= sliceMesh.Offset + iTriangle;
				::ced::STriangle3	<float>								triangleWorld		= mesh.Triangles	[iActualTriangle];
				::ced::SCoord3		<float>								normal				= mesh.Normals		[iActualTriangle / 2];
				::ced::STriangle2	<float>								triangleTexCoords	= mesh.TextureCoords[iActualTriangle];
				::ced::STriangle3	<float>								triangleScreen		= triangleWorld;
				::ced::transform(triangleWorld, matrixPart);
				::ced::transform(triangleScreen, matrixTransformView);
				if(triangleScreen.A.z < 0 || triangleScreen.A.z >= 1) continue;
				if(triangleScreen.B.z < 0 || triangleScreen.B.z >= 1) continue;
				if(triangleScreen.C.z < 0 || triangleScreen.C.z >= 1) continue;

				normal												= matrixPart.TransformDirection(normal).Normalize();

  				::ced::drawQuadTriangle(targetPixels, triangleWorld, triangleScreen, normal, triangleTexCoords, solarSystem.Scene.LightVector, pixelCoords, pixelVertexWeights, image, lightPointsModel, lightColorsModel, depthBuffer);
				pixelCoords			.clear();
				pixelVertexWeights	.clear();
				triangleWorld										= {triangleWorld.A, triangleWorld.C, triangleWorld.B};
				triangleScreen										= {triangleScreen.A, triangleScreen.C, triangleScreen.B};
				triangleTexCoords									= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
				normal.x											*= -1;
				normal.y											*= -1;
  				::ced::drawQuadTriangle(targetPixels, triangleWorld, triangleScreen, normal, triangleTexCoords, solarSystem.Scene.LightVector, pixelCoords, pixelVertexWeights, image, lightPointsModel, lightColorsModel, depthBuffer);
			}
		}
	}

	::ced::container<::ced::SCoord3<float>>					pixelCoordsCache;
	pixelCoordsCache.reserve(512);
	for(uint32_t iShip = 0; iShip < solarSystem.Ships.size(); ++iShip) {
		const ::SShip										& ship					= solarSystem.Ships[iShip];
		for(uint32_t iPart = 0; iPart < ship.Parts.size(); ++iPart) {
			const ::ced::SColorFloat							colorShot
				= (MUNITION_TYPE_RAY	== ship.Parts[iPart].Type) ? ::ced::RED
				: (MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? ::ced::SColorFloat{::ced::SColorBGRA{0x40, 0x20, 0xfF}}
				: (MUNITION_TYPE_BULLET == ship.Parts[iPart].Type) ? ::ced::DARKGRAY	//::ced::SColorFloat{::ced::SColorBGRA{0x40, 0xfF, 0x80}}
				: ::ced::SColorFloat{::ced::SColorBGRA{0xFF, 0xFF, 0xFF}}
				;
			const	double								brightRadius
				= (MUNITION_TYPE_RAY	== ship.Parts[iPart].Type) ? 4.0
				: (MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? 10.0
				: (MUNITION_TYPE_BULLET == ship.Parts[iPart].Type) ? 4.0
				: 4.0
				;
			const	double								intensity
				= (MUNITION_TYPE_RAY	== ship.Parts[iPart].Type) ? 1
				: (MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? 10
				: (MUNITION_TYPE_BULLET == ship.Parts[iPart].Type) ? 1
				: 1
				;
			const	bool								line
				= (MUNITION_TYPE_RAY	== ship.Parts[iPart].Type) ? true
				: (MUNITION_TYPE_SHELL	== ship.Parts[iPart].Type) ? false
				: (MUNITION_TYPE_BULLET == ship.Parts[iPart].Type) ? true
				: false
				;
			::drawShots(targetPixels, ship.Parts[iPart].Shots, matrixView, colorShot, brightRadius, intensity, line, depthBuffer, pixelCoordsCache);
		}
	}
	::drawDebris(targetPixels, solarSystem.Debris		, matrixView, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	return 0;
}
