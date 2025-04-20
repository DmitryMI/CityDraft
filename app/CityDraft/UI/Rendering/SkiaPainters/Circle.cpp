#include "Circle.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Circle::Circle(const Vector2D& pos, double radius, const QColor& color, double thickness):
		m_Center(pos),
		m_Radius(radius),
		m_Color(color),
		m_Thickness(thickness)
	{
		
	}

	void Circle::Paint(SkCanvas* canvas)
	{
		SkColor skColor = SkColorSetARGB(m_Color.alpha(), m_Color.red(), m_Color.green(), m_Color.blue());

		auto matrix = canvas->getLocalToDeviceAs3x3();

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(skColor);
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(m_Thickness);

		canvas->drawCircle(m_Center.GetX(), m_Center.GetY(), m_Radius, paint);
	}

}
