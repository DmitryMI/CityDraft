#include "LayersWidget.h"
#include <QPushButton>
#include <QListWidgetItem>
#include "LayerItem.h"

CityDraft::UI::LayersWidget::LayersWidget(QWidget* parent)
    : QWidget(parent),
      m_layout(new QVBoxLayout(this)),
      m_layerList(new QListWidget(this)),
      m_addLayerButton(new QPushButton("Add Layer", this))
{
    m_layout->addWidget(new QLabel("Layers", this));
    m_layout->addWidget(m_layerList);
    m_layout->addWidget(m_addLayerButton);
    m_layout->setAlignment(Qt::AlignRight);

    connect(m_addLayerButton, &QPushButton::clicked, this, &LayersWidget::onAddLayer);
}

void CityDraft::UI::LayersWidget::addLayer(const QString& layerName)
{
    QListWidgetItem* listItem = new QListWidgetItem();

    LayerItem* layerItem = new LayerItem(layerName, this);

    m_layerList->addItem(listItem);
    m_layerList->setItemWidget(listItem, layerItem);
    m_layerList->setStyleSheet("QListWidget::item { height: 50px; }");

    connect(layerItem, &LayerItem::removeLayer, this, [this, listItem, layerName]()
    {
        m_layerList->takeItem(m_layerList->row(listItem));
        delete listItem;
        emit layerRemoved(layerName);
    });
}

void CityDraft::UI::LayersWidget::onAddLayer()
{
    addLayer("New Layer");
}
