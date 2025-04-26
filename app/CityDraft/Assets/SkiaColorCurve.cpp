#pragma once

#include "CityDraft/Assets/ColorCurve.h"
#include "SkiaColorCurve.h"
#include "CityDraft/Drafts/SkiaColorCurve.h"
#include "CityDraft/Curves/ConstWidthProvider.h"
#include "CityDraft/LinearColor.h"

namespace CityDraft::Assets
{
	SkiaColorCurve::SkiaColorCurve(AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger):
		ColorCurve(assetManager, logger)
	{
		m_LineSegments.push_back(std::make_shared<CityDraft::Curves::LineSegment>(Vector2D{-100, -100}, Vector2D{100, 100}));
		m_LineSegments.push_back(std::make_shared<CityDraft::Curves::LineSegment>(Vector2D{-100, 100}, Vector2D{100, -100}));
	}

	std::shared_ptr<Drafts::Draft> CityDraft::Assets::SkiaColorCurve::CreateDraft()
	{
		auto curve = std::make_shared<Drafts::SkiaColorCurve>(this);
		std::shared_ptr<CityDraft::Curves::LineSegment> lineSegment = m_LineSegments[m_DraftCounter % m_LineSegments.size()];
		auto fillWidth = std::make_shared<CityDraft::Curves::ConstWidthProvider>(15);
		auto outlineWidth = std::make_shared<CityDraft::Curves::ConstWidthProvider>(5);
		LinearColorF fillColor = "#a4764aff"_frgba;
		LinearColorF outlineColor = "#7c5f42ff"_frgba;
		curve->SetCurve(lineSegment);
		curve->SetFillWidth(fillWidth);
		curve->SetOutlineWidth(outlineWidth);
		curve->SetFillColor(fillColor);
		curve->SetOutlineColor(outlineColor);
		m_DraftCounter++;
		return curve;
	}
}