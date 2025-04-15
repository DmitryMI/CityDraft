#pragma once

#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	Image::Image(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger) :
		Asset(path, assetManager, logger)
	{

	}

	FileSystemError Image::GetImageBytes(std::vector<uint8_t>& outBytes) const
	{
		return AssetManager::ReadFileBytes(m_AssetManager->ToAssetPath(m_AssetUrl), outBytes);
	}
}
