#pragma once

#include <list>
#include <memory>
#include "ImageAsset.h"

namespace CityDraft::Assets
{
	class ImageVariantGroup
	{
	public:
		inline ImageVariantGroup(std::list<ImageAsset*> images)
		{
			for (ImageAsset* image : images)
			{
				m_ImageAssets.push_back(std::shared_ptr<ImageAsset>(image));
			}
		}

		virtual inline std::list<CityDraft::Assets::ImageAsset*> GetImageVariants() const
		{
			std::list<ImageAsset*> result;
			std::transform(m_ImageAssets.begin(), m_ImageAssets.end(), result.begin(), [](const auto& ptr) {return ptr.get(); });
			return result;
		}

		virtual ImageAsset* GetDefaultImage() const
		{
			return (*m_ImageAssets.begin()).get();
		}

	private:
		std::list<std::shared_ptr<ImageAsset>> m_ImageAssets;
	};
}
