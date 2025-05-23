//
// Created by mariiakorolevaa on 23.04.2025.
//

#include "ItemWidget.h"
#include <qcoreevent.h>
#include <QHBoxLayout>
#include <QIcon>
#include <QInputDialog>
#include <QMouseEvent>
#include <QFile>
#include "CityDraft/UI/Layers/RemoveCommand.h"

namespace CityDraft::UI::Layers
{

	ItemWidget::ItemWidget(CityDraft::Scene* scene, std::shared_ptr<CityDraft::Layer> layer, QUndoStack* undoStack, QWidget* parent):
		QWidget(parent),
		m_Scene(scene),
		m_Layer(layer),
		m_UndoStack(undoStack)
	{
		BOOST_ASSERT(m_Scene);
		BOOST_ASSERT(m_Layer);
		BOOST_ASSERT(m_UndoStack);

		m_LayerRenamedConnection = m_Scene->ConnectToLayerNameChanged(std::bind(&ItemWidget::OnLayerRenamed, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_LayerFlagChangedConnection = m_Scene->ConnectToLayerFlagChanged(std::bind(&ItemWidget::OnLayerFlagChanged, this, std::placeholders::_1));

		m_label = new QLabel(QString::fromStdString(layer->GetName()), this);
		m_label->installEventFilter(this);

		m_eyeButton = new QPushButton(this);
		m_eyeButton->setFlat(true);
		m_eyeButton->setFixedSize(24, 24);
		updateIcon();

		m_removeButton = new QPushButton("Delete", this);
		m_removeButton->setFixedSize(50, 24);

		connect(m_eyeButton, &QPushButton::clicked, this, &ItemWidget::onToggleVisibility);
		connect(m_removeButton, &QPushButton::clicked, this, [this]()
		{
			RemoveCommand* command = new RemoveCommand(m_Scene, m_Layer);
			m_UndoStack->push(command);
		});

		auto layout = new QHBoxLayout(this);
		layout->addWidget(m_label);
		layout->addStretch();
		layout->addWidget(m_eyeButton);
		layout->addWidget(m_removeButton);
		layout->setAlignment(Qt::AlignRight);
		setLayout(layout);
	}

	ItemWidget::~ItemWidget()
	{
		m_LayerRenamedConnection.disconnect();
		m_LayerFlagChangedConnection.disconnect();
	}

	void ItemWidget::onToggleVisibility()
	{
		bool visibleOld = m_Layer->IsVisible();
		bool visibleNew = !visibleOld;
		m_Layer->SetVisible(visibleNew);
	}

	void ItemWidget::OnLayerRenamed(CityDraft::Layer* layer, const std::string& nameOld, const std::string& nameNew)
	{
		if(layer != m_Layer.get())
		{
			return;
		}

		m_label->setText(QString::fromStdString(nameNew));
	}

	void ItemWidget::OnLayerFlagChanged(CityDraft::Layer* layer)
	{
		if(layer != m_Layer.get())
		{
			return;
		}
		updateIcon();
	}

	void ItemWidget::updateIcon() const
	{
		BOOST_ASSERT(QFile(":/Resources/visible.png").exists());
		BOOST_ASSERT(QFile(":/Resources/invisible.png").exists());
		QIcon icon = m_Layer->IsVisible() ? QIcon(":/Resources/visible.png") : QIcon(":/Resources/invisible.png");
		m_eyeButton->setIcon(icon);
	}

	void ItemWidget::setVisibleState(bool visible)
	{
		m_Layer->SetVisible(visible);
	}

	bool ItemWidget::eventFilter(QObject* obj, QEvent* event)
	{
		QString oldName = QString::fromStdString(m_Layer->GetName());
		if(obj == m_label && event->type() == QEvent::MouseButtonDblClick)
		{
			auto mouseEvent = static_cast<QMouseEvent*>(event);
			if(mouseEvent->button() == Qt::LeftButton)
			{
				bool ok;
				QString newName = QInputDialog::getText(this, tr("Rename Layer"),
					tr("Enter new name:"), QLineEdit::Normal,
					oldName, &ok);
				if(ok && !newName.isEmpty() && newName != oldName)
				{
					m_Layer->SetName(newName.toStdString());
				}
				return true;
			}
		}
		return QWidget::eventFilter(obj, event);
	}

}