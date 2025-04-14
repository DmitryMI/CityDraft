#include "SkiaAssetManager.h"
#include "SkiaImage.h"
#include "CityDraft/Logging/LogManager.h"

namespace CityDraft::Assets
{
	SkiaAssetManager::SkiaAssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger):
		AssetManager(assetsRoot, logger)
	{
	}

	std::shared_ptr<Image> SkiaAssetManager::CreateImage(const std::filesystem::path& imageFilePath)
	{
		auto logger = CityDraft::Logging::LogManager::CreateLogger("SkiaImage");
		return std::make_shared<SkiaImage>(imageFilePath, this, logger);
	}

}
