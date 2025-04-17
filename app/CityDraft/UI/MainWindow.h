#pragma once

#include "CityDraft/Assets/SkiaAssetManager.h"
#include "CityDraft/Input/Factory.h"
#include "CityDraft/Scene.h"
#include "Rendering/SkiaWidget.h"
#include "spdlog/spdlog.h"
#include "ui_MainWindow.h"
#include <boost/signals2.hpp>
#include <list>
#include <qlabel.h>
#include <QtWidgets/QMainWindow>
#include "CityDraft/Input/Instruments/Instrument.h"

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
		QLabel* m_CursorProjectedPosition = nullptr;
		QLabel* m_ActiveInstrumentsLabel = nullptr;

		// CityDraft Objects
		std::shared_ptr<CityDraft::Assets::SkiaAssetManager> m_AssetManager;
		std::shared_ptr<CityDraft::Scene> m_Scene;

		// Config
		QString m_AssetsRootDirectory;

		// Input
		std::shared_ptr<CityDraft::Input::IKeyBindingProvider> m_KeyBindingProvider;

		void CreateRenderingWidget();
		void CreateAssetManager(const QString& assetsRoot);
		void CreateStatusBar();

	private:
		std::list<CityDraft::Input::Instruments::Instrument*> m_ActiveInstruments;

		void UpdateActiveInstrumentsLabel();

	private slots:
		void OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget);
		void OnRenderingWidgetMouseButtonEvent(QMouseEvent* event, bool pressed);
		void OnRenderingWidgetMouseMoveEvent(QMouseEvent* event);
		void OnInstrumentFinished(CityDraft::Input::Instruments::Instrument* instrument, CityDraft::Input::Instruments::FinishStatus status);
    };

}
