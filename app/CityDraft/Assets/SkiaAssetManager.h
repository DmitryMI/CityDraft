#pragma once

#include "CityDraft/Assets/AssetManager.h"
#include <QOpenGLExtraFunctions>
#include "include/gpu/ganesh/GrDirectContext.h"
#include "CityDraft/Assets/ColorCurve.h"
#include "CityDraft/Assets/Curve.h"

namespace CityDraft::Assets
{
	class SkiaAssetManager : public AssetManager
	{
	public:
		SkiaAssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger, sk_sp<GrDirectContext> directContext, QOpenGLExtraFunctions& glFunctions);

		std::shared_ptr<Image> CreateImage(const std::filesystem::path& imageFilePath) override;
		std::shared_ptr<Curve> CreateCurve() override;
		std::shared_ptr<ColorCurve> CreateColorCurve() override;

		sk_sp<GrDirectContext> GetDirectContext() const;
		QOpenGLExtraFunctions& GetGlFunctions();

	private:
		sk_sp<GrDirectContext> m_GrDirectContext;
		QOpenGLExtraFunctions& m_GlFunc;
	};
}
