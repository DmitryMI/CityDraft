//
// Created by mariiakorolevaa on 23.04.2025.
//

#include "LayerItem.h"
#include <qcoreevent.h>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QMouseEvent>

using namespace CityDraft::UI;

LayerItem::LayerItem(const QString& layerName, QWidget* parent)
    : QWidget(parent), m_layerName(layerName), m_visible(true)
{
    m_label = new QLabel(layerName, this);
    m_label->installEventFilter(this);

    m_eyeButton = new QPushButton(this);
    m_eyeButton->setFlat(true);
    m_eyeButton->setFixedSize(24, 24);
    updateIcon();

    m_removeButton = new QPushButton("Delete", this);
    m_removeButton->setFixedSize(50, 24);

    connect(m_eyeButton, &QPushButton::clicked, this, &LayerItem::onToggleVisibility);
    connect(m_removeButton, &QPushButton::clicked, this, [this]()
    {
        emit removeLayer(m_layerName);
    });

    auto layout = new QHBoxLayout(this);
    layout->addWidget(m_label);
    layout->addStretch();
    layout->addWidget(m_eyeButton);
    layout->addWidget(m_removeButton);
    layout->setAlignment(Qt::AlignRight);
    setLayout(layout);
}


void LayerItem::onToggleVisibility()
{
    m_visible = !m_visible;
    updateIcon();
    emit visibilityToggled(m_layerName, m_visible);
}

void LayerItem::updateIcon() const
{
    QIcon icon = m_visible ? QIcon(":/icons/eye.png") : QIcon(":/icons/eye-off.png");
    m_eyeButton->setIcon(icon);
}

void LayerItem::setVisibleState(bool visible)
{
    m_visible = visible;
    updateIcon();
}

bool LayerItem::isVisible() const
{
    return m_visible;
}

bool LayerItem::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_label && event->type() == QEvent::MouseButtonDblClick)
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton)
        {
            bool ok;
            QString newName = QInputDialog::getText(this, tr("Rename Layer"),
                                                    tr("Enter new name:"), QLineEdit::Normal,
                                                    m_layerName, &ok);
            if (ok && !newName.isEmpty() && newName != m_layerName)
            {
                QString oldName = m_layerName;
                m_layerName = newName;
                m_label->setText(newName);
                emit layerRenamed(oldName, newName);
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
