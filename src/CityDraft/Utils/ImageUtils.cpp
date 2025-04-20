#include "CityDraft/Utils/ImageUtils.h"
#include <boost/assert.hpp>


namespace CityDraft::Utils
{
	void ImageUtils::CompactImageTransform(const StbPixels& pixels, double alphaThreshold, std::shared_ptr<spdlog::logger> logger, int& outCenterX, int& outCenterY, int& outSizeX, int& outSizeY)
	{
		BOOST_ASSERT(pixels.IsValid());

		size_t left = pixels.Width;
		size_t top = pixels.Height;
		size_t right = 0;
		size_t bottom = 0;

		// Top
		for (int64_t row = 0; row < pixels.Height; row++)
		{
			size_t col = PixelsRayTrace(pixels, 0, row, 1, 0, alphaThreshold);
			if (col < pixels.Width - 1)
			{
				top = row;
				break;
			}
		}

		if (top == pixels.Height - 1)
		{
			outCenterX = 0;
			outCenterY = 0;
			outSizeX = 0;
			outSizeY = 0;
			return;
		}

		// Bottom
		for (int64_t row = pixels.Height - 1; row >= 0; row--)
		{
			size_t col = PixelsRayTrace(pixels, 0, row, 1, 0, alphaThreshold);
			if (col < pixels.Width - 1)
			{
				bottom = row;
				break;
			}
		}

		// Left
		for (int64_t col = 0; col < pixels.Width; col++)
		{
			size_t row = PixelsRayTrace(pixels, col, 0, 0, 1, alphaThreshold);
			if (row < pixels.Height - 1)
			{
				left = col;
				break;
			}
		}

		// Right
		for (int64_t col = pixels.Width - 1; col >= 0; col--)
		{
			size_t row = PixelsRayTrace(pixels, col, 0, 0, 1, alphaThreshold);
			if (row < pixels.Height - 1)
			{
				right = col;
				break;
			}
		}

		outSizeX = right - left;
		outSizeY = bottom - top;
		outCenterX = (left + right) / 2;
		outCenterY = (top + bottom) / 2;
	}

	size_t ImageUtils::PixelsRayTrace(const StbPixels& pixels, int64_t x, int64_t y, int64_t dx, int64_t dy, double alphaThreshold)
	{
		size_t counter = 0;
		while (0 <= x && x < pixels.Width && 0 <= y && y < pixels.Height)
		{
			LinearColorF color = pixels.GetPixel(x, y);
			if (color.A() >= alphaThreshold)
			{
				return counter;
			}
			x += dx;
			y += dy;
			counter++;
		}

		return counter;
	}
}

