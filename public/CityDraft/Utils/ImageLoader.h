#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include <CityDraft/LinearColor.h>

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
		StbPixels(const StbPixels&) = delete;
		StbPixels& operator=(const StbPixels&) = delete;
		StbPixels(StbPixels&& other) noexcept;
		StbPixels& operator=(StbPixels&& other) noexcept;

		~StbPixels();

		bool IsValid() const;

		LinearColorF GetPixel(size_t x, size_t y) const;
		void SetPixel(size_t x, size_t y, const LinearColorF& color);

		std::vector<unsigned char> GetCropped(int centerX, int centerY, int cropWidth, int cropHeight) const;
	};

	class ImageLoader
	{
	public:
		static StbPixels LoadImage(const std::filesystem::path& path, int componentsPerPixel, std::shared_ptr<spdlog::logger> logger);

	};
}
