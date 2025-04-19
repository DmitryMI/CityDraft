#pragma once

#include "spdlog/spdlog.h"
#include "Rendering/SkiaWidget.h"
#include "ui_MainWindow.h"
#include "CityDraft/Assets/SkiaAssetManager.h"
#include "CityDraft/Scene.h"
#include "CityDraft/Input/Factory.h"
#include "Rendering/ImageSelectionWidget.h"

#include <QLabel>
#include <memory>

namespace CityDraft::UI
{
	class MainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		MainWindow(QString assetsRoot, QWidget* parent = nullptr);
		virtual ~MainWindow();

	private:
		// Widgets
		Ui::MainWindow m_Ui{};
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

		// Initialization helpers
		void ReplacePlaceholdersWithSplitter();
		void CreateAssetManager(const QString& assetsRoot);
		void LoadImagesToSelectionWidget() const;
		void CreateStatusBar();

	private slots:
		void OnGraphicsInitialized(const UI::Rendering::SkiaWidget* widget);
		void OnCursorProjectedPositionChanged(const QPointF& projectedPosition) const;
	};
}