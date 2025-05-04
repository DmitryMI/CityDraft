#pragma once

#include "CityDraft/Assets/ColorCurve.h"
#include "CityDraft/Curves/LineSegment.h"
#include "CityDraft/Curves/CompositeBezierCurve.h"

namespace CityDraft::Assets
{
	class SkiaColorCurve : public ColorCurve
	{
	public:
		SkiaColorCurve(AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		std::shared_ptr<Drafts::Draft> CreateDraft() override;

	private:
		size_t m_DraftCounter = 0;
		std::vector<std::shared_ptr<CityDraft::Curves::ICurve>> m_Curves;
	};
}