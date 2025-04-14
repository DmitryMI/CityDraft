#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include <QString>
#include "CityDraft/Logging/LogManager.h"

namespace CityDraft::UI
{

	MainWindow::MainWindow(const QString& assetsRoot, QWidget* parent):
		QMainWindow(parent),
		m_AssetsRootDirectory(assetsRoot)
	{
		m_Ui.setupUi(this);

		CreateRenderingWidget();

		spdlog::info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::CreateRenderingWidget()
	{
		QWidget* placeholder = m_Ui.renderingWidgetPlaceholder;

		m_RenderingWidget = new Rendering::SkiaWidget(this);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsInitialized, this, &MainWindow::OnGraphicsInitialized);

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

	void MainWindow::OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget)
	{
		BOOST_ASSERT(widget == m_RenderingWidget);

		CreateAssetManager(m_AssetsRootDirectory);

		m_Scene = Scene::LoadSceneFromFile("mock_file.json", m_AssetManager, Logging::LogManager::CreateLogger("Scene"));
	}

}
