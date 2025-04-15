#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include <QString>
#include "CityDraft/Logging/LogManager.h"
#include <qstatusbar.h>

namespace CityDraft::UI
{

	MainWindow::MainWindow(const QString& assetsRoot, QWidget* parent):
		QMainWindow(parent),
		m_AssetsRootDirectory(assetsRoot)
	{
		m_Ui.setupUi(this);

		m_KeyBindingProvider = CityDraft::Input::Factory::CreateKeyBindingProvider();

		CreateRenderingWidget();
		CreateStatusBar();
		spdlog::info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::CreateRenderingWidget()
	{
		QWidget* placeholder = m_Ui.renderingWidgetPlaceholder;

		m_RenderingWidget = new Rendering::SkiaWidget(m_KeyBindingProvider, this);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsInitialized, this, &MainWindow::OnGraphicsInitialized);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::CursorPositionChanged, this, &MainWindow::OnCursorProjectedPositionChanged);

		QBoxLayout* layout = dynamic_cast<QBoxLayout*>(placeholder->parentWidget()->layout());
		int index = layout->indexOf(placeholder);
		layout->removeWidget(placeholder);
		delete placeholder;
		
		layout->insertWidget(index, m_RenderingWidget);
	}

	void MainWindow::CreateAssetManager(const QString& assetsRoot)
	{
		auto assetManagerLogger = CityDraft::Logging::LogManager::CreateLogger("Assets");
		std::filesystem::path assetsRootPath(assetsRoot.toStdString());
		m_AssetManager = std::make_shared<CityDraft::Assets::SkiaAssetManager>(assetsRootPath, assetManagerLogger, m_RenderingWidget->GetDirectContext(), m_RenderingWidget->GetGlFunctions());
		BOOST_ASSERT(m_AssetManager);
		m_AssetManager->LoadAssets(assetsRootPath, true);
	}

	void MainWindow::CreateStatusBar()
	{
		m_CursorProjectedPosition = new QLabel("Cursor at: N/A");
		statusBar()->addPermanentWidget(m_CursorProjectedPosition);
	}

	void MainWindow::OnCursorProjectedPositionChanged(const QPointF& projectedPosition)
	{
		QString msg = QString::asprintf("Cursor at: (%.2f, %.2f)", projectedPosition.x(), projectedPosition.y());
		m_CursorProjectedPosition->setText(msg);
	}

	void MainWindow::OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget)
	{
		BOOST_ASSERT(widget == m_RenderingWidget);

		CreateAssetManager(m_AssetsRootDirectory);

		m_Scene = Scene::LoadSceneFromFile("mock_file.json", m_AssetManager, Logging::LogManager::CreateLogger("Scene"));
		m_RenderingWidget->SetScene(m_Scene);
	}

}
