#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include <QString>
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Assets/Factory.h"

namespace CityDraft::UI
{

	MainWindow::MainWindow(const QString& assetsRoot, QWidget* parent): QMainWindow(parent)
	{
		m_Ui.setupUi(this);

		CreateRenderingWidget();
		CreateAssetManager(assetsRoot);

		spdlog::info("MainWindow created");

		m_Scene = Scene::LoadSceneFromFile("mock_file.json", m_AssetManager, Logging::LogManager::CreateLogger("Scene"));
	}

	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::CreateRenderingWidget()
	{
		QWidget* placeholder = m_Ui.renderingWidgetPlaceholder;

		// Determine which widget to use
		m_Renderer = UI::Rendering::RenderingWidget::Factory("skia", this);

		// Replace placeholder with the actual widget
		QBoxLayout* layout = dynamic_cast<QBoxLayout*>(placeholder->parentWidget()->layout());
		int index = layout->indexOf(placeholder);
		layout->removeWidget(placeholder);
		delete placeholder;
		
		layout->insertWidget(index, m_Renderer);
	}

	void MainWindow::CreateAssetManager(const QString& assetsRoot)
	{
		auto assetManagerLogger = CityDraft::Logging::LogManager::CreateLogger("Assets");
		std::filesystem::path assetsRootPath(assetsRoot.toStdString());
		m_AssetManager = CityDraft::Assets::Factory::CreateAssetManager("skia", assetsRootPath, assetManagerLogger);
		BOOST_ASSERT(m_AssetManager);
		m_AssetManager->LoadAssets(assetsRootPath, true);
	}


}
