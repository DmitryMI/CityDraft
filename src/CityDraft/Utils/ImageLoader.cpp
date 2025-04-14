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

	StbPixels& StbPixels::operator=(StbPixels&& other) noexcept
	{
		if (this != &other)
		{
			if (Pixels)
			{
				stbi_image_free(Pixels);
			}
			Pixels = other.Pixels;
			Width = other.Width;
			Height = other.Height;
			Channels = other.Channels;
			other.Pixels = nullptr;
		}
		return *this;
	}

	StbPixels::StbPixels(StbPixels&& other) noexcept
		: Pixels(other.Pixels), Width(other.Width), Height(other.Height), Channels(other.Channels)
	{
		other.Pixels = nullptr;
	}

	StbPixels::~StbPixels() 
	{
		if (Pixels)
		{
			stbi_image_free(Pixels);
			Pixels = nullptr;
		}
	}

	bool StbPixels::IsValid() const
	{
		return Pixels != nullptr;
	}

	StbPixels ImageLoader::LoadImage(const std::filesystem::path& path, int componentsPerPixel, std::shared_ptr<spdlog::logger> logger)
	{
		BOOST_ASSERT(logger);

		int width, height, channels;
		unsigned char* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 4); // force RGBA
		if (!pixels)
		{
			logger->error("Failed to load image {}", path.string());
			return {};
		}

		StbPixels stbPixels(pixels, width, height, channels );
		return stbPixels;
	}
}

