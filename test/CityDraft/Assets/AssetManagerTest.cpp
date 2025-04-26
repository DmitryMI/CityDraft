#include <gtest/gtest.h>
#include "CityDraft/Assets/AssetManager.h"
#include "AssetManagerMock.h"
#include "../LogManager.h"

namespace CityDraft::Assets
{
	TEST(AssetManagerTest, AssetRelativePathToUrl)
	{
		std::shared_ptr<spdlog::logger> logger = CityDraft::Logging::LogManager::CreateLogger("AssetManagerTest");

		AssetManagerMock assetManager(std::filesystem::path("C:/Default"), logger);

		std::filesystem::path path = "C:/Default/images/image.png";
		boost::url url = assetManager.AssetPathToUrl(path);
		ASSERT_STREQ(url.c_str(), "file://assets/images/image.png");
	}

	TEST(AssetManagerTest, ToAssetRelativePath)
	{
		std::shared_ptr<spdlog::logger> logger = CityDraft::Logging::LogManager::CreateLogger("AssetManagerTest");
		AssetManagerMock assetManager(std::filesystem::path("C:/Default"), logger);

		boost::url url = boost::url("file://assets/../images/image.png");

		// std::unique_ptr<AssetManager>

		std::filesystem::path path = assetManager.ToAssetPath(url);
		ASSERT_STREQ(path.string().c_str(), "C:\\images\\image.png");

	}
}
