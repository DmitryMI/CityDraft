#pragma once

#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	class SkiaAssetManager : public AssetManager
	{
	public:
		SkiaAssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger);

		std::shared_ptr<Image> CreateImage(const std::filesystem::path& imageFilePath) override;

	};
}
