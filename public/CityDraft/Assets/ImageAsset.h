#pragma once

#include "Asset.h"
#include <filesystem>
#include <cstdint>
#include "CityDraft/Assets/FileSystemError.h"

namespace CityDraft::Assets
{
	class AssetManager;

	class ImageAsset : public Asset
	{
	public:
		// ImageAsset() = default;

		ImageAsset(const std::filesystem::path& path);

		DraftObject* CreateDraftObject() override;

		FileSystemError GetImageBytes(std::vector<uint8_t>& outBytes) const;

		inline AssetStatus GetStatus() const override
		{
			return m_Status;
		}

	private:
		AssetStatus m_Status{ AssetStatus::Initialized};
	};
}
