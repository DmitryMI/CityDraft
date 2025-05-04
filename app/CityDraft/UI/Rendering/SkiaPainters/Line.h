#pragma once

#include "Painter.h"
#include "CityDraft/Vector2D.h"
#include "CityDraft/LinearColor.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	class Line: public Painter
	{
	public:
		Line(const Vector2D& from, const Vector2D& to, const LinearColorF& color, double thickness);
		void Paint(CityDraft::UI::Rendering::SkiaWidget* renderer) override;

	private:
		Vector2D m_Start;
		Vector2D m_End;
		LinearColorF m_Color;
		double m_Thickness;
	};
}
