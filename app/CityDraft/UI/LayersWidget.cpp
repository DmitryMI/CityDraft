#include "LayersWidget.h"
#include <QPushButton>
#include <QListWidgetItem>
#include "LayerItem.h"

CityDraft::UI::LayersWidget::LayersWidget(Scene* scene, QWidget* parent) :
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

	for(const auto& layer : scene->GetLayers())
	{
		OnSceneLayerAdded(layer.get());
	}
}

void CityDraft::UI::LayersWidget::addLayer(const QString& layerName)
{
	m_scene->AddLayer(layerName.toStdString(), CityDraft::Scene::InsertOrder::Highest);
}

CityDraft::UI::LayersWidget::~LayersWidget()
{
	m_LayerAddedConnection.disconnect();
	m_LayerRemovedConnection.disconnect();
	m_LayerZChangedConnection.disconnect();
}

void CityDraft::UI::LayersWidget::OnSceneLayerAdded(CityDraft::Layer* layer)
{
	QListWidgetItem* listItem = new QListWidgetItem();
	LayerItem* layerItem = new LayerItem(layer, this);

	m_layerList->addItem(listItem);
	m_layerList->setItemWidget(listItem, layerItem);
	m_layerList->setStyleSheet("QListWidget::item { height: 50px; }");

	connect(layerItem, &LayerItem::removeLayer, this, [this, listItem, layer]()
	{
		m_scene->RemoveLayer(layer);
	});

	connect(layerItem, &LayerItem::layerRenamed, this, [layer](const QString&, const QString& newName)
	{
		layer->SetName(newName.toStdString());
	});
}

void CityDraft::UI::LayersWidget::OnSceneLayerRemoved(CityDraft::Layer * layer)
{
	for(size_t i = 0; i < m_layerList->count(); i++)
	{
		QListWidgetItem* item = m_layerList->item(i);
		QWidget* widget = m_layerList->itemWidget(item);
		LayerItem* layerWidget = dynamic_cast<LayerItem*>(widget);
		if(layerWidget->GetLayer() == layer)
		{
			m_layerList->removeItemWidget(item);
			delete item;
			return;
		}
	}

	BOOST_ASSERT(false);
}

void CityDraft::UI::LayersWidget::OnSceneLayerZChanged(CityDraft::Layer * layer, int64_t oldZ, int64_t newZ)
{
}

void CityDraft::UI::LayersWidget::onAddLayer()
{
	addLayer("New Layer");
}
