#include "SkiaImage.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Drafts/SkiaImage.h"
#include <QOpenGLExtraFunctions>
#include "include/gpu/ganesh/GrDirectContext.h"
#include "CityDraft/Assets/SkiaAssetManager.h"
#include <include/core/SkImage.h>
#include <include/gpu/ganesh/SkImageGanesh.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <GL/gl.h>

namespace CityDraft::Assets
{
	SkiaImage::SkiaImage(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger):
		Image(path, assetManager, logger)
	{
	}

	std::shared_ptr<Drafts::Draft> SkiaImage::CreateDraft()
	{
		return std::make_shared<Drafts::SkiaImage>(this);
	}

	void SkiaImage::LoadImage(const CityDraft::Utils::StbPixels& stbPixels, int pivotX, int pivotY, int sizeX, int sizeY)
	{
		CreateGpuImage(stbPixels, pivotX, pivotY, sizeX, sizeY);
		CreateQtImage(stbPixels, pivotX, pivotY, sizeX, sizeY);
		
		m_Logger->info("SkiaImage loaded");
		m_Status = AssetStatus::Loaded;
	}

	sk_sp<GrDirectContext> SkiaImage::GetDirectContext() const
	{
		SkiaAssetManager* skiaManager = dynamic_cast<SkiaAssetManager*>(m_AssetManager);
		BOOST_ASSERT(skiaManager);

		return skiaManager->GetDirectContext();
	}

	QOpenGLExtraFunctions& SkiaImage::GetGlFunctions()
	{
		SkiaAssetManager* skiaManager = dynamic_cast<SkiaAssetManager*>(m_AssetManager);
		BOOST_ASSERT(skiaManager);

		return skiaManager->GetGlFunctions();
	}

	sk_sp<SkImage> SkiaImage::GetGpuImage() const
	{
		return m_GpuImage;
	}

	const QPixmap& SkiaImage::GetPixmap() const
	{
		return m_QtImage;
	}

	Vector2D SkiaImage::GetImageSize() const
	{
		if (!m_GpuImage)
		{
			m_Logger->error("Trying to get size of unloaded image.");
			return { 0, 0 };
		}
		return Vector2D(m_GpuImage->dimensions().fWidth, m_GpuImage->dimensions().fHeight);
	}

	void SkiaImage::CreateGpuImage(const Utils::StbPixels& stbPixels, int pivotX, int pivotY, int sizeX, int sizeY)
	{
		const auto pixelsCropped = stbPixels.GetCropped(pivotX, pivotY, sizeX, sizeY);

		sk_sp<GrDirectContext> skiaContext = GetDirectContext();
		BOOST_ASSERT(skiaContext);
		QOpenGLExtraFunctions& gl = GetGlFunctions();

		GLuint textureId;
		gl.glGenTextures(1, &textureId);
		gl.glBindTexture(GL_TEXTURE_2D, textureId);

		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sizeX, sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsCropped.data());

		GrGLTextureInfo textureInfo;
		textureInfo.fID = textureId;
		textureInfo.fFormat = GL_RGBA8;
		textureInfo.fTarget = GL_TEXTURE_2D;

		auto backendTexture = GrBackendTextures::MakeGL(sizeX, sizeY, skgpu::Mipmapped::kNo, textureInfo);

		m_GpuImage = SkImages::AdoptTextureFrom(skiaContext.get(),
			backendTexture,
			GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin,
			SkColorType::kRGBA_8888_SkColorType,
			SkAlphaType::kPremul_SkAlphaType
		);
		BOOST_ASSERT(m_GpuImage);

	}

	void SkiaImage::CreateQtImage(const Utils::StbPixels& pixels, int pivotX, int pivotY, int sizeX, int sizeY)
	{
		const auto pixelsCropped = pixels.GetCropped(pivotX, pivotY, sizeX, sizeY);

		QImage image(pixelsCropped.data(), sizeX, sizeY, QImage::Format_RGBA8888);

		// Copy it into a Qt-managed QImage (to avoid dependency on STB memory)
		QImage copy = image.copy();

		m_QtImage = QPixmap::fromImage(copy);
	}
}
