#pragma once

#include <filesystem>
#include <spdlog/spdlog.h>
#include <CityDraft/LinearColor.h>

namespace CityDraft::Utils
{

	/// <summary>
	/// Wrapper for STB image data. Automatically deallocates pixel array.
	/// </summary>
	class StbPixels
	{
	public:
		/// <summary>
		/// STB pixel array
		/// </summary>
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

		/// <summary>
		/// Checks if image data is valid
		/// </summary>
		/// <returns></returns>
		bool IsValid() const;

		LinearColorF GetPixel(size_t x, size_t y) const;
		void SetPixel(size_t x, size_t y, const LinearColorF& color);

		/// <summary>
		/// Gets copy of the image data, cropped with box with given center and size
		/// </summary>
		/// <param name="centerX">Crop box center X</param>
		/// <param name="centerY">Crop box center Y</param>
		/// <param name="cropWidth">Crop box width</param>
		/// <param name="cropHeight">Crop box height</param>
		/// <returns></returns>
		std::vector<unsigned char> GetCropped(int centerX, int centerY, int cropWidth, int cropHeight) const;
	};

	class ImageLoader
	{
	public:
		/// <summary>
		/// Loads image (multiple formats) from file using STB
		/// </summary>
		/// <param name="path">Path to image file</param>
		/// <param name="componentsPerPixel">Number of components to load (R, G, B, A)</param>
		/// <param name="logger">Logger</param>
		/// <returns>Wrapper around STB pixel data</returns>
		static StbPixels LoadImage(const std::filesystem::path& path, int componentsPerPixel, std::shared_ptr<spdlog::logger> logger);

	};
}
