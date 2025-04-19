#pragma once

#include "Painter.h"
#include <QColor>
#include "CityDraft/Vector2D.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Circle : public Painter
	{
	public:
		Circle(const Vector2D& pos, double radius, const QColor& color, double thickness);
		void Paint(SkCanvas* canvas) override;

	private:
		Vector2D m_Center;
		double m_Radius;
		QColor m_Color;
		double m_Thickness;
	};
}
