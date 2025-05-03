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

        if (oldRow == newRow || oldRow < 0 || newRow < 0)
            return;

        const int step = (newRow > oldRow) ? 1 : -1;

        for (int i = oldRow; i != newRow; i += step)
        {
            auto* itemA = item(i);
            auto* itemB = item(i + step);

            const auto* widgetA = qobject_cast<ItemWidget*>(itemWidget(itemA));
            const auto* widgetB = qobject_cast<ItemWidget*>(itemWidget(itemB));

            if (!widgetA || !widgetB)
                continue;

            Layer* layerA = widgetA->GetLayer().get();
            Layer* layerB = widgetB->GetLayer().get();

            m_scene->SwapLayersZ(layerA, layerB);
        }
    }
}
