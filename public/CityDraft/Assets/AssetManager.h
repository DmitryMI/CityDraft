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
#include <string_view>
#include "FileSystemError.h"

namespace CityDraft::Assets
{
	class AssetManager
	{
	public:
		constexpr static std::string_view ImagesDir = "images";

		AssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger);

		size_t LoadAssets(const std::filesystem::path& assetsDir, bool createMissingDirs);

		Asset* GetByUrl(const boost::url& url);

		const std::list<std::shared_ptr<ImageVariantGroup>>& GetVariantImages() const;
		const std::list<std::shared_ptr<ImageAsset>>& GetInvariantImages() const;

		static FileSystemError ReadFileBytes(const std::filesystem::path& filePath, std::vector<uint8_t>& outBytes);

		static boost::url AssetRelativePathToUrl(const std::filesystem::path& path);
		static std::filesystem::path ToAssetRelativePath(const boost::url& url);

	private:
		std::shared_ptr<spdlog::logger> m_Logger;
		std::filesystem::path m_AssetsRoot;

		std::list<std::shared_ptr<ImageVariantGroup>> m_VariantImages;
		std::list<std::shared_ptr<ImageAsset>> m_InvariantImages;

		size_t LoadImageAssets(const std::filesystem::path& imagesDir);
		size_t LoadVariantImageGroup(const std::filesystem::path& variantDir);
	};
}
