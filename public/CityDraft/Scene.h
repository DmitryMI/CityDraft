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
	/// Core class for managing instantiated Drafts, allows efficiend spatial queries and serialization to or from the filesystem. 
	/// </summary>
	class Scene
	{
	public:
		using RTreeValue = std::pair<AxisAlignedBoundingBox2D::UnderlyingType, std::shared_ptr<Drafts::Draft>>;
		using LayerChangedFunc = void(Layer*);
		using LayerMarkedForDeletionFunc = void(Layer*);

		using DraftAddedFunc = void(std::shared_ptr<Drafts::Draft>);
		using DraftRemovedFunc = void(Drafts::Draft*);

		/// <summary>
		/// Creates an empty scene. Use Scene::NewScene instead to create scene with default layers.
		/// </summary>
		/// <param name="assetManager">AssetManager</param>
		/// <param name="logger">Logger</param>
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

		/// <summary>
		/// Gets name of the Scene
		/// </summary>
		/// <returns>Name</returns>
		const std::string& GetName() const;

		/// <summary>
		/// Inserts the Draft into the Scene.
		/// </summary>
		/// <param name="obj">Draft to add</param>
		void AddDraft(std::shared_ptr<Drafts::Draft> obj);

		/// <summary>
		/// Removes the Draft from the Scene
		/// </summary>
		/// <param name="objPtr">Draft to remove</param>
		void RemoveDraft(Drafts::Draft* objPtr);

		/// <summary>
		/// Returns collection of Layers
		/// </summary>
		/// <returns>Layers</returns>
		inline const std::vector<Layer*>& GetLayers() const
		{
			std::vector<Layer*> result;
			std::transform(m_Layers.begin(), m_Layers.end(), result.begin(), [](const auto& layerPtr) {return layerPtr.get(); });
			return result;
		}

		/// <summary>
		/// Called by Drafts when their Transform changes. No need to be called manually.
		/// </summary>
		/// <param name="obj">Draft</param>
		virtual void UpdateDraftModel(Drafts::Draft* obj);

		/// <summary>
		/// Looks for RTree Entries inside a Bounding Box.
		/// </summary>
		/// <param name="box">Bounding Box in Scene Coordinates</param>
		/// <param name="outEntries">Collection to write found entries into.</param>
		/// <returns>Number of found RTree Entries</returns>
		size_t QueryRtreeEntries(const AxisAlignedBoundingBox2D& box, std::vector<RTreeValue>& outEntries);

		/// <summary>
		/// Looks for Drafts inside a Bounding Box
		/// </summary>
		/// <param name="box">Bounding Box in Scene Coordinates</param>
		/// <param name="drafts">Collection to write found Drafts into.</param>
		/// <returns>Number of found Drafts</returns>
		size_t QueryDraftsOnAllLayers(const AxisAlignedBoundingBox2D& box, std::vector<std::shared_ptr<Drafts::Draft>>& drafts);

		/// <summary>
		/// Creates a new Scene with default layers and some additional default setup.
		/// </summary>
		/// <param name="name">Name of new scene</param>
		/// <param name="assetManager">AssetManager</param>
		/// <param name="logger">Logger</param>
		/// <returns>Created scene</returns>
		static std::shared_ptr<Scene> NewScene(const std::string& name, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger);

		/// <summary>
		/// Loads Scene from a file.
		/// </summary>
		/// <param name="path">Path to serialized Scene file.</param>
		/// <param name="assetManager">AssetManager</param>
		/// <param name="logger">Logger</param>
		/// <returns>Created Scene or nullptr in case of a failure</returns>
		static std::shared_ptr<Scene> LoadFromFile(const std::filesystem::path& path, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger);

		/// <summary>
		/// Saves Scene into a file
		/// </summary>
		/// <param name="path">Path to file to serialize Scene into</param>
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
