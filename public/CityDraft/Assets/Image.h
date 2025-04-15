#pragma once

#include "Asset.h"
#include <filesystem>
#include <cstdint>
#include <mutex>
#include "CityDraft/Assets/FileSystemError.h"
#include "CityDraft/Utils/ImageLoader.h"
#include "CityDraft/Vector2D.h"

namespace CityDraft::Assets
{
	class AssetManager;

	class Image : public Asset
	{
	public:

		Image(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		virtual FileSystemError GetImageBytes(std::vector<uint8_t>& outBytes) const;
		virtual Vector2D GetImageSize() const = 0;
	};
}
