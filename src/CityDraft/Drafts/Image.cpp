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
		const Transform2D& transform = GetTransform();

		double halfWidth = 0.5 * imageSize.GetX() * transform.Scale.GetX();
		double halfHeight = 0.5 * imageSize.GetY() * transform.Scale.GetY();

		std::array<Vector2D, 4> corners =
		{
			Vector2D{-halfWidth, -halfHeight},
			Vector2D{ halfWidth, -halfHeight},
			Vector2D{ halfWidth,  halfHeight},
			Vector2D{-halfWidth,  halfHeight}
		};

		double cosA = std::cos(transform.Rotation);
		double sinA = std::sin(transform.Rotation);

		Vector2D min = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };
		Vector2D max = { std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest() };

		for (const Vector2D& corner : corners)
		{
			double rotatedX = corner.GetX() * cosA - corner.GetY() * sinA;
			double rotatedY = corner.GetX() * sinA + corner.GetY() * cosA;

			double worldX = rotatedX + transform.Translation.GetX();
			double worldY = rotatedY + transform.Translation.GetY();

			min = Vector2D{
				std::min(min.GetX(), worldX),
				std::min(min.GetY(), worldY)
			};
			max = Vector2D{
				std::max(max.GetX(), worldX),
				std::max(max.GetY(), worldY)
			};
		}

		return AxisAlignedBoundingBox2D(min, max);
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
