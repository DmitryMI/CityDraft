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
		static void CompactImageTransform(const StbPixels& pixels, double alphaThreshold, std::shared_ptr<spdlog::logger> logger, int& outCenterX, int& outCenterY, int& outSizeX, int& outSizeY);

	private:
		static size_t PixelsRayTrace(const StbPixels& pixels, int64_t x, int64_t y, int64_t dx, int64_t dy, double alphaThreshold);
	};
}
