#pragma once

#include "Asset.h"
#include <filesystem>
#include <cstdint>
#include "CityDraft/Assets/FileSystemError.h"

namespace CityDraft::Assets
{
	class ImageAsset : public Asset
	{
	public:
		DraftObject* CreateDraftObject() override;

		FileSystemError GetImageBytes(std::vector<uint8_t>& outBytes) const;
	private:
		std::filesystem::path m_ImagePath;
	};
}
