#pragma once

#include <list>
#include <memory>
#include "Image.h"

namespace CityDraft::Assets
{
	class ImageVariantGroup
	{
	public:
		inline ImageVariantGroup(std::list<std::shared_ptr<Image>> images):
			m_Images(images)
		{
			
		}

		virtual inline const std::list<std::shared_ptr<Image>>& GetImageVariants() const
		{
			return m_Images;
		}

		virtual std::shared_ptr<Image> GetDefaultImage() const
		{
			return *(m_Images.begin());
		}

	private:
		std::list<std::shared_ptr<Image>> m_Images;
	};
}
