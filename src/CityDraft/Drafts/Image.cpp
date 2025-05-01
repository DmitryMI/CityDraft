#include "CityDraft/Drafts/Image.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Scene.h"

namespace CityDraft::Drafts
{
	Vector2D Image::GetImageOriginalSize() const
	{
		BOOST_ASSERT(GetAsset());
		CityDraft::Assets::Image* image = dynamic_cast<CityDraft::Assets::Image*>(GetAsset());
		BOOST_ASSERT(image);
		BOOST_ASSERT(image->IsValid());

		return image->GetImageSize();
	}

	AxisAlignedBoundingBox2D Image::GetAxisAlignedBoundingBox() const
	{
		Vector2D imageSize = GetImageOriginalSize();
		double halfWidth = 0.5 * imageSize.GetX();
		double halfHeight = 0.5 * imageSize.GetY();

		AxisAlignedBoundingBox2D imageBox({-halfWidth, -halfHeight}, {halfWidth, halfHeight});

		const Transform2D& transform = GetTransform();

		return imageBox.Transform(transform);
	}


	void Image::OnAssetLoaded(Assets::Asset* asset, bool loadingOk)
	{
		if (!loadingOk)
		{
			return;
		}

		BOOST_ASSERT(GetScene());
		
		GetScene()->UpdateDraftModel(this);
	}
}
