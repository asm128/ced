#include "ced_demo_08.h"

int													drawStars			(SStars & stars, ::ced::view_grid<::ced::SColor> targetPixels)	{
	::ced::SColor											colors[]			=
		{ {0xfF, 0xfF, 0xfF}
		, {0xfF, 0x80, 0x40}
		, {0x40, 0x80, 0xfF}
		, {0x40, 0x80, 0x80}
		};
	for(uint32_t iStar = 0; iStar < stars.Brightness.size(); ++iStar) {
		::ced::SCoord2<float>									starPos			= stars.Position[iStar];
		::ced::SColor											starFinalColor	= colors[iStar % ::std::size(colors)] * stars.Brightness[iStar];
		::ced::setPixel(targetPixels, starPos.Cast<int32_t>(), starFinalColor);
		const	int32_t											brightRadius	= 1 + (iStar % 4) + (rand() % 4);
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

int													drawShots			(SApplication & app, const ::ced::SMatrix4<float> & matrixVPV)	{
	::ced::SColor											colorShot			= {0xfF, 0x20, 0x40};
	::ced::SFramework										& framework			= app.Framework;
	::ced::view_grid<::ced::SColor>							targetPixels		= {framework.Pixels, framework.Window.Size};
	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	for(uint32_t iShot = 0; iShot < app.Shots.Brightness.size(); ++iShot) {
		::ced::SCoord3<float>									starPosPrev		= app.Shots.PositionPrev[iShot];
		::ced::SCoord3<float>									starPos			= app.Shots.Position[iShot];
		::ced::SLine3<float>									raySegment		= {starPosPrev, starPos};
		raySegment.A										= matrixVPV.Transform(raySegment.A);
		raySegment.B										= matrixVPV.Transform(raySegment.B);

		::ced::SColor											starFinalColor	= colorShot * app.Shots.Brightness[iShot];
		::ced::drawLine(targetPixels,
			{ {(int32_t)raySegment.A.x, (int32_t)raySegment.A.y}
			, {(int32_t)raySegment.B.x, (int32_t)raySegment.B.y}
			}, pixelCoords);
		for(uint32_t iPixelCoord = 0; iPixelCoord < pixelCoords.size(); ++iPixelCoord) {
			const ::ced::SCoord2<int32_t>							& pixelCoord		= pixelCoords[iPixelCoord];
			::ced::setPixel(targetPixels, pixelCoord, starFinalColor);
			const	int32_t											brightRadius		= 5;
			double													brightUnit			= 1.0 / brightRadius;
			for(int32_t y = -brightRadius; y < brightRadius; ++y)
			for(int32_t x = -brightRadius; x < brightRadius; ++x) {
				::ced::SCoord2<float>									brightPos			= {(float)x, (float)y};
				const double											brightDistance		= brightPos.Length();
				if(brightDistance <= brightRadius) {
					::ced::SCoord2<int32_t>									pixelPos			= pixelCoord + (brightPos).Cast<int32_t>();
					if( pixelPos.y >= 0 && pixelPos.y < (int32_t)targetPixels.metrics().y
					 && pixelPos.x >= 0 && pixelPos.x < (int32_t)targetPixels.metrics().x
						)
						::ced::setPixel(targetPixels, pixelPos, targetPixels[pixelPos.y][pixelPos.x] + colorShot * app.Shots.Brightness[iShot] * (1.0-(brightDistance * brightUnit * (1 + (rand() % 3)))));
				}
			}
		}
	}
	return 0;
}

int													draw				(SApplication & app)	{
	//------------------------------------------- Transform and Draw
	::ced::SFramework										& framework			= app.Framework;
	::ced::view_grid<::ced::SColor>							targetPixels		= {framework.Pixels, framework.Window.Size};
	if(0 == targetPixels.size())
		return 1;
	::ced::SColor											colorBackground		= {0x20, 0x8, 0x4};
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


	::drawShots(app, matrixView * matrixViewport);


	::ced::container<::ced::SCoord2<int32_t>>				pixelCoords;
	::ced::container<::ced::STriangleWeights<double>>		pixelVertexWeights;
	::ced::SModelTransform									matrices;
	::ced::SModelTransform									matricesParent;
	for(uint32_t iModel = 0; iModel < app.Scene.Models.size(); ++iModel) {
		matrices.Scale		.Scale			(app.Scene.Models[iModel].Scale	, true);
		matrices.Rotation	.Rotation		(app.Scene.Models[iModel].Rotation);
		matrices.Position	.SetTranslation	(app.Scene.Models[iModel].Position, true);

		::ced::SEntity											& entity				= app.Scene.Entities[iModel];
		if(-1 == entity.Parent)
			continue;
		matricesParent.Scale	.Scale			(app.Scene.Models[entity.Parent].Scale, true);
		matricesParent.Rotation	.Rotation		(app.Scene.Models[entity.Parent].Rotation);
		matricesParent.Position	.SetTranslation	(app.Scene.Models[entity.Parent].Position, true);

		::ced::SMatrix4<float>									matrixTransform			= matrices.Scale * matrices.Rotation * matrices.Position;
		::ced::SMatrix4<float>									matrixTransformParent	= matricesParent.Scale * matricesParent.Rotation * matricesParent.Position;
		matrixTransform										= matrixTransform  * matrixTransformParent ;
		for(uint32_t iTriangle = 0; iTriangle < app.Scene.Geometry.Triangles.size(); ++iTriangle) {
			pixelCoords			.clear();
			pixelVertexWeights	.clear();
			::ced::drawQuadTriangle(targetPixels, app.Scene.Geometry, iTriangle, matrixTransform, matrixView, matrixViewport, app.Scene.LightVector, pixelCoords, pixelVertexWeights, {app.Scene.Image.Pixels.begin(), app.Scene.Image.Metrics}, framework.DepthBuffer);
		}
	}
	return 0;
}
