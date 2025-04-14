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

	AssetStatus Image::LoadAsset()
	{
		std::lock_guard lock(m_ResourceMutex);

		auto path = m_AssetManager->ToAssetPath(m_AssetUrl);
		if (!std::filesystem::is_regular_file(path))
		{
			m_Logger->error("Failed to load asset {}: path does not point to a file", m_AssetUrl.c_str());
			m_Status = AssetStatus::LoadingFailed;
			return m_Status;
		}

		auto pixels = CityDraft::Utils::ImageLoader::LoadImage(path, 4, m_Logger);
		if (!pixels.IsValid())
		{
			m_Status = AssetStatus::LoadingFailed;
			return m_Status;
		}

		m_Status = AssetStatus::Loaded;
		return m_Status;
	}

	Vector2D Image::GetImageSize() const
	{
		if (!m_Pixels.IsValid())
		{
			return Vector2D(0, 0);
		}

		return Vector2D(m_Pixels.Width, m_Pixels.Height);
	}
}
