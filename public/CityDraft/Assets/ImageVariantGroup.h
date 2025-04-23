#pragma once

#include <list>
#include <memory>
#include "Image.h"

namespace CityDraft::Assets
{
	class ImageVariantGroup final
	{
	public:
		explicit ImageVariantGroup(const std::list<std::shared_ptr<Image>>& images):
			m_Images(images)
		{
			
		}

		const std::list<std::shared_ptr<Image>>& GetImageVariants() const
		{
			return m_Images;
		}

		std::shared_ptr<Image> GetDefaultImage() const
		{
			return *(m_Images.begin());
		}

	private:
		std::list<std::shared_ptr<Image>> m_Images;
	};
}
