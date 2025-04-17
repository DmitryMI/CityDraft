#pragma once

#include "CityDraft/Assets/Image.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include <QOpenGLExtraFunctions>
#include <include/core/SkImage.h>
#include "CityDraft/Utils/ImageLoader.h"
#include <QPixmap>

namespace CityDraft::Drafts
{
	class Draft;
}

namespace CityDraft::Assets
{
	class SkiaImage : public Image
	{
	public:
		SkiaImage(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		std::shared_ptr<Drafts::Draft> CreateDraft() override;

		void LoadAssetInternal() override;

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

		sk_sp<SkImage> GetGpuImage() const;
		const QPixmap& GetPixmap() const;
		Vector2D GetImageSize() const override;
	private:
		sk_sp<SkImage> m_GpuImage;
		QPixmap m_QtImage;

		void CreateGpuImage(const Utils::StbPixels& stbPixels);
		void CreateQtImage(const Utils::StbPixels& stbPixels);
	};
}
