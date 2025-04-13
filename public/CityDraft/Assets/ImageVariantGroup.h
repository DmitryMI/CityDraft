#pragma once

#include <list>
#include <memory>
#include "ImageAsset.h"

namespace CityDraft::Assets
{
	class ImageVariantGroup
	{
	public:
		virtual inline std::list<ImageAsset*> GetImageVariants() const
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
