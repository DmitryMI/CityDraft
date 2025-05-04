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
#include "CityDraft/Curves/CompositeBezierCurve.h"

namespace CityDraft::UI::Rendering::SkiaPainters
{

	ColorCurve::ColorCurve(
		CityDraft::Curves::ICurve* curve,
		CityDraft::Curves::IWidthProvider* fillWidth,
		CityDraft::Curves::IWidthProvider* outlineWidth,
		const LinearColorF& fillColor,
		const LinearColorF& outlineColor
	):
		m_Curve(curve),
		m_FillWidth(fillWidth),
		m_OutlineWidth(outlineWidth),
		m_FillColor(fillColor),
		m_OutlineColor(outlineColor),
		Asset(Transform2D{})
	{

	}

	ColorCurve::ColorCurve(CityDraft::Drafts::SkiaColorCurve* draft):
		Asset(Transform2D{})
	{
		BOOST_ASSERT(draft);
		SetOwner(draft);
	}

	void ColorCurve::Paint(CityDraft::UI::Rendering::SkiaWidget* renderer)
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

		SkCanvas* canvas = renderer->GetPrimaryCanvas();

		auto matrix = canvas->getLocalToDeviceAs3x3();
		double zoom = matrix.getScaleX();

		PaintFill(renderer);
		PaintOutline(renderer);
	}

	void ColorCurve::PaintFill(CityDraft::UI::Rendering::SkiaWidget* renderer)
	{
		SkPaint paint;
		paint.setColor(CityDraft::UI::Colors::Utils::ToSkColor(m_FillColor));
		PaintCurve(renderer->GetPrimaryCanvas(), paint, m_FillWidth, nullptr);

		constexpr auto skColor = CityDraft::UI::Colors::Utils::ToSkColor("#000000FF"_frgba);
		paint.setColor(skColor);
		PaintCurve(renderer->GetMaskCanvas(), paint, m_FillWidth, nullptr);
	}

	void ColorCurve::PaintOutline(CityDraft::UI::Rendering::SkiaWidget* renderer)
	{
		auto* canvas = renderer->GetPrimaryCanvas();
		auto* maskCanvas = renderer->GetMaskCanvas();
		BOOST_ASSERT(canvas);
		BOOST_ASSERT(maskCanvas);
		sk_sp<SkImage> maskImage = maskCanvas->getSurface()->makeImageSnapshot();

		Vector2D rendererCenter = renderer->GetViewportCenter();
		double rendererScale = renderer->GetViewportZoom();

		canvas->saveLayer(nullptr, nullptr);

		SkPaint outlinePaint;
		outlinePaint.setColor(CityDraft::UI::Colors::Utils::ToSkColor(m_OutlineColor));
		PaintCurve(renderer->GetPrimaryCanvas(), outlinePaint, m_FillWidth, m_OutlineWidth);

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
		BOOST_ASSERT(canvas);
		if(dynamic_cast<CityDraft::Curves::CompositeBezierCurve*>(m_Curve))
		{
			PaintCompositeBezierCurve(canvas, paint, widthA, widthB);
			return;
		}

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

	void ColorCurve::PaintCompositeBezierCurve(SkCanvas* canvas, SkPaint paint, CityDraft::Curves::IWidthProvider* widthA, CityDraft::Curves::IWidthProvider* widthB)
	{
		auto* compositeBezierCurve = dynamic_cast<CityDraft::Curves::CompositeBezierCurve*>(m_Curve);
		BOOST_ASSERT(compositeBezierCurve);

		SkPath path;

		const auto& anchors = compositeBezierCurve->GetAnchors();

		if(anchors.size() >= 2)
		{
			const auto& firstAnchor = anchors[0];
			path.moveTo(firstAnchor.Position.GetX(), firstAnchor.Position.GetY());

			for(size_t i = 1; i < anchors.size(); ++i)
			{
				const auto& prev = anchors[i - 1];
				const auto& curr = anchors[i];

				SkPoint cp1 = {
					static_cast<float>(prev.Position.GetX() + prev.OutgoingHandle.GetX()),
					static_cast<float>(prev.Position.GetY() + prev.OutgoingHandle.GetY())
				};

				SkPoint cp2 = {
					static_cast<float>(curr.Position.GetX() + curr.IncomingHandle.GetX()),
					static_cast<float>(curr.Position.GetY() + curr.IncomingHandle.GetY())
				};

				SkPoint end = {
					static_cast<float>(curr.Position.GetX()),
					static_cast<float>(curr.Position.GetY())
				};

				path.cubicTo(cp1.x(), cp1.y(), cp2.x(), cp2.y(), end.x(), end.y());
			}
		}

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