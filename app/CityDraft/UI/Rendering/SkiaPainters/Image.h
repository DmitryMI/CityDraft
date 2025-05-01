#pragma once

#include "Asset.h"
#include "CityDraft/Assets/SkiaImage.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Image : public Asset
	{
	public:
		Image(CityDraft::Assets::SkiaImage* image, const CityDraft::Transform2D& transform);
		void Paint(CityDraft::UI::Rendering::SkiaWidget* renderer) override;

	private:
		CityDraft::Assets::SkiaImage* m_Asset;
	};
}
