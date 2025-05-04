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
		m_Curves.push_back(std::make_shared<CityDraft::Curves::LineSegment>(Vector2D{-1500, -1000}, Vector2D{1000, 1000}));
		
		std::vector<CityDraft::Curves::CompositeBezierCurve::Anchor> sampleCurve = {
			// Anchor 0 (Start)
			{{100, 300}, {50, -100}, {0, 0}},

			// Anchor 1
			{{300, 100}, {100, 0}, {-100, 0}},

			// Anchor 2
			{{500, 300}, {50, 100}, {-50, -100}},

			// Anchor 3 (End)
			{{700, 100}, {0, 0}, {-100, 100}}
		};
		m_Curves.push_back(std::make_shared<CityDraft::Curves::CompositeBezierCurve>(sampleCurve));
		m_Curves.push_back(std::make_shared<CityDraft::Curves::LineSegment>(Vector2D{-1000, 1000}, Vector2D{1000, -1000}));

	}

	std::shared_ptr<Drafts::Draft> CityDraft::Assets::SkiaColorCurve::CreateDraft()
	{
		auto curve = std::make_shared<Drafts::SkiaColorCurve>(this);
		std::shared_ptr<CityDraft::Curves::ICurve> lineSegment = m_Curves[m_DraftCounter % m_Curves.size()];
		auto fillWidth = std::make_shared<CityDraft::Curves::ConstWidthProvider>(50);
		auto outlineWidth = std::make_shared<CityDraft::Curves::ConstWidthProvider>(10);
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