#pragma once

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <boost/signals2.hpp>
#include <cstdint>
#include "CityDraft/Scene.h"
#include <QUndoStack>

namespace CityDraft::UI::Layers
{
	class ListWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit ListWidget(CityDraft::Scene* scene, QUndoStack* undoStack, QWidget* parent = nullptr);
		~ListWidget();

	private:
		QVBoxLayout* m_layout;
		QListWidget* m_layerList;
		QPushButton* m_addLayerButton;
		CityDraft::Scene* m_scene;
		QUndoStack* m_UndoStack;

		boost::signals2::connection m_LayerAddedConnection;
		boost::signals2::connection m_LayerRemovedConnection;
		boost::signals2::connection m_LayerZChangedConnection;

		void LoadLayersFromScene();
		void AddLayerToUi(std::shared_ptr<CityDraft::Layer> layer);
		void OnSceneLayerAdded(CityDraft::Layer* layer);
		void OnSceneLayerRemoved(CityDraft::Layer* layer);
		void OnSceneLayerZChanged(CityDraft::Layer* layer, int64_t oldZ, int64_t newZ);
	private slots:
		void onAddLayer();
	};
}
