#include "CityDraft/Scene.h"
#include <algorithm>
#include "CityDraft/Serialization/BoostArchive.h"
#include "CityDraft/DraftZSortKey.h"

namespace CityDraft
{
	const std::string& Scene::GetName() const
	{
		return m_Name;
	}

	bool Scene::AddDraft(std::shared_ptr<Drafts::Draft> obj, Layer* layer, InsertOrder order)
	{
		BOOST_ASSERT(obj);
		BOOST_ASSERT(layer);
		BOOST_ASSERT(obj->m_Scene == nullptr);

		obj->m_Layer = layer;

		if(order == InsertOrder::KeepExisting)
		{

		}
		else if (layer->m_Drafts.size() == 0)
		{
			obj->m_ZOrder = 0;
		}
		else if (order == InsertOrder::Highest)
		{
			obj->m_ZOrder = layer->m_Drafts.rbegin()->first + 1;
		}
		else
		{
			obj->m_ZOrder = layer->m_Drafts.begin()->first - 1;
		}

		return AddDraft(obj);
	}

	void Scene::RemoveDraft(Drafts::Draft* obj)
	{
		if (!obj)
		{
			m_Logger->error("RemoveDraft: obj argument is nullptr");
			return;
		}
		
		if (!RemoveObjectFromRtree(obj))
		{
			BOOST_ASSERT(obj->m_Scene == nullptr);
			return;
		}

		BOOST_ASSERT(obj->m_Scene == this);

		obj->m_Layer->m_Drafts.erase(obj->m_ZOrder);
		m_DraftRemoved(obj);
		obj->m_Layer = nullptr;
		obj->m_Scene = nullptr;
		m_Logger->debug("{} removed from scene", obj->GetName());
	}

	bool Scene::AddLayer(const std::shared_ptr<Layer>& layer, InsertOrder order)
	{
		if(m_Layers.size() == 0)
		{
			layer->m_ZOrder = 0;
		}
		else if(order == InsertOrder::Highest)
		{
			layer->m_ZOrder = m_Layers.rbegin()->first + 1;
		}
		else
		{
			layer->m_ZOrder = m_Layers.begin()->first - 1;
		}
		if(m_Layers.contains(layer->m_ZOrder))
		{
			return false;
		}
		m_Layers[layer->m_ZOrder] = layer;
		m_LayerAdded(layer.get());
		return true;
	}

	std::shared_ptr<Layer> Scene::AddLayer(std::string_view name, InsertOrder order)
	{
		std::shared_ptr<Layer> layer = Layer::Make(std::string(name));
		AddLayer(layer, order);
		return layer;
	}

	bool Scene::InsertLayer(std::shared_ptr<Layer> layer)
	{
		return AddLayer(layer, InsertOrder::KeepExisting);
	}

	void Scene::SwapLayersZ(Layer* layerA, Layer* layerB)
	{
		int64_t za = layerA->GetZOrder();
		int64_t zb = layerB->GetZOrder();
		std::shared_ptr<Layer> layerAPtr = m_Layers[za];
		std::shared_ptr<Layer> layerBPtr = m_Layers[zb];
		m_Layers.erase(za);
		m_Layers.erase(zb);
		m_Layers[zb] = layerAPtr;
		m_Layers[za] = layerBPtr;
		layerAPtr->m_ZOrder = zb;
		layerBPtr->m_ZOrder = za;

		m_LayerZChanged(layerA, za, zb);
		m_LayerZChanged(layerB, zb, za);
	}

	void Scene::RemoveLayer(Layer* layer)
	{
		for(const auto draft : layer->m_Drafts)
		{
			RemoveDraft(draft.second);
		}

		std::shared_ptr<Layer> layerPtr = m_Layers[layer->GetZOrder()]; // To prevent deallocation during m_LayerRemoved execution
		m_Layers.erase(layer->GetZOrder());
		m_LayerRemoved(layerPtr.get());
	}

