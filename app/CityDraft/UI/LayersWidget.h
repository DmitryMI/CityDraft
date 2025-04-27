#pragma once

#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>

#include "CityDraft/Scene.h"

namespace CityDraft::UI
{
    class LayersWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit LayersWidget(Scene* scene,QWidget* parent = nullptr);
        void addLayer(const QString& layerName);
        void reloadLayers();

    signals:
        void layerVisibilityChanged(const QString& layerName, bool visible);
        void layerRemoved(const QString& layerName);

    private:
        QVBoxLayout* m_layout;
        QListWidget* m_layerList;
        QPushButton* m_addLayerButton;
        Scene* m_scene;

    private slots:
        void onAddLayer();
    };
}
