#pragma once

#include "Asset.h"
#include "CityDraft/Curves/ICurve.h"
#include "CityDraft/Curves/IWidthProvider.h"
#include "CityDraft/LinearColor.h"
#include "CityDraft/Drafts/SkiaColorCurve.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class ColorCurve: public Asset
	{
	public:
		ColorCurve(
			CityDraft::Curves::ICurve* curve,
			CityDraft::Curves::IWidthProvider* fillWidth,
			CityDraft::Curves::IWidthProvider* outlineWidth,
			const LinearColorF& fillColor,
			const LinearColorF& outlineColor
		);

		ColorCurve(
			CityDraft::Drafts::SkiaColorCurve* draft
		);

		void Paint(SkCanvas* canvas) override;

	private:
		CityDraft::Curves::ICurve* m_Curve = nullptr;
		CityDraft::Curves::IWidthProvider* m_FillWidth = nullptr;
		CityDraft::Curves::IWidthProvider* m_OutlineWidth = nullptr;
		LinearColorF m_FillColor{};
		LinearColorF m_OutlineColor{};
	};
}
