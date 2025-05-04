#include "Line.h"
#include "CityDraft/UI/Colors/Utils.h"
#include "CityDraft/UI/Rendering/SkiaWidget.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Line::Line(const Vector2D& from, const Vector2D& to, const LinearColorF& color, double thickness):
		m_Start(from),
		m_End(to),
		m_Color(color),
		m_Thickness(thickness)
	{

	}

	void Line::Paint(CityDraft::UI::Rendering::SkiaWidget* renderer)
	{
		auto* canvas = renderer->GetPrimaryCanvas();
		SkColor skColor = CityDraft::UI::Colors::Utils::ToSkColor(m_Color);

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(skColor);
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(m_Thickness);
		canvas->drawLine(m_Start.GetX(), m_Start.GetY(), m_End.GetX(), m_End.GetY(), paint);
	}

}
