#pragma once

#include "CityDraft/Drafts/Draft.h"
#include <boost/url.hpp>
#include <future>
#include <spdlog/spdlog.h>
#include <filesystem>

namespace CityDraft::Assets
{
	class AssetManager;

	enum class AssetStatus
	{
		Initialized,
		Loading,
		Loaded,
		LoadingFailed
	};

	class Asset
	{
	public:
		Asset();
		Asset(const std::filesystem::path& localFilePath, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);
		Asset(const boost::url& url, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		virtual std::shared_ptr<Drafts::Draft> CreateDraft() = 0;
		virtual AssetStatus GetStatus() const = 0;
		virtual AssetStatus LoadAsset() = 0;

		/// <summary>
		/// Checks is Asset is describing a valid entity. This does not mean that all resources of the asset are available.
		/// This only means that this Asset is property initilized. To check validity of resources use GetStatus().
		/// </summary>
		/// <returns></returns>
		virtual inline bool IsValid() const
		{
			return !m_AssetUrl.empty();
		}

		inline const boost::url& GetUrl() const
		{
			return m_AssetUrl;
		}

	protected:
		boost::url m_AssetUrl;
		std::shared_ptr<spdlog::logger> m_Logger;
		AssetManager* m_AssetManager;
	};
}
