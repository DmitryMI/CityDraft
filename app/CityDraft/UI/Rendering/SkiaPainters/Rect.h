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
		Rect(
			const Vector2D& min,
			const Vector2D& max,
			const QColor& outlineColor,
			double outlineThickness,
			const QColor& fillColor
		);

		Rect(
			const Vector2D& min,
			const Vector2D& max,
			const QColor& outlineColor,
			double outlineThickness
		);

		Rect(
			const Vector2D& min,
			const Vector2D& max,
			const QColor& fillColor
		);

		void Paint(SkCanvas* canvas) override;

	private:
		Vector2D m_Min;
		Vector2D m_Max;
		QColor m_OutlineColor;
		double m_OutlineThickness;
		QColor m_FillColor;

		bool m_Outline;
		bool m_Fill;
	};
}
