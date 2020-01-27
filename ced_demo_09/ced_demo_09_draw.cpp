#include "ced_demo_09.h"
#include <algorithm>

static constexpr	const uint32_t					MAX_LIGHT_RANGE				= 10;

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
		const	double											brightRadiusSquared	= brightRadius * brightRadius;
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
				if( blendPos.y >= 0 && blendPos.y < (int32_t)targetPixels.metrics().y
				 && blendPos.x >= 0 && blendPos.x < (int32_t)targetPixels.metrics().x
				 && depth <= depthBuffer[blendPos.y][blendPos.x]
				) {
					depthBuffer[blendPos.y][blendPos.x]					= depth;
					double													finalBrightness					= 1.0-(brightDistance * brightUnit);
					::ced::SColorFloat										backgroundColor					= targetPixels[blendPos.y][blendPos.x];
					::ced::setPixel(targetPixels, blendPos, backgroundColor + starFinalColor * finalBrightness);
				}
			}
		}
	}
	return 0;
}

static	int											drawShots			(::ced::view_grid<::ced::SColorBGRA> targetPixels, SShots & shots
	, const ::ced::SMatrix4<float> & matrixVPV
	, ::ced::SColorFloat colorShot, ::ced::view_grid<uint32_t> depthBuffer)	{
	::ced::container<::ced::SCoord3<float>>					pixelCoords;
	const ::ced::SColorFloat								starFinalColor	= colorShot;// * shots.Brightness[iShot];
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		pixelCoords.clear();
		::ced::SCoord3<float>									starPosPrev		= shots.PositionPrev[iShot];
		::ced::SCoord3<float>									starPos			= shots.Particles.Position[iShot];
		::ced::SLine3<float>									raySegmentWorld	= {starPosPrev, starPos};

		::ced::SLine3<float>									raySegment		= raySegmentWorld;
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);

		::ced::drawLine(targetPixels, raySegment, pixelCoords, depthBuffer);
		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
			const ::ced::SCoord3<float>							& pixelCoord		= pixelCoords[iPixelCoord];
			if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
			 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
			)
				continue;
			uint32_t												depth				= uint32_t(pixelCoord.z * 0xFFFFFFFFU);
			::ced::setPixel(targetPixels, {(int32_t)pixelCoord.x, (int32_t)pixelCoord.y}, starFinalColor);

			const	double											brightRadius		= 7.5;
			const	double											brightRadiusSquared	= brightRadius * brightRadius;
			double													brightUnit			= 1.0 / brightRadiusSquared;
			for(int32_t y = (int32_t)-brightRadius; y < (int32_t)brightRadius; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < (int32_t)brightRadius; ++x) {
				::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.LengthSquared();
				if(brightDistance <= brightRadiusSquared) {
					::ced::SCoord2<int32_t>									blendPos			= ::ced::SCoord2<int32_t>{(int32_t)pixelCoord.x, (int32_t)pixelCoord.y} + (brightPos).Cast<int32_t>();
					if( blendPos.y >= 0 && blendPos.y < (int32_t)targetPixels.metrics().y
					 && blendPos.x >= 0 && blendPos.x < (int32_t)targetPixels.metrics().x
					 && depth <= depthBuffer[blendPos.y][blendPos.x]
					) {
						depthBuffer[blendPos.y][blendPos.x]					= depth;
						double													finalBrightness					= (1.0 - (brightDistance * brightUnit)) * (1.0 / pixelCoords.size() * iPixelCoord);
						::ced::SColorFloat										backgroundColor					= targetPixels[blendPos.y][blendPos.x];
						::ced::SColorFloat										pixelColor						= backgroundColor + starFinalColor * finalBrightness;
						::ced::setPixel(targetPixels, blendPos, pixelColor);
					}
				}
			}
		}

	}
	return 0;
}

static constexpr	const ::ced::SColorBGRA			colorShotPlayer			= ::ced::SColorBGRA{0x40, 0xfF, 0x80};// *.2;
static constexpr	const ::ced::SColorBGRA			colorShotEnemy			= ::ced::SColorBGRA{0x00, 0x00, 0xfF};// *.2;


