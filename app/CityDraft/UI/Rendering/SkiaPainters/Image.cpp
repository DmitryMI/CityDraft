#pragma once

#include "Image.h"
#include "CityDraft/Assets/SkiaImage.h"
#include "CityDraft/UI/Rendering/SkiaWidget.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Image::Image(CityDraft::Assets::SkiaImage* image, const CityDraft::Transform2D& transform) :
		Asset(transform),
		m_Asset(image)
	{
	}

	void Image::Paint(CityDraft::UI::Rendering::SkiaWidget* renderer)
	{
		if(m_Owner)
		{
			m_Transform = m_Owner->GetTransform();
		}

		SkCanvas* canvas = renderer->GetPrimaryCanvas();

		canvas->save();
		canvas->translate(m_Transform.Translation.GetX(), m_Transform.Translation.GetY());
		canvas->scale(m_Transform.Scale.GetX(), m_Transform.Scale.GetY());
		canvas->rotate(Degrees(m_Transform.Rotation).Value);

		BOOST_ASSERT(m_Asset);
		// TODO don't crash on assets with failed resources
		if (m_Asset->GetStatus() == Assets::AssetStatus::Initialized)
		{
			m_Asset->LoadAsset();
		}
		BOOST_ASSERT(m_Asset->GetStatus() == Assets::AssetStatus::Loaded);

		auto skImage = m_Asset->GetGpuImage();

		Vector2D imageSize = m_Asset->GetImageSize();
		Vector2D pivot = m_Asset->GetPivotPoint();
		SkRect destRect = SkRect::MakeXYWH(
			-pivot.GetX(),
			-pivot.GetY(),
			imageSize.GetX(),
			imageSize.GetY()
		);
		canvas->drawImageRect(skImage, destRect, SkSamplingOptions());
		canvas->restore();
	}
}
