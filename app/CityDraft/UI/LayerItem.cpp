//
// Created by mariiakorolevaa on 23.04.2025.
//

#include "LayerItem.h"
#include <qcoreevent.h>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QMouseEvent>
#include <QFile>
#include <QDir>

using namespace CityDraft::UI;

LayerItem::LayerItem(CityDraft::Layer* layer, QWidget* parent): 
	QWidget(parent),
	m_Layer(layer)
{
	BOOST_ASSERT(layer);

	m_label = new QLabel(QString::fromStdString(layer->GetName()), this);
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
			emit removeLayer(m_Layer);
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
	bool visibleOld = m_Layer->IsVisible();
	bool visibleNew = !visibleOld;
	m_Layer->SetVisible(visibleNew);
	updateIcon();
}

void LayerItem::updateIcon() const
{
	BOOST_ASSERT(QFile(":/Resources/visible.png").exists());
	BOOST_ASSERT(QFile(":/Resources/invisible.png").exists());
	QIcon icon = m_Layer->IsVisible() ? QIcon(":/Resources/visible.png") : QIcon(":/Resources/invisible.png");
	m_eyeButton->setIcon(icon);
}

void LayerItem::setVisibleState(bool visible)
{
	m_Layer->SetVisible(visible);
	updateIcon();
}

bool LayerItem::eventFilter(QObject* obj, QEvent* event)
{
	QString oldName = QString::fromStdString(m_Layer->GetName());
	if (obj == m_label && event->type() == QEvent::MouseButtonDblClick)
	{
		auto mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton)
		{
			bool ok;
			QString newName = QInputDialog::getText(this, tr("Rename Layer"),
				tr("Enter new name:"), QLineEdit::Normal,
				oldName, &ok);
			if (ok && !newName.isEmpty() && newName != oldName)
			{
				m_label->setText(newName);
				m_Layer->SetName(newName.toStdString());
			}
			return true;
		}
	}
	return QWidget::eventFilter(obj, event);
}
