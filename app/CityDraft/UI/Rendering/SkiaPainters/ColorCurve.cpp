#include "ColorCurve.h"
#include "CityDraft/Drafts/Curve.h"
#include <QColor>

namespace CityDraft::UI::Rendering::SkiaPainters
{

	ColorCurve::ColorCurve(
		CityDraft::Curves::ICurve* curve,
		CityDraft::Curves::IWidthProvider* fillWidth,
		CityDraft::Curves::IWidthProvider* outlineWidth,
		const LinearColorF& fillColor,
		const LinearColorF& outlineColor
	):
		m_Curve(curve),
		m_FillWidth(fillWidth),
		m_OutlineWidth(outlineWidth),
		m_FillColor(fillColor),
		m_OutlineColor(outlineColor),
		Asset(Transform2D{})
	{

	}

	ColorCurve::ColorCurve(CityDraft::Drafts::SkiaColorCurve* draft):
		Asset(Transform2D{})
	{
		BOOST_ASSERT(draft);
		SetOwner(draft);
	}

	void ColorCurve::Paint(SkCanvas* canvas)
	{
		if(GetOwner())
		{
			auto owner = dynamic_cast<CityDraft::Drafts::Curve*>(GetOwner());
			BOOST_ASSERT(owner);

			m_Curve = owner->GetCurve().get();
			m_FillWidth = owner->GetFillWidth().get();
			m_OutlineWidth = owner->GetOutlineWidth().get();
			m_FillColor = owner->GetFillColor();
			m_OutlineColor = owner->GetOutlineColor();
		}

		BOOST_ASSERT(m_Curve);
		BOOST_ASSERT(m_FillWidth);
		BOOST_ASSERT(m_OutlineWidth);

		for(size_t i = 0; i <= 100; i++)
		{
			double t = i / 100.0;
			Vector2D point = m_Curve->GetPoint(t);

			SkColor skColor = SkColorSetARGB(
				m_FillColor.Alpha<uint8_t>(),
				m_FillColor.Red<uint8_t>(),
				m_FillColor.Green<uint8_t>(),
				m_FillColor.Blue<uint8_t>()
			);

			SkPaint paint;
			paint.setAntiAlias(true);
			paint.setColor(skColor);
			paint.setStyle(SkPaint::kFill_Style);

			canvas->drawCircle(point.GetX(), point.GetY(), m_FillWidth->GetWidth(m_Curve->GetLength(t)), paint);
		}
	}

}