#pragma once

#include "CityDraft/Assets/Image.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include <QOpenGLExtraFunctions>
#include <include/core/SkImage.h>

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

		AssetStatus LoadAsset() override;

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

		sk_sp<SkImage> GetGpuImage() const;
	private:
		sk_sp<SkImage> m_GpuImage;
	};
}
