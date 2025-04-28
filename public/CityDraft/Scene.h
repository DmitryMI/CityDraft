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
#include <map>
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
		/// <summary>
		/// RTree Entry: Bounding Box and Draft
		/// </summary>
		using RTreeValue = std::pair<AxisAlignedBoundingBox2D::UnderlyingType, std::shared_ptr<Drafts::Draft>>;

		using LayerAddedFunc = void(Layer*);
		using LayerAddedSignal = boost::signals2::signal<LayerAddedFunc>;
		using LayerZChangedFunc = void(Layer*, int64_t, int64_t);
		using LayerZChangedSignal = boost::signals2::signal<LayerZChangedFunc>;
		using LayerNameChangedFunc = void(Layer*, const std::string&, const std::string&);
		using LayerNameChangedSignal = boost::signals2::signal<LayerNameChangedFunc>;
		using LayerFlagChangedFunc = void(Layer*);
		using LayerFlagChangedSignal = boost::signals2::signal<LayerFlagChangedFunc>;
		using LayerRemovedFunc = void(Layer*);
		using LayerRemovedSignal = boost::signals2::signal<LayerRemovedFunc>;

		using DraftAddedFunc = void(std::shared_ptr<Drafts::Draft>);
		using DraftRemovedFunc = void(Drafts::Draft*);

		enum class InsertOrder
		{
			Highest,
			Lowest,
			KeepExisting
		};

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
		/// <param name="order">Indicates what ZOrder should be assigned to the Draft</param>
		bool AddDraft(std::shared_ptr<Drafts::Draft> obj, Layer* layer, InsertOrder order);

		/// <summary>
		/// Removes the Draft from the Scene
		/// </summary>
		/// <param name="objPtr">Draft to remove</param>
		void RemoveDraft(Drafts::Draft* objPtr);

		std::shared_ptr<Layer> AddLayer(std::string_view name, InsertOrder order);

		/// <summary>
		/// Returns collection of Layers
		/// </summary>
		/// <returns>Layers</returns>
		inline std::list<std::shared_ptr<Layer>> GetLayers() const
		{
			std::list<std::shared_ptr<Layer>> result;
			std::transform(m_Layers.begin(), m_Layers.end(), std::back_inserter(result), [](auto& pair){return pair.second;});
			return result;
		}

		inline std::shared_ptr<Layer> GetLayer(int64_t z) const
		{
			if(!m_Layers.contains(z))
			{
				return nullptr;
			}

			return m_Layers.at(z);
		}

		void SwapLayersZ(Layer* layerA, Layer* layerB);

		/// <summary>
		/// Removes a Layer from the Scene
		/// </summary>
		void RemoveLayer(Layer* layer);

		void RenameLayer(CityDraft::Layer*, const std::string& name);

		void SetLayerVisibile(CityDraft::Layer*, bool isVisible);
		void SetLayerLocked(CityDraft::Layer*, bool isLocked);

		inline boost::signals2::connection ConnectToLayerAdded(const LayerAddedSignal::slot_type& slot)
		{
			return m_LayerAdded.connect(slot);
		}

		inline boost::signals2::connection ConnectToLayerRemoved(const LayerRemovedSignal::slot_type& slot)
		{
			return m_LayerRemoved.connect(slot);
		}

		inline boost::signals2::connection ConnectToLayerZChanged(const LayerZChangedSignal::slot_type& slot)
		{
			return m_LayerZChanged.connect(slot);
		}

		inline boost::signals2::connection ConnectToLayerNameChanged(const LayerNameChangedSignal::slot_type& slot)
		{
			return m_LayerNameChanged.connect(slot);
		}

		inline boost::signals2::connection ConnectToLayerFlagChanged(const LayerFlagChangedSignal::slot_type& slot)
		{
			return m_LayerFlagChanged.connect(slot);
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

		size_t QueryDrafts(Layer* layer, std::vector<std::shared_ptr<Drafts::Draft>>& outDraft);

		/// <summary>
		/// Looks for Drafts inside a Bounding Box
		/// </summary>
		/// <param name="box">Bounding Box in Scene Coordinates</param>
		/// <param name="drafts">Collection to write found Drafts into.</param>
		/// <returns>Number of found Drafts</returns>
		size_t QueryDraftsOnAllLayers(const AxisAlignedBoundingBox2D& box, std::vector<std::shared_ptr<Drafts::Draft>>& drafts);

		/// <summary>
		/// Looks for Draft with highest Z-Order that cointains the Point
		/// </summary>
		/// <param name="point">Point for hit-test</param>
		/// <returns>Found Draft or nullptr if point does not hit any draft.</returns>
		std::shared_ptr<Drafts::Draft> QueryHighestDraftAllLayers(const Vector2D& point);

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

		std::map<int64_t, std::shared_ptr<Layer>> m_Layers;

		boost::geometry::index::rtree<RTreeValue, boost::geometry::index::quadratic<16>> m_DraftsRtree;
		
		LayerAddedSignal m_LayerAdded;
		LayerZChangedSignal m_LayerZChanged;
		LayerNameChangedSignal m_LayerNameChanged;
		LayerFlagChangedSignal m_LayerFlagChanged;
		LayerRemovedSignal m_LayerRemoved;

		boost::signals2::signal<DraftAddedFunc> m_DraftAdded;
		boost::signals2::signal<DraftRemovedFunc> m_DraftRemoved;

		bool AddDraft(std::shared_ptr<Drafts::Draft> obj);
		void AddLayer(const std::shared_ptr<Layer>& layer, InsertOrder order);
		void InsertObjectToRtree(std::shared_ptr<Drafts::Draft> obj);
		bool RemoveObjectFromRtree(std::shared_ptr<Drafts::Draft> obj);
		std::shared_ptr<Drafts::Draft> RemoveObjectFromRtree(Drafts::Draft* obj);
	};
}
