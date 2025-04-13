#include <gtest/gtest.h>
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	TEST(AssetManagerTest, PathToUrl)
	{
		std::filesystem::path path = "../../images/image.png";
		boost::url url = AssetManager::ToUrl(path);
		ASSERT_STREQ(url.c_str(), "file://../../images/image.png");
		
	}


}
