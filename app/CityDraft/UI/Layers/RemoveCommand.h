#pragma once

#include <QUndoCommand>
#include <memory>
#include "CityDraft/Scene.h"
#include "CityDraft/Layer.h"
#include "CityDraft/Drafts/Draft.h"
#include <vector>

class RemoveCommand: public QUndoCommand
{
public:
	inline RemoveCommand(
		CityDraft::Scene* scene,
		std::shared_ptr<CityDraft::Layer> layer,
		QUndoCommand* parent = nullptr
	):
		m_Scene(scene),
		m_Layer(layer),
		QUndoCommand("Remove Layer", parent)
	{
		BOOST_ASSERT(m_Scene);
		BOOST_ASSERT(m_Layer);
	}

	inline void undo() override
	{
		BOOST_ASSERT(m_Layer);
		// Not AddLayer(), because InsertLayer() preserves existing Z-order.
		bool ok = m_Scene->InsertLayer(m_Layer);
		BOOST_ASSERT(ok);

		for(auto& draft : m_RemovedDrafts)
		{
			int64_t draftZ = draft->GetZOrder();
			ok = m_Scene->AddDraft(draft, m_Layer.get(), CityDraft::Scene::InsertOrder::KeepExisting);
			BOOST_ASSERT(ok);
			BOOST_ASSERT(draft->GetZOrder() == draftZ);
		}

		m_RemovedDrafts.clear();
	}

	inline void redo() override
	{
		m_AssertZOrder = m_Layer->GetZOrder();
		m_RemovedDrafts.clear();
		CityDraft::Scene::QueryParams query;
		query.Layers = {m_Layer.get()};
		m_Scene->QueryDrafts(query, m_RemovedDrafts);
		m_Scene->RemoveLayer(m_Layer.get());
	}

private:
	CityDraft::Scene* m_Scene = nullptr;
	std::string m_LayerName;
	std::shared_ptr<CityDraft::Layer> m_Layer;
	int64_t m_AssertZOrder = 0;
	
	std::vector<std::shared_ptr<CityDraft::Drafts::Draft>> m_RemovedDrafts;
};