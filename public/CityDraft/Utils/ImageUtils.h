#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include "ImageLoader.h"
#include "CityDraft/Vector2D.h"
#include <cstdint>

namespace CityDraft::Utils
{

	class ImageUtils
	{
	public:
		/// <summary>
		/// Scans the image and finds a minimal box, that cuts out transparent borders.
		/// </summary>
		/// <param name="pixels">Image Pixels</param>
		/// <param name="alphaThreshold">Pixels with Alpha less then this will be cut off</param>
		/// <param name="logger">Logger</param>
		/// <param name="outCenterX">Resulting Box Center X</param>
		/// <param name="outCenterY">Resulting Box Center Y</param>
		/// <param name="outSizeX">Resulting Box Width</param>
		/// <param name="outSizeY">Resulting Box Height</param>
		static void CompactImageTransform(const StbPixels& pixels, double alphaThreshold, std::shared_ptr<spdlog::logger> logger, int& outCenterX, int& outCenterY, int& outSizeX, int& outSizeY);

	private:
		static size_t PixelsRayTrace(const StbPixels& pixels, int64_t x, int64_t y, int64_t dx, int64_t dy, double alphaThreshold);
	};
}
