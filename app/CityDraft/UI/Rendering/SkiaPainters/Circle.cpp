#include "Circle.h"
#include "CityDraft/UI/Colors/Utils.h"
#include "CityDraft/UI/Rendering/SkiaWidget.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Circle::Circle(const Vector2D& pos, double radius, const LinearColorF& color, double thickness):
		m_Center(pos),
		m_Radius(radius),
		m_Color(color),
		m_Thickness(thickness)
	{
		
	}

	void Circle::Paint(CityDraft::UI::Rendering::SkiaWidget* renderer)
	{
		auto* canvas = renderer->GetPrimaryCanvas();
		SkColor skColor = CityDraft::UI::Colors::Utils::ToSkColor(m_Color);

		auto matrix = canvas->getLocalToDeviceAs3x3();

		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(skColor);
		paint.setStyle(SkPaint::kStroke_Style);
		paint.setStrokeWidth(m_Thickness);

		canvas->drawCircle(m_Center.GetX(), m_Center.GetY(), m_Radius, paint);
	}

}
