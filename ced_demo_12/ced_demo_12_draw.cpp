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
		const	int32_t											brightRadius		= 1 + (iStar % 5) + (rand() % 4);
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


static	int											drawDebris			(::ced::view_grid<::ced::SColorBGRA> targetPixels, SDebris & debris, const ::ced::SMatrix4<float> & matrixVPV, ::ced::view_grid<uint32_t> depthBuffer)	{
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		::ced::SColorFloat										colorShot			= debris.Colors[iParticle % ::std::size(debris.Colors)];
		::ced::SCoord3<float>									starPos				= debris.Particles.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		const ::ced::SCoord2<int32_t>							pixelCoord			= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		uint32_t												depth				= uint32_t(starPos.z * 0xFFFFFFFFU);
		if(depth > depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::ced::SColorFloat											starFinalColor	= colorShot * debris.Brightness[iParticle];
		starFinalColor.g										= ::std::max(0.0f, starFinalColor.g - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 2))));
		starFinalColor.b										= ::std::max(0.0f, starFinalColor.b - (1.0f - ::std::min(1.0f, debris.Brightness[iParticle] * 2.5f * (1.0f / debris.Brightness.size() * iParticle * 1))));
		//::ced::setPixel(targetPixels, pixelCoord, starFinalColor);
		const	double											brightRadius		= 3.0;
		const	double											brightRadiusSquared	= brightRadius * brightRadius;
		double													brightUnit			= 1.0 / brightRadiusSquared;

		for(int32_t y = (int32_t)-brightRadius; y < (int32_t)brightRadius; ++y)
		for(int32_t x = (int32_t)-brightRadius; x < (int32_t)brightRadius; ++x) {
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

static	int											drawShots			(::ced::view_grid<::ced::SColorBGRA> targetPixels, SShots & shots
	, const ::ced::SMatrix4<float>	& matrixVPV
	, ::ced::SColorFloat			colorShot
	, const	double					brightRadius
	, const	double					intensity
	, const	bool					line
	, ::ced::view_grid<uint32_t>	depthBuffer
	) {
	::ced::container<::ced::SCoord3<float>>					pixelCoords;
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		pixelCoords.clear();
		::ced::SCoord3<float>									starPosPrev		= shots.PositionPrev[iShot];
		::ced::SCoord3<float>									starPos			= shots.Particles.Position[iShot];
		::ced::SLine3<float>									raySegmentWorld	= {starPosPrev, starPos};

		::ced::SLine3<float>									raySegment		= raySegmentWorld;
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);
		if(line)
			::ced::drawLine(targetPixels, raySegment, pixelCoords, depthBuffer);
		else
			pixelCoords.push_back(raySegment.A);

		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
			const ::ced::SCoord3<float>							& pixelCoord		= pixelCoords[iPixelCoord];
			if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
			 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
			)
				continue;
			targetPixels[(uint32_t)pixelCoord.y][(uint32_t)pixelCoord.x] = colorShot;

			uint32_t												depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
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
					double													finalBrightness					= (1.0 - (brightDistance * brightUnit)) * (line ? (1.0 / pixelCoords.size() * iPixelCoord) : 1);
					::ced::SColorFloat										pixelColor						= colorShot * finalBrightness * intensity + pixelVal;
					pixelVal											= pixelColor;
				}
			}
		}

	}
	return 0;
}

static constexpr	const ::ced::SColorBGRA			colorShotPlayer			= ::ced::SColorBGRA{0x40, 0xfF, 0x80};// *.2;
static constexpr	const ::ced::SColorBGRA			colorShotEnemy			= ::ced::SColorBGRA{0x40, 0x20, 0xfF};// *.2;


