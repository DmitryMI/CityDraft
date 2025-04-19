#include "Rect.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Rect::Rect(const Vector2D& min, const Vector2D& max, const QColor& color, double thickness):
		m_Min(min),
		m_Max(max),
		m_Color(color),
		m_Thickness(thickness)
	{
		
	}

	void Rect::Paint(SkCanvas* canvas)
	{

		SkColor skColor = SkColorSetARGB(m_Color.alpha(), m_Color.red(), m_Color.green(), m_Color.blue());

		auto matrix = canvas->getLocalToDeviceAs3x3();

		SkPaint paint;
		paint.setColor(skColor);
		paint.setAntiAlias(true);
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(m_Thickness);

		double rectWidth = m_Max.GetX() - m_Min.GetX();
		double rectHeight = m_Max.GetY() - m_Min.GetY();
		SkRect rect = SkRect::MakeXYWH(m_Min.GetX(), m_Min.GetY(), rectWidth, rectHeight);
		canvas->drawRect(rect, paint);
	}

}
