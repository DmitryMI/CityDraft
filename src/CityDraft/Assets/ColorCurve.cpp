#include "CityDraft/Assets/ColorCurve.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	ColorCurve::ColorCurve(AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger):
		Curve(boost::urls::parse_uri(AssetManager::ColorCurveUrl).value(), assetManager, logger)
	{

	}

	void ColorCurve::LoadAssetInternal()
	{
		m_Status = CityDraft::Assets::AssetStatus::Loaded;
	}
}