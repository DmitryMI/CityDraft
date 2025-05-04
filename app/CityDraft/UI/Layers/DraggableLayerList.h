//
// Created by mariiakorolevaa on 03.05.2025.
//

#pragma once

#include <QListWidget>
#include "CityDraft/Scene.h"
#include <QUndoStack.h>

namespace CityDraft::UI::Layers
{
    class DraggableLayerList : public QListWidget
    {
        Q_OBJECT

    public:
        explicit DraggableLayerList(Scene* scene, QUndoStack* undoStack, QWidget* parent = nullptr);

    protected:
        void dropEvent(QDropEvent* event) override;

    private:
        Scene* m_scene;
		QUndoStack* m_UndoStack;
    };
}
