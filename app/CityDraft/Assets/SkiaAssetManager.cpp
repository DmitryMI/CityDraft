#include "SkiaAssetManager.h"
#include "SkiaImage.h"
#include "CityDraft/Logging/LogManager.h"

namespace CityDraft::Assets
{
	SkiaAssetManager::SkiaAssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger, sk_sp<GrDirectContext> directContext, QOpenGLExtraFunctions& glFunctions):
		AssetManager(assetsRoot, logger),
		m_GrDirectContext(directContext),
		m_GlFunc(glFunctions)
	{
		BOOST_ASSERT(m_GrDirectContext);
	}

	std::shared_ptr<Image> SkiaAssetManager::CreateImage(const std::filesystem::path& imageFilePath)
	{
		auto logger = CityDraft::Logging::LogManager::CreateLogger("SkiaImage");
		return std::make_shared<SkiaImage>(imageFilePath, this, logger);
	}

	sk_sp<GrDirectContext> SkiaAssetManager::GetDirectContext() const
	{
		return m_GrDirectContext;
	}

	QOpenGLExtraFunctions& SkiaAssetManager::GetGlFunctions()
	{
		return m_GlFunc;
	}

}
