#include "CityDraft/Utils/ImageLoader.h"
#include <boost/assert.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace CityDraft::Utils
{
	StbPixels::StbPixels(unsigned char* pixels, int w, int h, int c) :
		Pixels(pixels), Width(w), Height(h), Channels(c)
	{
		
	}

	StbPixels::~StbPixels() 
	{
		stbi_image_free(Pixels);
	}

	bool StbPixels::IsValid() const
	{
		return Pixels != nullptr;
	}

	StbPixels ImageLoader::LoadImage(const std::filesystem::path& path, int componentsPerPixel, std::shared_ptr<spdlog::logger> logger)
	{
		BOOST_ASSERT(logger);

		int width, height, channels;
		unsigned char* pixels = stbi_load("example.png", &width, &height, &channels, 4); // force RGBA
		if (!pixels)
		{
			logger->error("Failed to load image {}", path.string());
			return {};
		}

		return StbPixels(pixels, width, height, channels);
	}
}

