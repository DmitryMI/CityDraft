//
// Created by mariiakorolevaa on 03.05.2025.
//

#include "DraggableLayerList.h"
#include "ItemWidget.h"

namespace CityDraft::UI::Layers
{
    DraggableLayerList::DraggableLayerList(Scene* scene, QWidget* parent)
        : QListWidget(parent), m_scene(scene)
    {
        setDragEnabled(true);
        setAcceptDrops(true);
        setDropIndicatorShown(true);
        setDragDropMode(InternalMove);
    }

    void DraggableLayerList::dropEvent(QDropEvent* event)
    {
        const int oldRow = currentRow();
        QListWidget::dropEvent(event);
        const int newRow = currentRow();

		if(oldRow == newRow || oldRow < 0 || newRow < 0)
		{
			return;
		}

		std::list<std::shared_ptr<CityDraft::Layer>> layers;
		for(int i = 0; i < this->count(); ++i)
		{
			QListWidgetItem* item = this->item(i);
			const auto* widget = qobject_cast<ItemWidget*>(itemWidget(item));
			layers.push_front(widget->GetLayer());
		}

		m_scene->ReorderLayers(layers);
    }
}
