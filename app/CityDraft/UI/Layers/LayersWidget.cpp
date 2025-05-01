#include "LayersWidget.h"
#include <QPushButton>
#include <QListWidgetItem>
#include "LayerItem.h"

namespace CityDraft::UI::Layers
{

	LayersWidget::LayersWidget(Scene* scene, QWidget* parent):
		QWidget(parent), m_layout(new QVBoxLayout(this)),
		m_layerList(new QListWidget(this)),
		m_addLayerButton(new QPushButton("Add Layer", this)),
		m_scene(scene)
	{
		BOOST_ASSERT(scene);

		m_layout->addWidget(new QLabel("Layers", this));
		m_layout->addWidget(m_layerList);
		m_layout->addWidget(m_addLayerButton);
		m_layout->setAlignment(Qt::AlignRight);

		connect(m_addLayerButton, &QPushButton::clicked, this, &LayersWidget::onAddLayer);

		m_LayerAddedConnection = scene->ConnectToLayerAdded(std::bind(&LayersWidget::OnSceneLayerAdded, this, std::placeholders::_1));
		m_LayerRemovedConnection = scene->ConnectToLayerRemoved(std::bind(&LayersWidget::OnSceneLayerRemoved, this, std::placeholders::_1));
		m_LayerZChangedConnection = scene->ConnectToLayerZChanged(std::bind(&LayersWidget::OnSceneLayerZChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		LoadLayersFromScene();
	}

	void LayersWidget::addLayer(const QString& layerName)
	{
		m_scene->AddLayer(layerName.toStdString(), CityDraft::Scene::InsertOrder::Highest);
	}

	LayersWidget::~LayersWidget()
	{
		m_LayerAddedConnection.disconnect();
		m_LayerRemovedConnection.disconnect();
		m_LayerZChangedConnection.disconnect();
	}

	void LayersWidget::LoadLayersFromScene()
	{
		m_layerList->clear();
		for(const auto& layer : m_scene->GetLayers())
		{
			AddLayerToUi(layer.get());
		}
	}

	void LayersWidget::AddLayerToUi(CityDraft::Layer* layer)
	{
		QListWidgetItem* listItem = new QListWidgetItem();
		LayerItem* layerItem = new LayerItem(m_scene, layer, this);

		m_layerList->addItem(listItem);
		m_layerList->setItemWidget(listItem, layerItem);
		m_layerList->setStyleSheet("QListWidget::item { height: 50px; }");
	}

	void LayersWidget::OnSceneLayerAdded(CityDraft::Layer* layer)
	{
		LoadLayersFromScene();
	}

	void LayersWidget::OnSceneLayerRemoved(CityDraft::Layer* layer)
	{
		LoadLayersFromScene();
	}

	void LayersWidget::OnSceneLayerZChanged(CityDraft::Layer* layer, int64_t oldZ, int64_t newZ)
	{
		LoadLayersFromScene();
	}

	void LayersWidget::onAddLayer()
	{
		addLayer("New Layer");
	}

}