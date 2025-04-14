#include "Factory.h"
#include "SkiaAssetManager.h"

namespace CityDraft::Assets
{
	std::shared_ptr<AssetManager> CityDraft::Assets::Factory::CreateAssetManager(const QString& rendererName, const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger)
	{
		if (rendererName == "skia")
		{
			return std::make_shared<SkiaAssetManager>(assetsRoot, logger);
		}
		return nullptr;
	}
}


