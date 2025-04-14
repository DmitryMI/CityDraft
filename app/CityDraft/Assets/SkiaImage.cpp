#include "SkiaImage.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Utils/ImageLoader.h"
#include "CityDraft/Drafts/SkiaImage.h"
#include <QOpenGLExtraFunctions>
#include "include/gpu/ganesh/GrDirectContext.h"
#include <GL/gl.h>
#include "CityDraft/Assets/SkiaAssetManager.h"
#include <include/core/SkImage.h>
#include <include/gpu/ganesh/SkImageGanesh.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/SkImageGanesh.h>
#include "CityDraft/Drafts/SkiaImage.h"

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

	AssetStatus SkiaImage::LoadAsset()
	{
		std::lock_guard lock(m_ResourceMutex);

		auto path = m_AssetManager->ToAssetPath(m_AssetUrl);
		if (!std::filesystem::is_regular_file(path))
		{
			m_Logger->error("Failed to load asset {}: path does not point to a file", m_AssetUrl.c_str());
			m_Status = AssetStatus::LoadingFailed;
			return m_Status;
		}

		m_Pixels = CityDraft::Utils::ImageLoader::LoadImage(path, 4, m_Logger);
		if (!m_Pixels.IsValid())
		{
			m_Status = AssetStatus::LoadingFailed;
			return m_Status;
		}

		m_Logger->info("Read raw pixels from {}. Width: {}, Height: {}, Channels: {}", path.string(), m_Pixels.Width, m_Pixels.Height, m_Pixels.Channels);

		sk_sp<GrDirectContext> skiaContext = GetDirectContext();
		BOOST_ASSERT(skiaContext);
		QOpenGLExtraFunctions& gl = GetGlFunctions();

		m_Logger->info("Creating OpenGL texture for {}...", path.string());

		GLuint textureId;
		gl.glGenTextures(1, &textureId);
		gl.glBindTexture(GL_TEXTURE_2D, textureId);

		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Pixels.Width, m_Pixels.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Pixels.Pixels);

		GrGLTextureInfo textureInfo;
		textureInfo.fID = textureId;
		textureInfo.fFormat = GL_RGBA8;
		textureInfo.fTarget = GL_TEXTURE_2D;

		auto backendTexture = GrBackendTextures::MakeGL(m_Pixels.Width, m_Pixels.Height, skgpu::Mipmapped::kNo, textureInfo);

		m_GpuImage = SkImages::AdoptTextureFrom(skiaContext.get(),
			backendTexture,
			GrSurfaceOrigin::kTopLeft_GrSurfaceOrigin,
			SkColorType::kRGBA_8888_SkColorType,
			SkAlphaType::kPremul_SkAlphaType
		);
		BOOST_ASSERT(m_GpuImage);

		m_Status = AssetStatus::Loaded;
		return m_Status;
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
}
