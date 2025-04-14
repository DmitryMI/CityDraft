#include "SkiaImage.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Utils/ImageLoader.h"
#include "CityDraft/Drafts/SkiaImage.h"

namespace CityDraft::Assets
{
	SkiaImage::SkiaImage(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger):
		Image(path, assetManager, logger)
	{

	}

	std::shared_ptr<Drafts::Draft> SkiaImage::CreateDraft()
	{
		return std::shared_ptr<Drafts::Draft>();
	}
}
