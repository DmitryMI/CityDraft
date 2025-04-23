//
// Created by mariiakorolevaa on 23.04.2025.
//

#pragma once

#include <QLabel>
#include "ImageSelectionWidget.h"

class LayerItem : public QWidget
{
    Q_OBJECT

public:
    LayerItem(const QString& layerName, QWidget* parent = nullptr);

    void setVisibleState(bool visible);
    bool isVisible() const;
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void visibilityToggled(const QString& layerName, bool visible);
    void removeLayer(const QString& layerName);
    void layerRenamed(const QString& oldName, const QString& newName);

private slots:
    void onToggleVisibility();

private:
    QString m_layerName;
    QLabel* m_label;
    QPushButton* m_eyeButton;
    QPushButton* m_removeButton;
    bool m_visible;

    void updateIcon() const;
};
