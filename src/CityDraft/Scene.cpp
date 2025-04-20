#include "CityDraft/Scene.h"
#include <algorithm>
#include "CityDraft/Serialization/BoostArchive.h"

namespace CityDraft
{
	const std::string& Scene::GetName() const
	{
		return m_Name;
	}

	void Scene::AddDraft(std::shared_ptr<Drafts::Draft> obj)
	{
		BOOST_ASSERT(obj->m_Scene == nullptr);
		obj->m_Scene = this;
		if (obj->GetName() == "")
		{
			obj->m_Name = "Draft-" + std::to_string(m_DraftsRtree.size());
		}
		std::shared_ptr<Drafts::Draft> objPtr(obj);

		InsertObjectToRtree(obj);
		m_DraftAdded(obj);
		m_Logger->debug("{} added to scene", obj->GetName());
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

		obj->m_Scene = nullptr;
		m_DraftRemoved(obj);
		m_Logger->debug("{} removed from scene", obj->GetName());
	}

	std::shared_ptr<Scene> Scene::NewScene(const std::string& name, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger)
	{
		std::string nameStr = name;
		if (nameStr.empty())
		{
			nameStr = "New Draft Scene";
		}

		std::shared_ptr<Scene> scene(new Scene(assetManager, logger));
		scene->m_Name = nameStr;

		scene->m_Layers.push_back(std::make_shared<Layer>("Background", 0));
		scene->m_Layers.push_back(std::make_shared<Layer>("Terrain", 1));
		scene->m_Layers.push_back(std::make_shared<Layer>("Roads", 2));
		scene->m_Layers.push_back(std::make_shared<Layer>("Walls", 3));
		scene->m_Layers.push_back(std::make_shared<Layer>("Buildings", 4));
		scene->m_Layers.push_back(std::make_shared<Layer>("Annotations", 5));

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

		scene->AddDraft(building1);
		scene->AddDraft(building2);
		scene->AddDraft(tower1);

		logger->warn("Scene created with hardcoded drafts");

		return scene;
	}

	std::shared_ptr<Scene> Scene::LoadFromFile(const std::filesystem::path& path, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger)
	{
		std::shared_ptr<Scene> scene(new Scene(assetManager, logger));

		scene->m_Layers.push_back(std::make_shared<Layer>("Background", 0));
		scene->m_Layers.push_back(std::make_shared<Layer>("Terrain", 1));
		scene->m_Layers.push_back(std::make_shared<Layer>("Roads", 2));
		scene->m_Layers.push_back(std::make_shared<Layer>("Walls", 3));
		scene->m_Layers.push_back(std::make_shared<Layer>("Buildings", 4));
		scene->m_Layers.push_back(std::make_shared<Layer>("Annotations", 5));

		if(!std::filesystem::is_regular_file(path))
		{
			logger->error("Path {} does not point to a file", path.string());
			return nullptr;
		}

		CityDraft::Serialization::BoostInputArchive archive(path);
		archive >> scene->m_Name;
		size_t draftsNum;
		archive >> draftsNum;
		for (size_t i = 0; i < draftsNum; i++)
		{
			std::string assetUrl;
			archive >> assetUrl;
			auto asset = assetManager->GetByUrl(assetUrl);
			auto draft = asset->CreateDraft();
			CityDraft::Drafts::Draft* draftRaw = draft.get();
			archive >> *draftRaw;
			scene->AddDraft(draft);
		}

		logger->info("Scene loaded from {}", path.string());
		return scene;
	}

	void Scene::UpdateObjectModel(Drafts::Draft* obj)
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

	size_t Scene::QueryDraftsOnAllLayers(const AxisAlignedBoundingBox2D& box, std::vector<std::shared_ptr<Drafts::Draft>>& drafts)
	{
		size_t draftsSize = drafts.size();
		m_DraftsRtree.query(
			boost::geometry::index::intersects(box.Data),
			boost::make_function_output_iterator([&drafts](const auto& entry) {drafts.push_back(std::get<1>(entry)); })
		);
		return drafts.size() - draftsSize;
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

		archive << m_Name;
		// TODO Serialize layers
		archive << m_DraftsRtree.size();
		for (const auto& draftEntry : m_DraftsRtree)
		{
			std::string url = draftEntry.second->GetAsset()->GetUrl().c_str();
			archive << url;
			archive << *draftEntry.second;
		}
	}
}
