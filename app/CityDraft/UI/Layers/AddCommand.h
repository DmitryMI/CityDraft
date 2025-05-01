#pragma once

#include <QUndoCommand>
#include <memory>
#include "CityDraft/Scene.h"
#include "CityDraft/Layer.h"

class AddCommand: public QUndoCommand
{
public:
	inline AddCommand(
		CityDraft::Scene* scene,
		const std::string& layerName,
		CityDraft::Scene::InsertOrder insertOrder,
		QUndoCommand* parent = nullptr
	):
		m_Scene(scene),
		m_LayerName(layerName),
		m_InsertOrder(insertOrder),
		QUndoCommand("Add Layer", parent)
	{}

	inline void undo() override
	{
		BOOST_ASSERT(m_AddedLayer);
		m_Scene->RemoveLayer(m_AddedLayer.get());
	}

	inline void redo() override
	{
		if(m_AddedLayer)
		{
			// Not AddLayer(), because InsertLayer() preserves existing Z-order.
			bool ok = m_Scene->InsertLayer(m_AddedLayer);
			BOOST_ASSERT(ok);
			BOOST_ASSERT(m_AddedLayer->GetZOrder() == m_AssertZOrder);
		}
		else
		{
			m_AddedLayer = m_Scene->AddLayer(m_LayerName, m_InsertOrder);
			BOOST_ASSERT(m_AddedLayer);
			m_AssertZOrder = m_AddedLayer->GetZOrder();
		}
	}

private:
	CityDraft::Scene* m_Scene = nullptr;
	std::string m_LayerName;
	CityDraft::Scene::InsertOrder m_InsertOrder;
	std::shared_ptr<CityDraft::Layer> m_AddedLayer = nullptr;
	int64_t m_AssertZOrder = 0;
};