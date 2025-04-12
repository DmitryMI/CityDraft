#pragma once

#include <QtWidgets/QMainWindow>
#include "Rendering/RenderingWidget.h"
#include "ui_MainWindow.h"

namespace CityDraft
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(QWidget* parent = nullptr);
        virtual ~MainWindow();

    private:
        Ui::MainWindow m_Ui;
		UI::Rendering::RenderingWidget* m_Renderer;

    };

}
