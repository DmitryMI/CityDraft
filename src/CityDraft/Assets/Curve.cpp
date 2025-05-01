#include "CityDraft/Assets/Curve.h"

namespace CityDraft::Assets
{
	Curve::Curve(const boost::url& url, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger):
		Asset(url, assetManager, logger)
	{

	}

}