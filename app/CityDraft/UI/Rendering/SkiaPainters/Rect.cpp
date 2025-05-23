#include "Rect.h"
#include "CityDraft/UI/Colors/Utils.h"
#include "CityDraft/UI/Rendering/SkiaWidget.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{
	Rect::Rect(
		const Vector2D& min,
		const Vector2D& max,
		const LinearColorF& outlineColor,
		double outlineThickness,
		const LinearColorF& fillColor
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
		const LinearColorF& outlineColor,
		double outlineThickness
	):
		m_Min(min),
		m_Max(max),
		m_OutlineColor(outlineColor),
		m_OutlineThickness(outlineThickness),
		m_FillColor(LinearColorF(0, 0, 0, 0)),
		m_Outline(true),
		m_Fill(false)
	{

	}

	Rect::Rect(const Vector2D& min, const Vector2D& max, const LinearColorF& fillColor):
		m_Min(min),
		m_Max(max),
		m_OutlineColor(),
		m_OutlineThickness(0),
		m_FillColor(fillColor),
		m_Outline(false),
		m_Fill(true)
	{
	}

	void Rect::Paint(CityDraft::UI::Rendering::SkiaWidget* renderer)
	{
		SkCanvas* canvas = renderer->GetPrimaryCanvas();
		double rectWidth = m_Max.GetX() - m_Min.GetX();
		double rectHeight = m_Max.GetY() - m_Min.GetY();
		SkRect rect = SkRect::MakeXYWH(m_Min.GetX(), m_Min.GetY(), rectWidth, rectHeight);

		if (m_Fill)
		{
			SkColor skFillColor = CityDraft::UI::Colors::Utils::ToSkColor(m_FillColor);
			SkPaint paint;
			paint.setColor(skFillColor);
			paint.setAntiAlias(true);
			paint.setStyle(SkPaint::kFill_Style);
			canvas->drawRect(rect, paint);
		}

		if (m_Outline)
		{
			SkColor skOutlineColor = CityDraft::UI::Colors::Utils::ToSkColor(m_OutlineColor);
			SkPaint paint;
			paint.setColor(skOutlineColor);
			paint.setAntiAlias(true);
			paint.setStyle(SkPaint::kStroke_Style);
			paint.setStrokeWidth(m_OutlineThickness);
			canvas->drawRect(rect, paint);
		}		
	}

}
