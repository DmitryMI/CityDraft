#pragma once

#include "CityDraft/Assets/ImageAsset.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	ImageAsset::ImageAsset(const std::filesystem::path& path) : Asset(AssetManager::AssetRelativePathToUrl(path))
	{

	}

	DraftObject* ImageAsset::CreateDraftObject()
	{
		return nullptr;
	}

	FileSystemError ImageAsset::GetImageBytes(std::vector<uint8_t>& outBytes) const
	{
		return AssetManager::ReadFileBytes(AssetManager::ToAssetRelativePath(m_AssetUrl), outBytes);
	}
}
