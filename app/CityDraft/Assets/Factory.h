#pragma once

#include <QString>
#include <spdlog/spdlog.h>
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	class Factory
	{
	public:
		static std::shared_ptr<AssetManager> CreateAssetManager(const QString& rendererName, const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger);
	};
}
