#pragma once

#include "CityDraft/Assets/ImageAsset.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	DraftObject* ImageAsset::CreateDraftObject()
	{
		return nullptr;
	}

	FileSystemError ImageAsset::GetImageBytes(std::vector<uint8_t>& outBytes) const
	{
		return AssetManager::ReadFileBytes(m_ImagePath, outBytes);
	}
}
