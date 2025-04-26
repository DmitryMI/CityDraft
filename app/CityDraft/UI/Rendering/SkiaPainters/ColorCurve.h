#pragma once

#include "Asset.h"
#include "CityDraft/Curves/ICurve.h"
#include "CityDraft/Curves/IWidthProvider.h"
#include <QColor>

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class ColorCurve: public Asset
	{
	public:
		ColorCurve(
			CityDraft::Curves::ICurve* curve,
			CityDraft::Curves::IWidthProvider* fillWidth,
			CityDraft::Curves::IWidthProvider* outlineWidth,
			const QColor& fillColor,
			const QColor& outlineColor,
			const CityDraft::Transform2D& transform
		);

		void Paint(SkCanvas* canvas) override;

	private:
		CityDraft::Curves::ICurve* m_Curve = nullptr;
		CityDraft::Curves::IWidthProvider* m_FillWidth = nullptr;
		CityDraft::Curves::IWidthProvider* m_OutlineWidth = nullptr;
		QColor m_FillColor;
		QColor m_OutlineColor;
	};
}
