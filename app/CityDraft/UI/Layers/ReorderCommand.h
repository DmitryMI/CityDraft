#pragma once

#include <QUndoCommand>
#include <memory>
#include <list>
#include "CityDraft/Scene.h"
#include "CityDraft/Layer.h"

class ReorderCommand: public QUndoCommand
{
public:
	inline ReorderCommand(
		CityDraft::Scene* scene,
		const std::list<std::shared_ptr<CityDraft::Layer>>& layers,
		QUndoCommand* parent = nullptr
	):
		m_Scene(scene),
		m_Layers(layers),
		QUndoCommand("Reorder Layers", parent)
	{
		
	}

	inline void undo() override
	{
		m_Scene->ReorderLayers(m_PreviousOrder, m_PreviousOrderZ);
	}

	inline void redo() override
	{
		m_PreviousOrder = m_Scene->GetLayers();
		m_PreviousOrder.reverse();
		for(const auto& layer : m_PreviousOrder)
		{
			m_PreviousOrderZ.push_back(layer->GetZOrder());
		}

		m_Scene->ReorderLayers(m_Layers);
	}

private:
	CityDraft::Scene* m_Scene = nullptr;
	std::list<std::shared_ptr<CityDraft::Layer>> m_PreviousOrder;
	std::list<int64_t> m_PreviousOrderZ;
	std::list<std::shared_ptr<CityDraft::Layer>> m_Layers;
};