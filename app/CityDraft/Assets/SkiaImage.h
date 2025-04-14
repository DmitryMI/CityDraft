#pragma once

#include "CityDraft/Assets/Image.h"

namespace CityDraft::Drafts
{
	class Draft;
}

namespace CityDraft::Assets
{
	class SkiaImage : public Image
	{
	public:
		SkiaImage(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		std::shared_ptr<Drafts::Draft> CreateDraft() override;

	private:

	};
}
