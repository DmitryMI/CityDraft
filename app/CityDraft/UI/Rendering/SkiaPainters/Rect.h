#pragma once

#include "Painter.h"
#include <QPointF>
#include <CityDraft/LinearColor.h>
#include "CityDraft/Vector2D.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Rect : public Painter
	{
	public:
		Rect(
			const Vector2D& min,
			const Vector2D& max,
			const LinearColorF& outlineColor,
			double outlineThickness,
			const LinearColorF& fillColor
		);

		Rect(
			const Vector2D& min,
			const Vector2D& max,
			const LinearColorF& outlineColor,
			double outlineThickness
		);

		Rect(
			const Vector2D& min,
			const Vector2D& max,
			const LinearColorF& fillColor
		);

		void Paint(SkCanvas* canvas) override;

	private:
		Vector2D m_Min;
		Vector2D m_Max;
		LinearColorF m_OutlineColor;
		double m_OutlineThickness;
		LinearColorF m_FillColor;

		bool m_Outline;
		bool m_Fill;
	};
}
