#include "CityDraft/Scene.h"

namespace CityDraft
{
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

		return scene;
	}

	std::shared_ptr<Scene> Scene::LoadSceneFromFile(const std::filesystem::path& path, std::shared_ptr<Assets::AssetManager> assetManager, std::shared_ptr<spdlog::logger> logger)
	{
		// Mock scene

		std::shared_ptr<Scene> scene(new Scene(assetManager, logger));
		scene->m_Name = "Mock Scene";

		scene->m_Layers.push_back(std::make_shared<Layer>("Background", 0));
		scene->m_Layers.push_back(std::make_shared<Layer>("Terrain", 1));
		scene->m_Layers.push_back(std::make_shared<Layer>("Roads", 2));
		scene->m_Layers.push_back(std::make_shared<Layer>("Walls", 3));
		scene->m_Layers.push_back(std::make_shared<Layer>("Buildings", 4));
		scene->m_Layers.push_back(std::make_shared<Layer>("Annotations", 5));

		// "assets\images\Building 20\Building 20, blue.png"
		auto building20blueAsset = assetManager->GetByUrl("file://assets/images/Building20/Building20blue.png");
		BOOST_ASSERT(building20blueAsset);
		auto building20redAsset = assetManager->GetByUrl("file://assets/images/Building20/Building20red.png");
		BOOST_ASSERT(building20redAsset);
		auto tower2blueAsset = assetManager->GetByUrl("file://assets/images/Tower2blue.png");
		BOOST_ASSERT(tower2blueAsset);
		// auto building1 = std::make_shared

		auto building1 = building20blueAsset->CreateDraft();
		BOOST_ASSERT(building1);
		building1->SetTranslation(Vector2D(100, 100));

		auto building2 = building20redAsset->CreateDraft();
		BOOST_ASSERT(building2);
		building2->SetTranslation(Vector2D(256, 300));

		auto tower1 = tower2blueAsset->CreateDraft();
		BOOST_ASSERT(tower1);
		tower1->SetTranslation(Vector2D(500, 100));

		scene->AddDraft(building1);
		scene->AddDraft(building2);
		scene->AddDraft(tower1);

		logger ->info("Scene loaded from {}", path.string());
		return scene;
	}

	void Scene::UpdateObjectModel(Drafts::Draft* obj)
	{
		BOOST_ASSERT(obj->m_Scene == this);
		auto objPtr = RemoveObjectFromRtree(obj);
		BOOST_ASSERT(objPtr != nullptr);
		InsertObjectToRtree(objPtr);
	}

	void Scene::InsertObjectToRtree(std::shared_ptr<Drafts::Draft> obj)
	{
		m_DraftsRtree.insert(std::make_pair(obj->GetAxisAlignedBoundingBox(), obj));
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

}
