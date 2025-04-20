#include "Rect.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Rect::Rect(
		const Vector2D& min,
		const Vector2D& max,
		const QColor& outlineColor,
		double outlineThickness,
		const QColor& fillColor
	):
		m_Min(min),
		m_Max(max),
		m_OutlineColor(outlineColor),
		m_OutlineThickness(outlineThickness),
		m_FillColor(fillColor),
		m_Outline(true),
		m_Fill(true)
	{
		
	}

	Rect::Rect(
		const Vector2D& min,
		const Vector2D& max,
		const QColor& outlineColor,
		double outlineThickness
	):
		m_Min(min),
		m_Max(max),
		m_OutlineColor(outlineColor),
		m_OutlineThickness(outlineThickness),
		m_FillColor(QColor(0, 0, 0, 0)),
		m_Outline(true),
		m_Fill(false)
	{

	}

	Rect::Rect(const Vector2D& min, const Vector2D& max, const QColor& fillColor):
		m_Min(min),
		m_Max(max),
		m_OutlineColor(QColor(0, 0, 0, 0)),
		m_OutlineThickness(0),
		m_FillColor(fillColor),
		m_Outline(false),
		m_Fill(true)
	{
	}

	void Rect::Paint(SkCanvas* canvas)
	{
		double rectWidth = m_Max.GetX() - m_Min.GetX();
		double rectHeight = m_Max.GetY() - m_Min.GetY();
		SkRect rect = SkRect::MakeXYWH(m_Min.GetX(), m_Min.GetY(), rectWidth, rectHeight);

		if (m_Fill)
		{
			SkColor skFillColor = SkColorSetARGB(m_FillColor.alpha(), m_FillColor.red(), m_FillColor.green(), m_FillColor.blue());
			SkPaint paint;
			paint.setColor(skFillColor);
			paint.setAntiAlias(true);
			paint.setStyle(SkPaint::kFill_Style);
			canvas->drawRect(rect, paint);
		}

		if (m_Outline)
		{
			SkColor skOutlineColor = SkColorSetARGB(m_OutlineColor.alpha(), m_OutlineColor.red(), m_OutlineColor.green(), m_OutlineColor.blue());
			SkPaint paint;
			paint.setColor(skOutlineColor);
			paint.setAntiAlias(true);
			paint.setStyle(SkPaint::kStroke_Style);
			paint.setStrokeWidth(m_OutlineThickness);
			canvas->drawRect(rect, paint);
		}		
	}

}
