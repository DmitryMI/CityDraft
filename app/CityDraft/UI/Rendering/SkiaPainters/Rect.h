#pragma once

#include "Painter.h"
#include <QPointF>
#include <QColor>
#include "CityDraft/Vector2D.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Rect : public Painter
	{
	public:
		Rect(const Vector2D& min, const Vector2D& max, const QColor& color, double thickness);
		void Paint(SkCanvas* canvas) override;

	private:
		Vector2D m_Min;
		Vector2D m_Max;
		QColor m_Color;
		double m_Thickness;
	};
}
