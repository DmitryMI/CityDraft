#pragma once

#include "CityDraft/Assets/AssetManager.h"
#include <gmock/gmock.h>

namespace CityDraft::Assets
{
	class AssetManagerMock : public AssetManager
	{
		using AssetManager::AssetManager;

		MOCK_METHOD(size_t, LoadAssets, (const std::filesystem::path& assetsDir, bool createMissingDirs));
		MOCK_METHOD(Asset*, GetByUrl, (const boost::url& url));
		MOCK_METHOD(const std::list<std::shared_ptr<ImageVariantGroup>>&, GetVariantImages, ());
		MOCK_METHOD(const std::list<std::shared_ptr<Image>>&, GetInvariantImages, ());
		MOCK_METHOD(std::shared_ptr<Image>, CreateImage, (const std::filesystem::path& imageFilePath));
		MOCK_METHOD(std::shared_ptr<Curve>, CreateCurve, ());
		MOCK_METHOD(std::shared_ptr<ColorCurve>, CreateColorCurve, ());
	};
}
