#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>

namespace CityDraft::Utils
{
	class StbPixels
	{
	public:
		unsigned char* Pixels = nullptr;

		int Width = 0;
		int Height = 0;
		int Channels = 0;

		StbPixels() = default;
		StbPixels(unsigned char* pixels, int w, int h, int c);
		~StbPixels();

		bool IsValid() const;
	};

	class ImageLoader
	{
	public:
		static StbPixels LoadImage(const std::filesystem::path& path, int componentsPerPixel, std::shared_ptr<spdlog::logger> logger);

	};
}
