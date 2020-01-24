#include "ced_demo_09.h"

int													drawStars			(SStars & stars, ::ced::view_grid<::ced::SColorBGRA> targetPixels)	{
	::ced::SColorBGRA											colors[]			=
		{ {0xfF, 0xfF, 0xfF, }
		, {0x40, 0x80, 0xfF, }
		, {0xfF, 0x80, 0x40, }
		, {0x80, 0x80, 0x40, }
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::ced::SCoord2<float>									starPos			= stars.Position[iStar];
		::ced::SColorBGRA											starFinalColor	= colors[iStar % ::std::size(colors)] * stars.Brightness[iStar];
		::ced::setPixel(targetPixels, starPos.Cast<int32_t>(), starFinalColor);
		const	int32_t											brightRadius	= 1 + (iStar % 2) + (rand() % 2);
		double													brightUnit		= 1.0 / brightRadius;
		for(int32_t y = -brightRadius; y < brightRadius; ++y)
		for(int32_t x = -brightRadius; x < brightRadius; ++x) {
			::ced::SCoord2<float>									brightPos		= {(float)x, (float)y};
			const double											brightDistance	= brightPos.Length();
			if(brightDistance <= brightRadius) {
				::ced::SCoord2<int32_t>									pixelPos		= (starPos + brightPos).Cast<int32_t>();
				if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
				 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
 				)
					::ced::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + starFinalColor * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
			}
		}
	}
	return 0;
}


