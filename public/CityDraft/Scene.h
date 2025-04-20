#pragma once

#include "Layer.h"
#include "Drafts/Draft.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <boost/signals2.hpp>
#include <spdlog/spdlog.h>
#include <list>
#include <filesystem>
#include "Assets/AssetManager.h"
#include <boost/geometry/index/rtree.hpp>
#include "AxisAlignedBoundingBox2D.h"

namespace CityDraft
{

	/// <summary>
	/// Container for all Drafts on the Draft
	/// </summary>
	class Scene
	{
	public:
		using RTreeValue = std::pair<AxisAlignedBoundingBox2D::UnderlyingType, std::shared_ptr<Drafts::Draft>>;
		using LayerChangedFunc = void(Layer*);
		using LayerMarkedForDeletionFunc = void(Layer*);

		using DraftAddedFunc = void(std::shared_ptr<Drafts::Draft>);
		using DraftRemovedFunc = void(Drafts::Draft*);

		inline Scene(std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger) :
			m_AssetManager(assetManager),
			m_Logger(logger)
		{
			BOOST_ASSERT(assetManager);
			BOOST_ASSERT(logger);
		};

		inline ~Scene()
		{
			m_Logger->info("{} destroyed", GetName());
		}

		const std::string& GetName() const;

		void AddDraft(std::shared_ptr<Drafts::Draft> obj);
		void RemoveDraft(Drafts::Draft* objPtr);

		inline const std::vector<Layer*>& GetLayers() const
		{
			std::vector<Layer*> result;
			std::transform(m_Layers.begin(), m_Layers.end(), result.begin(), [](const auto& layerPtr) {return layerPtr.get(); });
			return result;
		}

		virtual void UpdateObjectModel(Drafts::Draft* obj);
		size_t QueryRtreeEntries(const AxisAlignedBoundingBox2D& box, std::vector<RTreeValue>& entries);
		size_t QueryDraftsOnAllLayers(const AxisAlignedBoundingBox2D& box, std::vector<std::shared_ptr<Drafts::Draft>>& drafts);


		static std::shared_ptr<Scene> NewScene(const std::string& name, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger);
		static std::shared_ptr<Scene> LoadFromFile(const std::filesystem::path& path, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger);
		void SaveToFile(const std::filesystem::path& path);
		
	private:
		std::shared_ptr<spdlog::logger> m_Logger;
		std::shared_ptr<Assets::AssetManager> m_AssetManager;
		std::string m_Name;

		std::list<std::shared_ptr<Layer>> m_Layers;

		boost::geometry::index::rtree<RTreeValue, boost::geometry::index::quadratic<16>> m_DraftsRtree;

		boost::signals2::signal<LayerChangedFunc> m_LayerChanged;
		boost::signals2::signal<LayerMarkedForDeletionFunc> m_LayerMarkedForDeletion;

		boost::signals2::signal<DraftAddedFunc> m_DraftAdded;
		boost::signals2::signal<DraftRemovedFunc> m_DraftRemoved;

		void InsertObjectToRtree(std::shared_ptr<Drafts::Draft> obj);
		bool RemoveObjectFromRtree(std::shared_ptr<Drafts::Draft> obj);
		std::shared_ptr<Drafts::Draft> RemoveObjectFromRtree(Drafts::Draft* obj);

		friend class Draft;
	};
}
