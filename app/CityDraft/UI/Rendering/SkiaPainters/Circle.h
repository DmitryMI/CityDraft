#pragma once

#include "Painter.h"
#include <QColor>
#include "CityDraft/Vector2D.h"
#include "CityDraft/LinearColor.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Circle : public Painter
	{
	public:
		Circle(const Vector2D& pos, double radius, const LinearColorF& color, double thickness);
		void Paint(CityDraft::UI::Rendering::SkiaWidget* renderer) override;

	private:
		Vector2D m_Center;
		double m_Radius;
		LinearColorF m_Color;
		double m_Thickness;
	};
}
