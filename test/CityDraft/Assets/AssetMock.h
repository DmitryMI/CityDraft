#pragma once

#include "CityDraft/Assets/Asset.h"
#include <gmock/gmock.h>

namespace CityDraft::Assets
{
	class AssetMock: public Asset
	{
		using Asset::Asset;

		MOCK_METHOD(std::shared_ptr<Drafts::Draft>, CreateDraft, (), (override));
		MOCK_METHOD(AssetStatus, GetStatus, (), (const, override));
		MOCK_METHOD(bool, IsValid, (), (const, override));
		MOCK_METHOD(void, LoadAssetInternal, (), (override));
	};
}
