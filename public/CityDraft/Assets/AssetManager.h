#pragma once

#include "Asset.h"
#include "Image.h"
#include "ImageVariantGroup.h"
#include <cstdint>
#include <filesystem>
#include <list>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>
#include <string_view>
#include "FileSystemError.h"
#include "CityDraft/Assets/Curve.h"
#include "CityDraft/Assets/ColorCurve.h"

namespace CityDraft::Assets
{
	class AssetManager
	{
	public:
		constexpr static std::string_view ImagesDir = "images";
		constexpr static std::string_view ColorCurveUrl = "app://assets/curves/color";

		AssetManager(const std::filesystem::path& assetsRoot, std::shared_ptr<spdlog::logger> logger);

		size_t LoadAssetInfos(const std::filesystem::path& assetsDir, bool createMissingDirs);

		std::shared_ptr<Asset> GetByUrl(const boost::url& url);
		std::shared_ptr<Asset> GetByUrl(const std::string& url);

		const std::list<std::shared_ptr<ImageVariantGroup>>& GetVariantImages() const;
		const std::list<std::shared_ptr<Image>>& GetInvariantImages() const;
		const std::list<std::shared_ptr<Curve>>& GetCurves() const;
		std::shared_ptr<ColorCurve> GetColorCurve() const;

		boost::url AssetPathToUrl(const std::filesystem::path& path);
		std::filesystem::path ToAssetPath(const boost::url& url);

		virtual std::shared_ptr<Image> CreateImage(const std::filesystem::path& imageFilePath) = 0;
		virtual std::shared_ptr<Curve> CreateCurve() = 0;
		virtual std::shared_ptr<ColorCurve> CreateColorCurve() = 0;

		static FileSystemError ReadFileBytes(const std::filesystem::path& filePath, std::vector<uint8_t>& outBytes);

	private:
		std::shared_ptr<spdlog::logger> m_Logger;
		std::filesystem::path m_AssetsRoot;

		std::list<std::shared_ptr<ImageVariantGroup>> m_VariantImages;
		std::list<std::shared_ptr<Image>> m_InvariantImages;
		std::list<std::shared_ptr<Curve>> m_Curves;
		std::shared_ptr<ColorCurve> m_ColorCurve;

		size_t LoadImageInfos(const std::filesystem::path& imagesDir);
		size_t LoadVariantImageGroupInfo(const std::filesystem::path& variantDir);
	};
}
