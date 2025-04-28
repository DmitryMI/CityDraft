#pragma once

#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <boost/signals2.hpp>
#include <cstdint>
#include "CityDraft/Scene.h"

namespace CityDraft::UI
{
	class LayersWidget : public QWidget
	{
		Q_OBJECT

	public:
		explicit LayersWidget(CityDraft::Scene* scene, QWidget* parent = nullptr);
		void addLayer(const QString& layerName);

		~LayersWidget();

	private:
		QVBoxLayout* m_layout;
		QListWidget* m_layerList;
		QPushButton* m_addLayerButton;
		CityDraft::Scene* m_scene;

		boost::signals2::connection m_LayerAddedConnection;
		boost::signals2::connection m_LayerRemovedConnection;
		boost::signals2::connection m_LayerZChangedConnection;

		void SortLayerItems();

		void OnSceneLayerAdded(CityDraft::Layer* layer);
		void OnSceneLayerRemoved(CityDraft::Layer* layer);
		void OnSceneLayerZChanged(CityDraft::Layer* layer, int64_t oldZ, int64_t newZ);
	private slots:
		void onAddLayer();
	};
}