	void Scene::RenameLayer(CityDraft::Layer* layer, const std::string& name)
	{
		if(layer->m_Name == name)
		{
			return;
		}
		std::string oldName = layer->m_Name;
		layer->m_Name = name;
		m_LayerNameChanged(layer, oldName, name);
	}

	void Scene::SetLayerVisibile(CityDraft::Layer* layer, bool isVisible)
	{
		if(layer->m_IsVisible == isVisible)
		{
			return;
		}

		layer->m_IsVisible = isVisible;
		m_LayerFlagChanged(layer);
	}

	void Scene::SetLayerLocked(CityDraft::Layer* layer, bool isLocked)
	{
		if(layer->m_IsLocked == isLocked)
		{
			return;
		}

		layer->m_IsLocked = isLocked;
		m_LayerFlagChanged(layer);
	}

	std::shared_ptr<Scene> Scene::NewScene(const std::string& name, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger)
	{
		std::string nameStr = name;
		if (nameStr.empty())
		{
			nameStr = "New Scene";
		}

		std::shared_ptr<Scene> scene(new Scene(assetManager, logger));
		scene->m_Name = nameStr;

		auto backgroundLayer = scene->AddLayer("Background", InsertOrder::Highest);
		auto terrainLayer = scene->AddLayer("Terrain", InsertOrder::Highest);
		auto roadsLayer = scene->AddLayer("Roads", InsertOrder::Highest);
		auto foliageLayer = scene->AddLayer("Foliage", InsertOrder::Highest);
		auto wallsLayer = scene->AddLayer("Walls", InsertOrder::Highest);
		auto buildingsLayer = scene->AddLayer("Buildings", InsertOrder::Highest);
		auto annotationsLayer = scene->AddLayer("Annotations", InsertOrder::Highest);

		// file://assets/images/BuildingSmallHorisontal/Building%2012,%20blue.png
		auto building20blueAsset = assetManager->GetByUrl("file://assets/images/BuildingSmallHorisontal/Building%202,%20blue.png");
		BOOST_ASSERT(building20blueAsset);
		auto building20redAsset = assetManager->GetByUrl("file://assets/images/BuildingSmallHorisontal/Building%2012,%20red.png");
		BOOST_ASSERT(building20redAsset);
		auto tower2blueAsset = assetManager->GetByUrl("file://assets/images/BuildingSmallHorisontal/Building%2012,%20blue.png");
		BOOST_ASSERT(tower2blueAsset);

		auto building1 = building20blueAsset->CreateDraft();
		BOOST_ASSERT(building1);
		building1->SetTranslation(Vector2D(100, 100));

		auto building2 = building20redAsset->CreateDraft();
		BOOST_ASSERT(building2);
		building2->SetTranslation(Vector2D(200, 300));

		auto tower1 = tower2blueAsset->CreateDraft();
		BOOST_ASSERT(tower1);
		tower1->SetTranslation(Vector2D(500, 100));
		auto line1 = assetManager->GetColorCurve()->CreateDraft();
		auto line2 = assetManager->GetColorCurve()->CreateDraft();
		scene->AddDraft(building1, buildingsLayer.get(), InsertOrder::Highest);
		scene->AddDraft(building2, buildingsLayer.get(), InsertOrder::Highest);
		scene->AddDraft(line1, roadsLayer.get(), InsertOrder::Highest);
		scene->AddDraft(line2, roadsLayer.get(), InsertOrder::Highest);
		scene->AddDraft(tower1, wallsLayer.get(), InsertOrder::Highest);
		
		logger->warn("Scene created with hardcoded drafts");

		return scene;
	}

