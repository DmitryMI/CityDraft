#pragma once

#include "CityDraft/Drafts/Draft.h"
#include <boost/url.hpp>
#include <future>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <boost/signals2.hpp>

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
		using AssetLoadedEventSignal = boost::signals2::signal<void(Asset*, bool)>;

		Asset();
		Asset(const std::filesystem::path& localFilePath, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);
		Asset(const boost::url& url, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		virtual std::shared_ptr<Drafts::Draft> CreateDraft() = 0;
		virtual inline AssetStatus GetStatus() const
		{
			return m_Status;
		}

		inline AssetStatus LoadAsset()
		{
			LoadAssetInternal();
			m_AssetLoadedEvent(this, m_Status == AssetStatus::Loaded);
			return m_Status;
		}

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

		inline boost::signals2::connection ConnectToAssetLoadedEvent(const AssetLoadedEventSignal::slot_type& slot)
		{
			return m_AssetLoadedEvent.connect(slot);
		}

	protected:
		boost::url m_AssetUrl;
		std::shared_ptr<spdlog::logger> m_Logger;
		std::mutex m_ResourceMutex;
		AssetManager* m_AssetManager;
		AssetStatus m_Status{ AssetStatus::Initialized };

		AssetLoadedEventSignal m_AssetLoadedEvent;

		virtual void LoadAssetInternal() = 0;
	};
}
