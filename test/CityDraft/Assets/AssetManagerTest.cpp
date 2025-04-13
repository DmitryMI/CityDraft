#include <gtest/gtest.h>
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	TEST(AssetManagerTest, AssetRelativePathToUrl)
	{
		std::filesystem::path path = "../../images/image.png";
		boost::url url = AssetManager::AssetRelativePathToUrl(path);
		ASSERT_STREQ(url.c_str(), "file://assets/../../images/image.png");
	}

	TEST(AssetManagerTest, ToAssetRelativePath)
	{
		boost::url url = boost::url("file://assets/../../images/image.png");
		std::filesystem::path path = AssetManager::ToAssetRelativePath(url);
		ASSERT_STREQ(path.string().c_str(), "../../images/image.png");

	}
}
