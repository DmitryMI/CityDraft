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
	m_layout->addWidget(new QLabel("Layers", this));
	m_layout->addWidget(m_layerList);
	m_layout->addWidget(m_addLayerButton);
	m_layout->setAlignment(Qt::AlignRight);

	connect(m_addLayerButton, &QPushButton::clicked, this, &LayersWidget::onAddLayer);
}

void CityDraft::UI::LayersWidget::addLayer(const QString& layerName)
{
	if (!m_scene)
	{
		return;
	}

	auto layer = std::make_shared<Layer>(layerName.toStdString(), /*zOrder=*/0);
	m_scene->AddLayer(layer);

	QListWidgetItem* listItem = new QListWidgetItem();
	LayerItem* layerItem = new LayerItem(layerName, this);

	m_layerList->addItem(listItem);
	m_layerList->setItemWidget(listItem, layerItem);
	m_layerList->setStyleSheet("QListWidget::item { height: 50px; }");

	connect(layerItem, &LayerItem::removeLayer, this, [this, listItem, layerName, layer]()
		{
			m_layerList->takeItem(m_layerList->row(listItem));
			delete listItem;

			m_scene->RemoveLayer(layer);

			emit layerRemoved(layerName);
		});

	connect(layerItem, &LayerItem::visibilityToggled, this, [layer](const QString&, bool visible)
		{
			layer->SetVisible(visible);
		});

	connect(layerItem, &LayerItem::layerRenamed, this, [layer](const QString&, const QString& newName)
		{
			layer->SetName(newName.toStdString());
		});
}

void CityDraft::UI::LayersWidget::reloadLayers()
{
	if (!m_scene)
		return;

	m_layerList->clear();

	for (auto& layer : m_scene->GetLayers())
	{
		addLayer(QString::fromStdString(layer->GetName()));
	}
}

void CityDraft::UI::LayersWidget::onAddLayer()
{
	addLayer("New Layer");
}