static	int											getLightArrays
	( const ::SSolarSystem						& app
	, ::ced::container<::ced::SCoord3<float>>	& lightPoints
	, ::ced::container<::ced::SColorBGRA>		& lightColors
	)						{
	::ced::SColorBGRA										colorLightPlayer		= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF}  * .2;
	::ced::SColorBGRA										colorLightEnemy			= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF}  * .2;
	lightPoints.resize(app.ShotsEnemy.Particles.Position.size() + app.ShotsPlayer.Particles.Position.size() + app.Debris.Particles.Position.size() + 4);
	lightColors.resize(app.ShotsEnemy.Particles.Position.size() + app.ShotsPlayer.Particles.Position.size() + app.Debris.Particles.Position.size() + 4);
	lightPoints[0]										= app.Scene.Transforms[0].Position;
	lightColors[0]										= colorLightPlayer;
	for(uint32_t iEnemy = 1; iEnemy < 4; ++iEnemy) {
		uint32_t iModelEnemy = 7 * iEnemy;
		if(iModelEnemy >= app.Scene.Transforms.size())
			continue;
		lightPoints[iEnemy]									= app.Scene.Transforms[iModelEnemy].Position;
		lightColors[iEnemy]									= colorLightEnemy;
	}
	uint32_t												iOffset					= 4;
	for(uint32_t iShot = 0; iShot < app.ShotsEnemy.Particles.Position.size(); ++iShot) {
		lightPoints[iOffset + iShot]						= app.ShotsEnemy.Particles.Position[iShot];
		lightColors[iOffset + iShot]						= colorShotEnemy;
	}
	iOffset												+= app.ShotsEnemy.Particles.Position.size();
	for(uint32_t iShot = 0; iShot < app.ShotsPlayer.Particles.Position.size(); ++iShot) {
		lightPoints[iOffset + iShot]						= app.ShotsPlayer.Particles.Position[iShot];
		lightColors[iOffset + iShot]						= colorShotPlayer;
	}
	iOffset												+= app.ShotsPlayer.Particles.Position.size();
	for(uint32_t iParticle = 0; iParticle < app.Debris.Particles.Position.size(); ++iParticle) {
		lightPoints[iOffset + iParticle]					= app.Debris.Particles.Position[iParticle];
		::ced::SColorFloat										colorShot			= app.Debris.Colors[iParticle % ::std::size(app.Debris.Colors)];
		lightColors[iOffset + iParticle]					= colorShot * app.Debris.Brightness[iParticle];
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
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	::ced::container<::ced::STriangleWeights<float>>		pixelVertexWeights;

	::ced::container<::ced::SCoord3<float>>					lightPointsWorld			= {};
	::ced::container<::ced::SColorBGRA>						lightColorsWorld			= {};
	::ced::container<::ced::SCoord3<float>>					lightPointsModel			= {};
	::ced::container<::ced::SColorBGRA>						lightColorsModel			= {};
	::getLightArrays(solarSystem, lightPointsWorld, lightColorsWorld);
	lightPointsModel.resize(lightPointsWorld.size());
	lightColorsModel.resize(lightColorsWorld.size());
	lightPointsModel.clear();
	lightColorsModel.clear();

	::ced::view_grid<uint32_t>								depthBuffer					= {framework.DepthBuffer.begin(), framework.Window.Size};
	for(uint32_t iEntity = 0; iEntity < solarSystem.Entities.size(); ++iEntity) {
		if(solarSystem.Health[iEntity] <= 0)
			continue;
		::SEntity												& entity					= solarSystem.Entities[iEntity];
		if(-1 == entity.Parent)
			continue;
		::ced::SMatrix4<float>									matrixTransform				= solarSystem.Scene.ModelMatricesLocal[entity.Transform];
		const ::ced::SMatrix4<float>							& matrixTransformParent		= solarSystem.Scene.ModelMatricesLocal[solarSystem.Entities[entity.Parent].Transform];
		matrixTransform										= matrixTransform * matrixTransformParent ;
		::ced::SMatrix4<float>									matrixTransformView			= matrixTransform * matrixView;
		::getLightArrays(matrixTransform.GetTranslation(), lightPointsWorld, lightColorsWorld, lightPointsModel, lightColorsModel);
		::ced::SGeometryQuads									& mesh						= solarSystem.Scene.Geometry[entity.Geometry];
		::ced::view_grid<::ced::SColorBGRA>						image						= solarSystem.Scene.Image	[entity.Image];
		for(uint32_t iTriangle = 0; iTriangle < mesh.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawQuadTriangle(targetPixels, mesh, iTriangle, matrixTransform, matrixTransformView, solarSystem.Scene.LightVector, pixelCoords, pixelVertexWeights, image, lightPointsModel, lightColorsModel, depthBuffer);
		}
	}

	for(uint32_t iExplosion = 0; iExplosion < solarSystem.Explosions.size(); ++iExplosion) {
		const ::SExplosion							& explosion				= solarSystem.Explosions[iExplosion];

		::ced::view_grid<::ced::SColorBGRA>			image					= solarSystem.Scene.Image	[explosion.IndexMesh];
		const ::ced::SGeometryQuads					& mesh					= solarSystem.Scene.Geometry[explosion.IndexMesh];
		for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
			const SSlice								& sliceMesh				= explosion.Slices[iExplosionPart];
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

	::drawShots	(targetPixels, solarSystem.ShotsPlayer	, matrixView, colorShotPlayer	, 4.0,  1, true	, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	::drawShots	(targetPixels, solarSystem.ShotsEnemy	, matrixView, colorShotEnemy	, 7.0, 10, false, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	::drawDebris(targetPixels, solarSystem.Debris		, matrixView, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	return 0;
}
