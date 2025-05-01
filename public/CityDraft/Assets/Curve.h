#pragma once

#include "Asset.h"
#include <filesystem>
#include <cstdint>
#include "CityDraft/Assets/FileSystemError.h"
#include "CityDraft/Utils/ImageLoader.h"
#include "CityDraft/Vector2D.h"

namespace CityDraft::Assets
{
	class AssetManager;

	class Curve: public Asset
	{
	public:

		Curve(const boost::url& url, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);
		
	};
}
