#pragma once

#include "Asset.h"
#include "ImageAsset.h"
#include "ImageVariantGroup.h"
#include <cstdint>
#include <filesystem>
#include <list>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>
#include "FileSystemError.h"

namespace CityDraft::Assets
{
	class AssetManager
	{
	public:
		static FileSystemError ReadFileBytes(const std::filesystem::path& filePath, std::vector<uint8_t>& outBytes);
	private:
		std::list<std::shared_ptr<ImageVariantGroup>> m_VariantImages;
		std::list<std::shared_ptr<ImageAsset>> m_InvariantImages;
	};
}
