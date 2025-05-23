#pragma once

#include "Asset.h"
#include "CityDraft/Curves/ICurve.h"
#include "CityDraft/Curves/IWidthProvider.h"
#include "CityDraft/LinearColor.h"
#include "CityDraft/Drafts/SkiaColorCurve.h"
#include <include/core/SkCanvas.h>

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class ColorCurve: public Asset
	{
	public:
		constexpr static size_t PixelsPerSegment = 10;

		ColorCurve(
			CityDraft::Curves::ICurve* curve,
			CityDraft::Curves::IWidthProvider* fillWidth,
			CityDraft::Curves::IWidthProvider* outlineWidth,
			const LinearColorF& fillColor,
			const LinearColorF& outlineColor
		);

		ColorCurve(CityDraft::Drafts::SkiaColorCurve* draft);

		void Paint(CityDraft::UI::Rendering::SkiaWidget* renderer) override;

	private:
		CityDraft::Curves::ICurve* m_Curve = nullptr;
		CityDraft::Curves::IWidthProvider* m_FillWidth = nullptr;
		CityDraft::Curves::IWidthProvider* m_OutlineWidth = nullptr;
		LinearColorF m_FillColor{};
		LinearColorF m_OutlineColor{};

		void PaintFill(CityDraft::UI::Rendering::SkiaWidget* renderer);
		void PaintOutline(CityDraft::UI::Rendering::SkiaWidget* renderer);
		void PaintCurve(SkCanvas* canvas, SkPaint paint, CityDraft::Curves::IWidthProvider* widthA, CityDraft::Curves::IWidthProvider* widthB);
	
		void DebugDumpImage(CityDraft::UI::Rendering::SkiaWidget* renderer, sk_sp<SkImage> image);
	};
}
