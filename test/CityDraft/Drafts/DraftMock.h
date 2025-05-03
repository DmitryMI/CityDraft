#pragma once

#include "CityDraft/Drafts/Draft.h"
#include <gmock/gmock.h>

namespace CityDraft::Drafts
{
	class DraftMock: public Draft
	{
		using Draft::Draft;

		MOCK_METHOD(bool, IsPointInside, (const Vector2D& point), (override));
		MOCK_METHOD(AxisAlignedBoundingBox2D, GetAxisAlignedBoundingBox, (), (const, override));
		MOCK_METHOD(void, OnAssetLoaded, (Assets::Asset* asset, bool loadingOk), (override));
		MOCK_METHOD(void, SetTranslation, (const Vector2D& translation), (override));
		MOCK_METHOD(void, SetTransform, (const Transform2D& transform), (override));
	};
}
