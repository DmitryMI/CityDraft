#pragma once

#include <QtWidgets/QMainWindow>
#include "spdlog/spdlog.h"
#include "Rendering/SkiaWidget.h"
#include "ui_MainWindow.h"
#include "CityDraft/Assets/SkiaAssetManager.h"
#include "CityDraft/Scene.h"
#include <qlabel.h>
#include "CityDraft/Input/Factory.h"
#include "Rendering/ImageSelectionWidget.h"

namespace CityDraft::UI
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(const QString& assetsRoot, QWidget* parent = nullptr);
        virtual ~MainWindow();

    private:

		// Widgets
        Ui::MainWindow m_Ui;
		UI::Rendering::SkiaWidget* m_RenderingWidget = nullptr;
    	ImageSelectionWidget* m_ImageSelectionWidget = nullptr;
		QLabel* m_CursorProjectedPosition = nullptr;

		// CityDraft Objects
		std::shared_ptr<CityDraft::Assets::SkiaAssetManager> m_AssetManager;
		std::shared_ptr<CityDraft::Scene> m_Scene;

		// Config
		QString m_AssetsRootDirectory;

		// Input
		std::shared_ptr<CityDraft::Input::IKeyBindingProvider> m_KeyBindingProvider;

		void CreateRenderingWidget();
    	void CreateImageSelectionWidget();
		void CreateAssetManager(const QString& assetsRoot);
		void CreateStatusBar();

	private slots:
		void OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget);
		void OnCursorProjectedPositionChanged(const QPointF& projectedPosition);
    };

}
