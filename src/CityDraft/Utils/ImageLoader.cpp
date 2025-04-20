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

	LinearColorF StbPixels::GetPixel(size_t x, size_t y) const
	{
		BOOST_ASSERT(IsValid() && x >= 0 && x < Width && y >= 0 && y < Height);
		size_t index = (y * Width + x) * Channels;
		unsigned char* block = &Pixels[index];
		return LinearColorF(block, Channels);
	}

	void StbPixels::SetPixel(size_t x, size_t y, const LinearColorF& color)
	{
		BOOST_ASSERT(IsValid() && x >= 0 && x < Width && y >= 0 && y < Height);
		size_t index = (y * Width + x) * Channels;
		unsigned char* block = &Pixels[index];
		block[0] = color.R() * std::numeric_limits<unsigned char>::max();
		block[1] = color.G() * std::numeric_limits<unsigned char>::max();
		block[2] = color.B() * std::numeric_limits<unsigned char>::max();
		block[3] = color.A() * std::numeric_limits<unsigned char>::max();
	}

	std::vector<unsigned char> StbPixels::GetCropped(int centerX, int centerY, int cropWidth, int cropHeight) const
	{
		std::vector<unsigned char> cropped;

		if (!IsValid() || cropWidth <= 0 || cropHeight <= 0)
			return cropped;

		// Calculate crop origin
		int startX = centerX - cropWidth / 2;
		int startY = centerY - cropHeight / 2;

		// Clamp crop region to image bounds
		if (startX < 0) {
			cropWidth += startX;
			startX = 0;
		}
		if (startY < 0) {
			cropHeight += startY;
			startY = 0;
		}
		if (startX + cropWidth > Width) {
			cropWidth = Width - startX;
		}
		if (startY + cropHeight > Height) {
			cropHeight = Height - startY;
		}

		// If after clamping the crop is invalid, return empty
		if (cropWidth <= 0 || cropHeight <= 0)
			return cropped;

		cropped.resize(cropWidth * cropHeight * Channels);

		for (int y = 0; y < cropHeight; ++y)
		{
			const unsigned char* srcRow = Pixels + ((startY + y) * Width + startX) * Channels;
			unsigned char* dstRow = cropped.data() + (y * cropWidth) * Channels;
			std::memcpy(dstRow, srcRow, cropWidth * Channels);
		}

		return cropped;
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

