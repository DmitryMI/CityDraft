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
		Vector2D imageSizeScaled = Vector2D(
			imageSize.GetX() * GetTransform().Scale.GetX(),
			imageSize.GetY() * GetTransform().Scale.GetY()
		);
		Vector2D translation = GetTranslation();
		return AxisAlignedBoundingBox2D(translation - imageSizeScaled * 0.5, translation + imageSizeScaled * 0.5);
	}


	void Image::OnAssetLoaded(Assets::Asset* asset, bool loadingOk)
	{
		if (!loadingOk)
		{
			return;
		}

		BOOST_ASSERT(GetScene());
		
		GetScene()->UpdateObjectModel(this);
	}
}