int													drawDebris			(::ced::view_grid<::ced::SColorBGRA> targetPixels, SDebris & debris, const ::ced::SMatrix4<float> & matrixVPV, ::ced::view_grid<uint32_t> depthBuffer)	{
	::ced::SColorBGRA											colors[]			=
		{ {0xFF, 0xFF, 0xFF, }
		, {0x20, 0x80, 0xFF, }
		, {0x40, 0xD0, 0xFF, }
		, {0x00, 0x00, 0xFF, }
		};
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iParticle = 0; iParticle < debris.Brightness.size(); ++iParticle) {
		::ced::SColorBGRA											colorShot			= colors[iParticle % ::std::size(colors)];
		::ced::SCoord3<float>									starPos			= debris.Position[iParticle];
		starPos												= matrixVPV.Transform(starPos);
		const ::ced::SCoord2<int32_t>							pixelCoord		= {(int32_t)starPos.x, (int32_t)starPos.y};
		if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
		 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
		)
			continue;
		if(starPos.z > 1 || starPos.z < 0)
			continue;
		uint32_t												depth				= uint32_t((1.0 - starPos.z) * 0xFFFFFFFFU);
		if(depth < depthBuffer[pixelCoord.y][pixelCoord.x])
			continue;
		::ced::SColorBGRA											starFinalColor	= colorShot * debris.Brightness[iParticle];
		::ced::setPixel(targetPixels, pixelCoord, starFinalColor);
		const	double											brightRadius		= 3.0;
		double													brightUnit			= 1.0 / brightRadius;
		for(int32_t y = (int32_t)-brightRadius; y < (int32_t)brightRadius; ++y)
		for(int32_t x = (int32_t)-brightRadius; x < (int32_t)brightRadius; ++x) {
			::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
			const double											brightDistance		= brightPos.Length();
			if(brightDistance <= brightRadius) {
				::ced::SCoord2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
				if( blendPos.y >= 0 && blendPos.y < (int32_t)targetPixels.metrics().y
				 && blendPos.x >= 0 && blendPos.x < (int32_t)targetPixels.metrics().x
				 && depth >= depthBuffer[blendPos.y][blendPos.x]
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

int													drawShots			(::ced::view_grid<::ced::SColorBGRA> targetPixels, SShots & shots, const ::ced::SMatrix4<float> & matrixVPV, ::ced::SColorFloat colorShot, ::ced::view_grid<uint32_t> depthBuffer)	{
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iShot = 0; iShot < shots.Brightness.size(); ++iShot) {
		::ced::SCoord3<float>									starPosPrev		= shots.PositionPrev[iShot];
		::ced::SCoord3<float>									starPos			= shots.Position[iShot];
		::ced::SLine3<float>									raySegment		= {starPosPrev, starPos};
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);

		::ced::SColorFloat											starFinalColor	= colorShot * shots.Brightness[iShot];
		::ced::drawLine(targetPixels,
			{ {(int32_t)raySegment.A.x, (int32_t)raySegment.A.y}
			, {(int32_t)raySegment.B.x, (int32_t)raySegment.B.y}
			}, pixelCoords);
		//if(2 > pixelCoords.size()) {
		//	pixelCoords.push_back({int32_t(raySegment.A.x), int32_t(raySegment.A.y)});
		//	pixelCoords.push_back({int32_t(raySegment.B.x), int32_t(raySegment.B.y + 1)});
		//}
		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
			const ::ced::SCoord2<int32_t>							& pixelCoord		= pixelCoords[iPixelCoord];
			if( pixelCoord.y < 0 || pixelCoord.y >= (int32_t)targetPixels.metrics().y
			 || pixelCoord.x < 0 || pixelCoord.x >= (int32_t)targetPixels.metrics().x
			)
				continue;
			uint32_t												depth				= uint32_t((1.0 - raySegment.A.z) * 0xFFFFFFFFU);
			if(depth <= depthBuffer[pixelCoord.y][pixelCoord.x])
				continue;
			depthBuffer[pixelCoord.y][pixelCoord.x]				= depth;

			::ced::setPixel(targetPixels, pixelCoord, starFinalColor);

			const	double											brightRadius		= 7.5;
			double													brightUnit			= 1.0 / brightRadius;
			for(int32_t y = (int32_t)-brightRadius; y < (int32_t)brightRadius; ++y)
			for(int32_t x = (int32_t)-brightRadius; x < (int32_t)brightRadius; ++x) {
				::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.Length();
				if(brightDistance <= brightRadius) {
					::ced::SCoord2<int32_t>									blendPos			= pixelCoord + (brightPos).Cast<int32_t>();
					if( blendPos.y >= 0 && blendPos.y < (int32_t)targetPixels.metrics().y
					 && blendPos.x >= 0 && blendPos.x < (int32_t)targetPixels.metrics().x
					 && depth >= depthBuffer[blendPos.y][blendPos.x]
					) {
						depthBuffer[blendPos.y][blendPos.x]					= depth;
						double													finalBrightness					= 1.0-(brightDistance * brightUnit);
						::ced::SColorFloat										backgroundColor					= targetPixels[blendPos.y][blendPos.x];
						::ced::setPixel(targetPixels, blendPos, backgroundColor + starFinalColor * finalBrightness);
					}
				}
			}
		}
	}
	return 0;
}

