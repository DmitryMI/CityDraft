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

	/// <summary>
	/// Asset status
	/// </summary>
	enum class AssetStatus
	{
		Initialized,	// Asset created, not loaded
		Loaded,			// Asset fully loaded
		LoadingFailed	// Asset loading failed
	};

	/// <summary>
	/// Represents a unique visual resource.
	/// </summary>
	class Asset
	{
	public:
		using AssetLoadedEventSignal = boost::signals2::signal<void(Asset*, bool)>;

		Asset();
		Asset(const std::filesystem::path& localFilePath, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);
		Asset(const boost::url& url, AssetManager* assetManager, std::shared_ptr<spdlog::logger> logger);

		virtual ~Asset();

		/// <summary>
		/// Creates a Draft based on this Asset
		/// </summary>
		/// <returns>New Draft</returns>
		virtual std::shared_ptr<Drafts::Draft> CreateDraft() = 0;

		/// <summary>
		/// Gets status of the Asset
		/// </summary>
		/// <returns></returns>
		virtual inline AssetStatus GetStatus() const
		{
			return m_Status;
		}

		/// <summary>
		/// Tries to load all Asset's resources into memory.
		/// </summary>
		/// <returns>Status of the Asset after loading</returns>
		inline AssetStatus LoadAsset()
		{
			LoadAssetInternal();
			m_Logger->debug("Broadcasting AssetLoadedEvent: {} listeners", m_AssetLoadedEvent.num_slots());
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

		/// <summary>
		/// Returns Asset's unique identifier
		/// </summary>
		/// <returns>URL</returns>
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
