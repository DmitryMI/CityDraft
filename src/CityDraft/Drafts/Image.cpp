#include "CityDraft/Drafts/Image.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/Asset.h"

namespace CityDraft::Drafts
{
	AxisAlignedBoundingBox2D Image::GetAxisAlignedBoundingBox() const
	{
		BOOST_ASSERT(GetAsset());
		CityDraft::Assets::Image* Image = dynamic_cast<CityDraft::Assets::Image*>(GetAsset());
		BOOST_ASSERT(Image);
		BOOST_ASSERT(Image->IsValid());

		Vector2D imageSize = Image->GetImageSize();
		Vector2D imageSizeScaled = GetTransform().Scale;
		Vector2D translation = GetTranslation();
		return AxisAlignedBoundingBox2D(translation - imageSizeScaled * 0.5, translation + imageSizeScaled * 0.5);
	}
}