int													draw				(SApplication & app)	{
	//------------------------------------------- Transform and Draw
	::ced::SFramework										& framework			= app.Framework;
	::ced::view_grid<::ced::SColorBGRA>							targetPixels		= {framework.Pixels, framework.Window.Size};
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
	::ced::SMatrix4<float>									matrixProjection	= {};
	::ced::SMatrix4<float>									matrixViewport		= {};
	matrixView.LookAt(app.Scene.Camera.Position, app.Scene.Camera.Target, app.Scene.Camera.Up);
	matrixProjection.FieldOfView(::ced::MATH_PI * .25, targetPixels.metrics().x / (double)targetPixels.metrics().y, 0.01, 1000);
	matrixViewport.Viewport(targetPixels.metrics(), 0.01, 1000);
	matrixView											= matrixView * matrixProjection;
	matrixViewport										= matrixViewport.GetInverse();
	matrixViewport._41									+= targetPixels.metrics().x / 2;
	matrixViewport._42									+= targetPixels.metrics().y / 2;

	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights;
	::ced::SModelTransform									matrices;
	::ced::SModelTransform									matricesParent;
	::ced::SColorBGRA										colorShotPlayer			= ::ced::SColorBGRA{0x40, 0xfF, 0x80};// *.2;
	::ced::SColorBGRA										colorShotEnemy			= ::ced::SColorBGRA{0x00, 0x00, 0xfF};// *.2;
	::ced::SColorBGRA										colorLightPlayer		= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF};// *.2;
	::ced::SColorBGRA										colorLightEnemy			= ::ced::SColorBGRA{0xFF, 0xFF, 0xFF};// *.2;
	for(uint32_t iModel = 0; iModel < app.Scene.Models.size(); ++iModel) {
		if(app.Health[iModel] <= 0)
			continue;
		::ced::SModel3D											& model			= app.Scene.Models[iModel];
		matrices.Scale		.Scale			(model.Scale	, true);
		matrices.Rotation	.Rotation		(model.Rotation);
		matrices.Position	.SetTranslation	(model.Position, true);

		::ced::SEntity											& entity				= app.Scene.Entities[iModel];
		if(-1 == entity.Parent)
			continue;
		matricesParent.Scale	.Scale			(app.Scene.Models[entity.Parent].Scale, true);
		matricesParent.Rotation	.Rotation		(app.Scene.Models[entity.Parent].Rotation);
		matricesParent.Position	.SetTranslation	(app.Scene.Models[entity.Parent].Position, true);

		::ced::SMatrix4<float>									matrixTransform			= matrices.Scale * matrices.Rotation * matrices.Position;
		::ced::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
		matrixTransform										= matrixTransform  * matrixTransformParent ;
		::ced::container<::ced::SCoord3<float>>					lightPoints				= {};
		::ced::container<::ced::SColorBGRA>						lightColors				= {};
		lightPoints.resize(app.ShotsEnemy.Position.size() + app.ShotsPlayer.Position.size() + app.Debris.Position.size() + 4);
		lightColors.resize(app.ShotsEnemy.Position.size() + app.ShotsPlayer.Position.size() + app.Debris.Position.size() + 4);
		lightPoints[0]									= app.Scene.Models[0].Position;
		lightColors[0]									= colorLightPlayer;
		for(uint32_t iEnemy = 1; iEnemy < 4; ++iEnemy) {
			lightPoints[iEnemy]								= app.Scene.Models[7 * iEnemy].Position;
			lightColors[iEnemy]								= colorLightEnemy;
		}
		uint32_t												iOffset					= 4;
		for(uint32_t iShot = 0; iShot < app.ShotsEnemy.Position.size(); ++iShot) {
			lightPoints[iOffset + iShot]						= app.ShotsEnemy.Position[iShot];
			lightColors[iOffset + iShot]						= colorShotEnemy;
		}
		iOffset												+= app.ShotsEnemy.Position.size();
		for(uint32_t iShot = 0; iShot < app.ShotsPlayer.Position.size(); ++iShot) {
			lightPoints[iOffset + iShot]						= app.ShotsPlayer.Position[iShot];
			lightColors[iOffset + iShot]						= colorShotPlayer;
		}
		iOffset												+= app.ShotsPlayer.Position.size();
		for(uint32_t iShot = 0; iShot < app.Debris.Position.size(); ++iShot) {
			lightPoints[iOffset + iShot]						= app.Debris.Position[iShot];
			lightColors[iOffset + iShot]						= {0x05,0x10, 0xFF};
		}
		for(uint32_t iTriangle = 0; iTriangle < app.Scene.Geometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawQuadTriangle(targetPixels, app.Scene.Geometry, iTriangle, matrixTransform, matrixView, matrixViewport, app.Scene.LightVector, pixelCoords, pixelVertexWeights, {app.Scene.Image.Pixels.begin(), app.Scene.Image.Metrics}, lightPoints, lightColors, framework.DepthBuffer);
		}
	}

	::drawShots(targetPixels, app.ShotsPlayer	, matrixView * matrixViewport, colorShotPlayer	, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	::drawShots(targetPixels, app.ShotsEnemy	, matrixView * matrixViewport, colorShotEnemy	, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	::drawDebris(targetPixels, app.Debris		, matrixView * matrixViewport, {app.Framework.DepthBuffer.begin(), targetPixels.metrics()});
	return 0;
}
