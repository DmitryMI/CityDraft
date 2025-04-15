#pragma once

#include "Draft.h"

namespace Assets
{
	class Asset;
}

namespace CityDraft::Drafts
{
	class Image : public Draft
	{
	public:
		using Draft::Draft;

		Vector2D GetImageSize() const;
		AxisAlignedBoundingBox2D GetAxisAlignedBoundingBox() const override;
		void OnAssetLoaded(Assets::Asset* asset, bool loadingOk) override;
	};
}