static	int											getLightArrays
	( const ::SApplication									& app
	, ::ced::container<::ced::SCoord3<float>>				& lightPoints
	, ::ced::container<::ced::SColorBGRA>					& lightColors
	)						{
	::ced::SColorBGRA										colorLightPlayer		= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF}  * .2;
	::ced::SColorBGRA										colorLightEnemy			= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF}  * .2;
	lightPoints.resize(app.ShotsEnemy.Particles.Position.size() + app.ShotsPlayer.Particles.Position.size() + app.Debris.Particles.Position.size() + 4);
	lightColors.resize(app.ShotsEnemy.Particles.Position.size() + app.ShotsPlayer.Particles.Position.size() + app.Debris.Particles.Position.size() + 4);
	lightPoints[0]									= app.Scene.Models[0].Position;
	lightColors[0]									= colorLightPlayer;
	for(uint32_t iEnemy = 1; iEnemy < 4; ++iEnemy) {
		uint32_t iModelEnemy = 7 * iEnemy;
		if(iModelEnemy >= app.Scene.Models.size())
			continue;
		lightPoints[iEnemy]								= app.Scene.Models[iModelEnemy].Position;
		lightColors[iEnemy]								= colorLightEnemy;
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
		lightPoints[iOffset + iParticle]						= app.Debris.Particles.Position[iParticle];
		::ced::SColorFloat									colorShot			= app.Debris.Colors[iParticle % ::std::size(app.Debris.Colors)];
		lightColors[iOffset + iParticle]						= colorShot * app.Debris.Brightness[iParticle];
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
		if((lightPointsWorld[iLightPoint] - modelPosition).LengthSquared() < (MAX_LIGHT_RANGE * MAX_LIGHT_RANGE)) {
			lightPointsModel.push_back(lightPointsWorld[iLightPoint]);
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
	::ced::SColorBGRA											colorBackground		= {0x20, 0x8, 0x4};
	//colorBackground										+= (colorBackground * (0.5 + (0.5 / 65535 * rand())) * ((rand() % 2) ? -1 : 1)) ;
	for(uint32_t y = 0; y < framework.Window.Size.y; ++y) // Generate noise color for planet texture
	for(uint32_t x = 0; x < framework.Window.Size.x; ++x)
		framework.Pixels[y * framework.Window.Size.x + x]	= colorBackground;

	drawStars(app.Stars, {framework.Pixels, framework.Window.Size});

	app.Scene.LightVector.Normalize();

	::ced::SMatrix4<float>									matrixView			= {};
	matrixView.LookAt(app.Scene.Camera.Position, app.Scene.Camera.Target, app.Scene.Camera.Up);
	{
		::ced::SMatrix4<float>									matrixProjection	= {};
		matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.1, 1000);
		matrixView											= matrixView * matrixProjection;
	}
	{
		::ced::SMatrix4<float>									matrixViewport		= {};
		matrixViewport.Viewport(targetPixels.metrics(), 0.01, 1000);
		matrixViewport										= matrixViewport.GetInverse();
		matrixViewport._41									+= targetPixels.metrics().x / 2;
		matrixViewport._42									+= targetPixels.metrics().y / 2;
		matrixView											*= matrixViewport;
	}

	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights;

	::ced::container<::ced::SCoord3<float>>					lightPointsWorld			= {};
	::ced::container<::ced::SColorBGRA>						lightColorsWorld			= {};
	::ced::container<::ced::SCoord3<float>>					lightPointsModel			= {};
	::ced::container<::ced::SColorBGRA>						lightColorsModel			= {};
	::getLightArrays(app, lightPointsWorld, lightColorsWorld);

	for(uint32_t iModel = 0; iModel < app.Scene.Models.size(); ++iModel) {
		if(app.Health[iModel] <= 0)
			continue;
		::ced::SEntity											& entity				= app.Scene.Entities[iModel];
		if(-1 == entity.Parent)
			continue;
		::ced::SMatrix4<float>									matrixTransform			= app.Scene.ModelMatricesLocal[iModel];
		const ::ced::SMatrix4<float>							& matrixTransformParent	= app.Scene.ModelMatricesLocal[entity.Parent];
		matrixTransform										= matrixTransform  * matrixTransformParent ;
		::getLightArrays(matrixTransform.GetTranslation(), lightPointsWorld, lightColorsWorld, lightPointsModel, lightColorsModel);
		for(uint32_t iTriangle = 0; iTriangle < app.Scene.Geometry[iModel / 7].Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawQuadTriangle(targetPixels, app.Scene.Geometry[iModel / 7], iTriangle, matrixTransform, matrixView, app.Scene.LightVector, pixelCoords, pixelVertexWeights, {app.Scene.Image[iModel / 7].Pixels.begin(), app.Scene.Image[iModel / 7].Metrics}, lightPointsModel, lightColorsModel, {framework.DepthBuffer.begin(), framework.Window.Size});
		}
	}

	for(uint32_t iExplosion = 0; iExplosion < app.Explosions.size(); ++iExplosion) {
		const ::SExplosion							& explosion				= app.Explosions[iExplosion];
		::ced::SEntity								& entity				= app.Scene.Entities[explosion.IndexEntity];
		if(-1 == entity.Parent)
			continue;

		for(uint32_t iExplosionPart = 0; iExplosionPart < explosion.Particles.Position.size(); ++iExplosionPart) {
			const SSlice								& sliceMesh				= explosion.Slices[iExplosionPart];
			::ced::SMatrix4<float>						matrixPart				= {};
			matrixPart.Identity();
			matrixPart.RotationX(app.AnimationTime * 2);
			matrixPart.SetTranslation(explosion.Particles.Position[iExplosionPart], false);
			::getLightArrays(matrixPart.GetTranslation(), lightPointsWorld, lightColorsWorld, lightPointsModel, lightColorsModel);
			for(uint32_t iTriangle = 0, countTriangles = sliceMesh.Count; iTriangle < countTriangles; ++iTriangle) {
				pixelCoords			.clear();
				pixelVertexWeights	.clear();
				const uint32_t											iActualTriangle		= sliceMesh.Offset + iTriangle;
				const ::ced::SGeometryQuads								& geometry			= app.Scene.Geometry[explosion.IndexMesh];
				::ced::STriangle3	<float>								triangle			= geometry.Triangles	[iActualTriangle];
				::ced::SCoord3		<float>								normal				= geometry.Normals		[iActualTriangle / 2];
				::ced::STriangle2	<float>								triangleTexCoords	= geometry.TextureCoords[iActualTriangle];
  				::ced::drawQuadTriangle(targetPixels, triangle, normal, triangleTexCoords, matrixPart, matrixView, app.Scene.LightVector, pixelCoords, pixelVertexWeights, {app.Scene.Image[explosion.IndexMesh].Pixels.begin(), app.Scene.Image[explosion.IndexMesh].Metrics}, lightPointsModel, lightColorsModel, {framework.DepthBuffer.begin(), framework.Window.Size});
				pixelCoords			.clear();
				pixelVertexWeights	.clear();
				triangle											= {triangle.A, triangle.C, triangle.B};
				triangleTexCoords									= {triangleTexCoords.A, triangleTexCoords.C, triangleTexCoords.B};
				normal.x											*= -1;
				normal.y											*= -1;
				::ced::drawQuadTriangle(targetPixels, triangle, normal, triangleTexCoords, matrixPart, matrixView, app.Scene.LightVector, pixelCoords, pixelVertexWeights, {app.Scene.Image[explosion.IndexMesh].Pixels.begin(), app.Scene.Image[explosion.IndexMesh].Metrics}, lightPointsModel, lightColorsModel, {framework.DepthBuffer.begin(), framework.Window.Size});
			}
		}
	}

	::drawShots(targetPixels, app.ShotsPlayer	, matrixView, colorShotPlayer	, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	::drawShots(targetPixels, app.ShotsEnemy	, matrixView, colorShotEnemy	, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	::drawDebris(targetPixels, app.Debris		, matrixView, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	return 0;
}
