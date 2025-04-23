#pragma once

#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>

namespace CityDraft::UI
{
    class LayersWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit LayersWidget(QWidget* parent = nullptr);
        void addLayer(const QString& layerName);

    signals:
        void layerVisibilityChanged(const QString& layerName, bool visible);
        void layerRemoved(const QString& layerName);

    private:
        QVBoxLayout* m_layout;
        QListWidget* m_layerList;
        QPushButton* m_addLayerButton;

    private slots:
        void onAddLayer();
    };
}
