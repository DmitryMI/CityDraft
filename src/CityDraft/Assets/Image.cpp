#pragma once

#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	Image::Image(const std::filesystem::path& path, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger) :
		Asset(path, assetManager, logger)
	{

	}

	FileSystemError Image::GetImageBytes(std::vector<uint8_t>& outBytes) const
	{
		return AssetManager::ReadFileBytes(m_AssetManager->ToAssetPath(m_AssetUrl), outBytes);
	}

	void Image::LoadAssetInternal()
	{
		std::lock_guard lock(m_ResourceMutex);

		auto path = m_AssetManager->ToAssetPath(m_AssetUrl);
		m_Logger->info("Loading SkiaImage from {}...", path.string());
		if (!std::filesystem::is_regular_file(path))
		{
			m_Logger->error("Failed to load asset {}: path does not point to a file", m_AssetUrl.c_str());
			m_Status = AssetStatus::LoadingFailed;
			return;
		}

		auto stbPixels = CityDraft::Utils::ImageLoader::LoadImage(path, 4, m_Logger);
		if (!stbPixels.IsValid())
		{
			m_Status = AssetStatus::LoadingFailed;
			return;
		}
		m_Logger->info("Read raw pixels from {}. Width: {}, Height: {}, Channels: {}", path.string(), stbPixels.Width, stbPixels.Height, stbPixels.Channels);

		LoadImage(stbPixels);
	}
}
