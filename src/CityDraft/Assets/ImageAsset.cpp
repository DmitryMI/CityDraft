#pragma once

#include "CityDraft/Assets/ImageAsset.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	ImageAsset::ImageAsset(const std::filesystem::path& path) : Asset(AssetManager::ToUrl(path))
	{

	}

	DraftObject* ImageAsset::CreateDraftObject()
	{
		return nullptr;
	}

	FileSystemError ImageAsset::GetImageBytes(std::vector<uint8_t>& outBytes) const
	{
		return AssetManager::ReadFileBytes(AssetManager::ToPath(m_AssetUrl), outBytes);
	}
}
