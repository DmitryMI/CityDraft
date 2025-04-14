#pragma once

#include <QtWidgets/QMainWindow>
#include "spdlog/spdlog.h"
#include "Rendering/RenderingWidget.h"
#include "ui_MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Scene.h"

namespace CityDraft::UI
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(const QString& assetsRoot, QWidget* parent = nullptr);
        virtual ~MainWindow();

    private:
        Ui::MainWindow m_Ui;
		UI::Rendering::RenderingWidget* m_Renderer;
		std::shared_ptr<CityDraft::Assets::AssetManager> m_AssetManager;
		std::shared_ptr<CityDraft::Scene> m_Scene;

		void CreateRenderingWidget();
		void CreateAssetManager(const QString& assetsRoot);
    };

}