	std::shared_ptr<Scene> Scene::LoadFromFile(const std::filesystem::path& path, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger)
	{
		if(!std::filesystem::is_regular_file(path))
		{
			logger->error("Cannot load scene from {}: path does not point to a file.", path.string());
			return nullptr;
		}

		std::shared_ptr<Scene> scene(new Scene(assetManager, logger));

		CityDraft::Serialization::BoostInputArchive archive(path);
		archive >> scene->m_Name;

		size_t layersNum;
		archive >> layersNum;
		for(size_t i = 0; i < layersNum; i++)
		{
			std::shared_ptr<Layer> layer = Layer::Make();
			archive >> *layer;
			scene->m_Layers[layer->GetZOrder()] = layer;
		}

		if(!std::filesystem::is_regular_file(path))
		{
			logger->error("Path {} does not point to a file", path.string());
			return nullptr;
		}

		size_t draftsNum;
		archive >> draftsNum;
		for (size_t i = 0; i < draftsNum; i++)
		{
			std::string assetUrl;
			archive >> assetUrl;
			auto asset = assetManager->GetByUrl(assetUrl);
			auto draft = asset->CreateDraft();
			CityDraft::Drafts::Draft* draftRaw = draft.get();
			draftRaw->m_Scene = scene.get();
			archive >> *draftRaw;
			bool ok = scene->AddDraft(draft);
			BOOST_ASSERT(ok);
		}

		logger->info("Scene loaded from {}", path.string());
		return scene;
	}

	void Scene::UpdateDraftModel(Drafts::Draft* obj)
	{
		BOOST_ASSERT(obj->m_Scene == this);
		auto objPtr = RemoveObjectFromRtree(obj);
		BOOST_ASSERT(objPtr != nullptr);
		InsertObjectToRtree(objPtr);
	}

	size_t Scene::QueryRtreeEntries(const AxisAlignedBoundingBox2D& box, std::vector<Scene::RTreeValue>& entries)
	{
		size_t entriesNum = entries.size();
		m_DraftsRtree.query(boost::geometry::index::intersects(box.Data), std::back_inserter(entries));
		return entries.size() - entriesNum;
	}

	size_t Scene::QueryDrafts(const QueryParams& params, std::vector<std::shared_ptr<Drafts::Draft>>& outDrafts)
	{
		size_t num = outDrafts.size();
		for(const auto& pair : m_DraftsRtree)
		{
			if(SatisfiesQueryParams(pair.second.get(), params))
			{
				outDrafts.push_back(pair.second);
			}
		}

		return outDrafts.size() - num;
	}

	size_t Scene::QueryDrafts(const AxisAlignedBoundingBox2D& box, const QueryParams& params, std::vector<std::shared_ptr<Drafts::Draft>>& outDrafts)
	{
		size_t draftsSize = outDrafts.size();
		m_DraftsRtree.query(
			boost::geometry::index::intersects(box.Data),
			boost::make_function_output_iterator
			(
				[&](const auto& entry) 
				{
					if(SatisfiesQueryParams(std::get<1>(entry).get(), params))
					{
						outDrafts.push_back(std::get<1>(entry));
					}
				}
			)
		);
		return outDrafts.size() - draftsSize;
	}

	size_t Scene::QueryDrafts(const AxisAlignedBoundingBox2D& box, const QueryParams& params, std::set<CityDraft::DraftZSortKey<std::shared_ptr<Drafts::Draft>>>& outDrafts)
	{
		size_t draftsSize = outDrafts.size();
		m_DraftsRtree.query(
			boost::geometry::index::intersects(box.Data),
			boost::make_function_output_iterator
			(
			[&](const auto& entry)
		{
			if(SatisfiesQueryParams(std::get<1>(entry).get(), params))
			{
				auto draftPtr = std::get<1>(entry);
				outDrafts.emplace(DraftZSortKey(draftPtr));
			}
		}
		)
		);
		return outDrafts.size() - draftsSize;
	}

	std::shared_ptr<Drafts::Draft> Scene::QueryHighestDraft(const Vector2D& point, const QueryParams& params)
	{
		std::set<DraftZSortKey<std::shared_ptr<CityDraft::Drafts::Draft>>> draftsZOrders;
		m_DraftsRtree.query
		(
			boost::geometry::index::contains(point.Data),
			boost::make_function_output_iterator
			(
				[&](const auto& entry)
				{
					std::shared_ptr<Drafts::Draft> draft = std::get<1>(entry);
					if(!SatisfiesQueryParams(draft.get(), params))
					{
						return;
					}

					if (draft->IsPointInside(point))
					{
						draftsZOrders.insert(DraftZSortKey(draft));
					}
				}
			)
		);

		if (draftsZOrders.size() == 0)
		{
			return nullptr;
		}

		return draftsZOrders.rbegin()->Draft;
	}

