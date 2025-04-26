#include "ColorCurve.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{

	ColorCurve::ColorCurve(
		CityDraft::Curves::ICurve* curve,
		CityDraft::Curves::IWidthProvider* fillWidth,
		CityDraft::Curves::IWidthProvider* outlineWidth,
		const QColor& fillColor,
		const QColor& outlineColor,
		const CityDraft::Transform2D& transform
	):
		m_Curve(curve),
		m_FillWidth(fillWidth),
		m_OutlineWidth(outlineWidth),
		m_FillColor(fillColor),
		m_OutlineColor(outlineColor),
		Asset(transform)
	{
	}

	void ColorCurve::Paint(SkCanvas* canvas)
	{

	}

}