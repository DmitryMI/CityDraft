#pragma once

#include "Image.h"
#include "CityDraft/Assets/SkiaImage.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Image::Image(CityDraft::Assets::SkiaImage* image, const CityDraft::Transform2D& transform) :
		Asset(transform),
		m_Asset(image)
	{
	}

	void Image::Paint(SkCanvas* canvas)
	{
		canvas->save();
		canvas->translate(m_Transform.Translation.GetX(), m_Transform.Translation.GetY());
		canvas->scale(m_Transform.Scale.GetX(), m_Transform.Scale.GetY());

		BOOST_ASSERT(m_Asset);
		// TODO don't crash on assets with failed resources
		if (m_Asset->GetStatus() == Assets::AssetStatus::Initialized)
		{
			m_Asset->LoadAsset();
		}
		BOOST_ASSERT(m_Asset->GetStatus() == Assets::AssetStatus::Loaded);

		auto skImage = m_Asset->GetGpuImage();

		Vector2D imageSize = m_Asset->GetImageSize();

		SkRect destRect = SkRect::MakeXYWH(
			-imageSize.GetX() / 2.0f,
			-imageSize.GetY() / 2.0f,
			imageSize.GetX(),
			imageSize.GetY()
		);
		canvas->drawImageRect(skImage, destRect, SkSamplingOptions());
		canvas->restore();
	}
}