	bool Scene::AddDraft(std::shared_ptr<Drafts::Draft> obj)
	{
		BOOST_ASSERT(obj->m_Layer);
		if(obj->m_Layer->m_Drafts.contains(obj->m_ZOrder))
		{
			return false;
		}

		// Ensure that layer exists on the Scene
		BOOST_ASSERT(std::any_of(m_Layers.begin(), m_Layers.end(), [obj](auto& pair){return pair.second.get() == obj->GetLayer();}));

		obj->m_Scene = this;
		if (obj->GetName() == "")
		{
			obj->m_Name = "Draft-" + std::to_string(m_DraftsRtree.size());
		}
		std::shared_ptr<Drafts::Draft> objPtr(obj);

		InsertObjectToRtree(obj);
		
		obj->m_Layer->m_Drafts[obj->m_ZOrder] = obj.get();

		m_DraftAdded(obj);
		m_Logger->debug("{} added to layer {}({}) with Z-Order {}", obj->GetName(), obj->GetLayer()->GetName(), obj->GetLayer()->GetZOrder(), obj->GetZOrder());
		return true;
	}

	void Scene::InsertObjectToRtree(std::shared_ptr<Drafts::Draft> obj)
	{
		auto bbox = obj->GetAxisAlignedBoundingBox();
		m_Logger->debug("Inserting Draft [({},{}), ({},{})] to the RTree",
			bbox.GetMin().GetX(),
			bbox.GetMin().GetY(),
			bbox.GetMax().GetX(),
			bbox.GetMax().GetY()
			);
		m_DraftsRtree.insert(std::make_pair(bbox, obj));
	}

	bool Scene::RemoveObjectFromRtree(std::shared_ptr<Drafts::Draft> obj)
	{
		return RemoveObjectFromRtree(obj.get()) != nullptr;
	}

	std::shared_ptr<Drafts::Draft> Scene::RemoveObjectFromRtree(Drafts::Draft* obj)
	{
		std::vector<RTreeValue> toRemove;
		m_DraftsRtree.query(boost::geometry::index::satisfies([&](const RTreeValue& v) {
			return v.second.get() == obj;
			}), std::back_inserter(toRemove));

		if (toRemove.size() == 0)
		{
			BOOST_ASSERT(obj->m_Scene == nullptr);
			m_Logger->warn("RemoveObjectFromRtree failed: object {} was not found in R-tree", obj->GetName());
			return nullptr;
		}

		BOOST_ASSERT(toRemove.size() == 1);

		size_t removedNum = 0;
		for (const auto& v : toRemove)
		{
			removedNum += m_DraftsRtree.remove(v);
		}

		BOOST_ASSERT(removedNum == 1);
		return toRemove[0].second;
	}

	void Scene::SaveToFile(const std::filesystem::path& path)
	{
		CityDraft::Serialization::BoostOutputArchive archive(path);

		m_Name = path.stem().string();
		archive << m_Name;
		
		archive << m_Layers.size();
		for(const auto& layerPair : m_Layers)
		{
			archive << *layerPair.second;
		}

		archive << m_DraftsRtree.size();
		for (const auto& draftEntry : m_DraftsRtree)
		{
			std::string url = draftEntry.second->GetAsset()->GetUrl().c_str();
			archive << url;
			archive << *draftEntry.second;
		}
	}

	bool Scene::SatisfiesQueryParams(CityDraft::Drafts::Draft* draft, const QueryParams& params)
	{
		BOOST_ASSERT(draft);
		if(params.Layers.size() != 0 && !params.Layers.contains(draft->GetLayer()))
		{
			return false;
		}

		if(!params.IncludeInvisible && !draft->GetLayer()->IsVisible())
		{
			return false;
		}

		return true;
	}
}
