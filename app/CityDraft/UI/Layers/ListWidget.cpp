#include "ListWidget.h"
#include <QPushButton>
#include <QListWidgetItem>
#include "ItemWidget.h"
#include "AddCommand.h"

namespace CityDraft::UI::Layers
{

	ListWidget::ListWidget(Scene* scene, QUndoStack* undoStack, QWidget* parent):
		QWidget(parent),
		m_layout(new QVBoxLayout(this)),
		m_draggableLayerList(new DraggableLayerList(scene, this)),
		m_addLayerButton(new QPushButton("Add Layer", this)),
		m_scene(scene),
		m_UndoStack(undoStack)
	{
		BOOST_ASSERT(m_scene);
		BOOST_ASSERT(m_UndoStack);

		m_draggableLayerList->setDragEnabled(true);
		m_draggableLayerList->setAcceptDrops(true);
		m_draggableLayerList->setDropIndicatorShown(true);
		m_draggableLayerList->setDefaultDropAction(Qt::MoveAction);
		m_draggableLayerList->setDragDropMode(QAbstractItemView::InternalMove);

		m_layout->addWidget(new QLabel("Layers", this));
		m_layout->addWidget(m_draggableLayerList);
		m_layout->addWidget(m_addLayerButton);
		m_layout->setAlignment(Qt::AlignRight);

		connect(m_addLayerButton, &QPushButton::clicked, this, &ListWidget::onAddLayer);

		m_LayerAddedConnection = scene->ConnectToLayerAdded(std::bind(&ListWidget::OnSceneLayerAdded, this, std::placeholders::_1));
		m_LayerRemovedConnection = scene->ConnectToLayerRemoved(std::bind(&ListWidget::OnSceneLayerRemoved, this, std::placeholders::_1));
		m_LayerZChangedConnection = scene->ConnectToLayerZChanged(std::bind(&ListWidget::OnSceneLayerZChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

		LoadLayersFromScene();
	}

	ListWidget::~ListWidget()
	{
		m_LayerAddedConnection.disconnect();
		m_LayerRemovedConnection.disconnect();
		m_LayerZChangedConnection.disconnect();
	}

	void ListWidget::LoadLayersFromScene()
	{
		m_draggableLayerList->clear();
		for(const auto& layer : m_scene->GetLayers())
		{
			AddLayerToUi(layer);
		}
	}

	void ListWidget::AddLayerToUi(std::shared_ptr<CityDraft::Layer> layer)
	{
		QListWidgetItem* listItem = new QListWidgetItem();
		ItemWidget* layerItem = new ItemWidget(m_scene, layer, m_UndoStack, this);

		m_draggableLayerList->addItem(listItem);
		m_draggableLayerList->setItemWidget(listItem, layerItem);
		m_draggableLayerList->setStyleSheet("QListWidget::item { height: 50px; }");
	}

	void ListWidget::OnSceneLayerAdded(CityDraft::Layer* layer)
	{
		LoadLayersFromScene();
	}

	void ListWidget::OnSceneLayerRemoved(CityDraft::Layer* layer)
	{
		LoadLayersFromScene();
	}

	void ListWidget::OnSceneLayerZChanged(CityDraft::Layer* layer, int64_t oldZ, int64_t newZ)
	{
		LoadLayersFromScene();
	}

	void ListWidget::onAddLayer()
	{
		AddCommand* command = new AddCommand(m_scene, "New Layer", CityDraft::Scene::InsertOrder::Highest);
		m_UndoStack->push(command);
	}

}