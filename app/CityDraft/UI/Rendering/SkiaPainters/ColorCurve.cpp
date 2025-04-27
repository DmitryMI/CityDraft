#include "ColorCurve.h"
#include "CityDraft/Drafts/Curve.h"
#include <QColor>
#include "CityDraft/UI/Colors/Utils.h"
#include <include/core/SkPath.h>
#include <include/core/SkSurface.h>
#include "CityDraft/UI/Rendering/SkiaWidget.h"
#include <include/encode/SkPngEncoder.h>
#include <QStandardPaths>
#include <fstream>

namespace CityDraft::UI::Rendering::SkiaPainters
{

	ColorCurve::ColorCurve(
		CityDraft::Curves::ICurve* curve,
		CityDraft::Curves::IWidthProvider* fillWidth,
		CityDraft::Curves::IWidthProvider* outlineWidth,
		const LinearColorF& fillColor,
		const LinearColorF& outlineColor,
		SkCanvas* maskCanvas
	):
		m_Curve(curve),
		m_FillWidth(fillWidth),
		m_OutlineWidth(outlineWidth),
		m_FillColor(fillColor),
		m_OutlineColor(outlineColor),
		m_MaskCanvas(maskCanvas),
		Asset(Transform2D{})
	{

	}

	ColorCurve::ColorCurve(CityDraft::Drafts::SkiaColorCurve* draft, SkCanvas* maskCanvas):
		m_MaskCanvas(maskCanvas),
		Asset(Transform2D{})
	{
		BOOST_ASSERT(draft);
		SetOwner(draft);
	}

	void ColorCurve::Paint(CityDraft::UI::Rendering::SkiaWidget* renderer, SkCanvas* canvas)
	{
		if(GetOwner())
		{
			auto owner = dynamic_cast<CityDraft::Drafts::Curve*>(GetOwner());
			BOOST_ASSERT(owner);

			m_Curve = owner->GetCurve().get();
			m_FillWidth = owner->GetFillWidth().get();
			m_OutlineWidth = owner->GetOutlineWidth().get();
			m_FillColor = owner->GetFillColor();
			m_OutlineColor = owner->GetOutlineColor();
		}

		BOOST_ASSERT(m_Curve);
		BOOST_ASSERT(m_FillWidth);
		BOOST_ASSERT(m_OutlineWidth);

		auto matrix = canvas->getLocalToDeviceAs3x3();
		double zoom = matrix.getScaleX();

		PaintFill(canvas);
		PaintOutline(renderer, canvas);
	}

	void ColorCurve::PaintFill(SkCanvas* canvas)
	{
		SkPaint paint;
		paint.setColor(CityDraft::UI::Colors::Utils::ToSkColor(m_FillColor));
		PaintCurve(canvas, paint, m_FillWidth, nullptr);

		constexpr auto skColor = CityDraft::UI::Colors::Utils::ToSkColor("#000000FF"_frgba);
		paint.setColor(skColor);
		PaintCurve(m_MaskCanvas, paint, m_FillWidth, nullptr);
	}

	void ColorCurve::PaintOutline(CityDraft::UI::Rendering::SkiaWidget* renderer, SkCanvas* canvas)
	{
		sk_sp<SkImage> maskImage = m_MaskCanvas->getSurface()->makeImageSnapshot();

		Vector2D rendererCenter = renderer->GetViewportCenter();
		double rendererScale = renderer->GetViewportZoom();

		canvas->saveLayer(nullptr, nullptr);

		SkPaint outlinePaint;
		outlinePaint.setColor(CityDraft::UI::Colors::Utils::ToSkColor(m_OutlineColor));
		PaintCurve(canvas, outlinePaint, m_FillWidth, m_OutlineWidth);

		SkPaint maskedPaint;
		maskedPaint.setBlendMode(SkBlendMode::kDstOut);

		canvas->translate(rendererCenter.GetX(), rendererCenter.GetY());
		canvas->scale(1 / rendererScale, 1 / rendererScale);
		canvas->translate(-renderer->size().width() / 2, -renderer->size().height() / 2);

		canvas->drawImage(maskImage, 0, 0, SkSamplingOptions(), &maskedPaint);

		canvas->restore();
	}

	void ColorCurve::PaintCurve(SkCanvas* canvas, SkPaint paint, CityDraft::Curves::IWidthProvider* widthA, CityDraft::Curves::IWidthProvider* widthB)
	{
		size_t segments = 100;

		double width = 0;
		if(widthA)
		{
			width += widthA->GetWidth(0);
		}
		if(widthB)
		{
			width += widthB->GetWidth(0);
		}

		paint.setStrokeWidth(width);
		paint.setStyle(SkPaint::kStroke_Style);

		paint.setAntiAlias(true);
		SkPath path;
		Vector2D start = m_Curve->GetPoint(0);
		path.moveTo(start.GetX(), start.GetY());
		for(size_t i = 1; i <= segments; i++)
		{
			double t = (double)i / segments;
			Vector2D point = m_Curve->GetPoint(t);

			path.lineTo(point.GetX(), point.GetY());
		}

		canvas->drawPath(path, paint);
	}

	void ColorCurve::DebugDumpImage(CityDraft::UI::Rendering::SkiaWidget* renderer, sk_sp<SkImage> image)
	{
		BOOST_ASSERT(image);
		std::filesystem::path dir = (QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/debug").toStdString();

		std::filesystem::path path = dir / std::format("CurveMask-{}.png", GetOwner()->GetName());
		if(std::filesystem::exists(path))
		{
			return;
		}

		std::filesystem::create_directories(path.parent_path());

		sk_sp<SkData> pngData = SkPngEncoder::Encode(
			renderer->GetDirectContext().get(),
			image.get(),
			SkPngEncoder::Options()
		);

		BOOST_ASSERT(pngData);

		std::ofstream file(path, std::ios::binary);
		file.write((const char*)pngData->data(), pngData->size());
	}

}