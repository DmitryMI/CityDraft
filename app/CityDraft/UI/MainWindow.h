#pragma once

#include <QtWidgets/QMainWindow>
#include "spdlog/spdlog.h"
#include "Rendering/SkiaWidget.h"
#include "ui_MainWindow.h"
#include "CityDraft/Assets/SkiaAssetManager.h"
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
		UI::Rendering::SkiaWidget* m_RenderingWidget;
		std::shared_ptr<CityDraft::Assets::SkiaAssetManager> m_AssetManager;
		std::shared_ptr<CityDraft::Scene> m_Scene;
		QString m_AssetsRootDirectory;

		void CreateRenderingWidget();
		void CreateAssetManager(const QString& assetsRoot);

	private slots:
		void OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget);
    };

}
