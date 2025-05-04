#pragma once

#include <QHBoxLayout>
#include <QPushButton>
#include <boost/signals2.hpp>
#include "CityDraft/Scene.h"
#include <QUndoStack>
#include "DraggableLayerList.h"

namespace CityDraft::UI::Layers
{
	class ListWidget : public QWidget
	{
		Q_OBJECT
	public:
		ListWidget(Scene* scene, QUndoStack* undoStack, QWidget* parent = nullptr);
		~ListWidget();

	private:
		void LoadLayersFromScene();
		void AddLayerToUi(std::shared_ptr<CityDraft::Layer> layer);
		void OnSceneLayerAdded(CityDraft::Layer* layer);
		void OnSceneLayerRemoved(CityDraft::Layer* layer);
		void OnSceneLayersZChanged(const std::vector<CityDraft::Layer*>& layers);
		void onAddLayer();

		QVBoxLayout* m_layout;
		DraggableLayerList* m_draggableLayerList;
		QPushButton* m_addLayerButton;
		Scene* m_scene;
		QUndoStack* m_UndoStack;

		boost::signals2::connection m_LayerAddedConnection;
		boost::signals2::connection m_LayerRemovedConnection;
		boost::signals2::connection m_LayersZChangedConnection;
	};
}