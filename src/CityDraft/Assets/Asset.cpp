#pragma once

#include "CityDraft/Assets/Asset.h"
#include "CityDraft/Assets/AssetManager.h"

namespace CityDraft::Assets
{
	Asset::Asset() :
		m_Logger(nullptr),
		m_AssetUrl("")
	{

	}

	Asset::Asset(const std::filesystem::path& localFilePath, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger):
		Asset(assetManager->AssetPathToUrl(localFilePath), assetManager, logger)
	{

	}

	Asset::Asset(const boost::url& url, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger) :
		m_AssetUrl(url),
		m_Logger(logger),
		m_AssetManager(assetManager)
	{

	}
}
