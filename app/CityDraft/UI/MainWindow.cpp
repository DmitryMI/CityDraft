#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include <QString>
#include "CityDraft/Logging/LogManager.h"
#include <qstatusbar.h>
#include "CityDraft/Input/Instruments/Panner.h"
#include <algorithm>

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

		m_RenderingWidget = new Rendering::SkiaWidget(this);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsInitialized, this, &MainWindow::OnGraphicsInitialized);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::MouseMoveEvent, this, &MainWindow::OnRenderingWidgetMouseMoveEvent);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::MouseButtonEvent, this, &MainWindow::OnRenderingWidgetMouseButtonEvent);

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

		m_ActiveInstrumentsLabel = new QLabel("[]");
		statusBar()->addPermanentWidget(m_ActiveInstrumentsLabel);
	}

	void MainWindow::UpdateActiveInstrumentsLabel()
	{
		QString message = "[";
		
		for (const auto& instrument : m_ActiveInstruments)
		{
			message += instrument->GetName() + ", ";
		}
		if (m_ActiveInstruments.size() > 0)
		{
			message.removeLast();
			message.removeLast();
		}
		message += "]";
		m_ActiveInstrumentsLabel->setText(message);
	}

	void MainWindow::OnRenderingWidgetMouseButtonEvent(QMouseEvent* event, bool pressed)
	{
		BOOST_ASSERT(m_KeyBindingProvider);

		auto activeInstrumentsCopy = m_ActiveInstruments;

		for (auto& instrument : activeInstrumentsCopy)
		{
			BOOST_ASSERT(instrument);
			auto action = instrument->OnRendererMouseButton(event, pressed);
			if (action == CityDraft::Input::Instruments::EventChainAction::Stop)
			{
				return;
			}
		}

		if (event->button() == m_KeyBindingProvider->GetMouseViewportPanningButton() && pressed)
		{
			auto* panner = new CityDraft::Input::Instruments::Panner(m_KeyBindingProvider.get(), m_RenderingWidget, this);
			m_ActiveInstruments.push_back(panner);
			UpdateActiveInstrumentsLabel();
			connect(panner, &CityDraft::Input::Instruments::Instrument::Finished, this, &MainWindow::OnInstrumentFinished);
			auto action = panner->OnRendererMouseButton(event, pressed);
			if (action == CityDraft::Input::Instruments::EventChainAction::Stop)
			{
				return;
			}
		}
	}

	void MainWindow::OnRenderingWidgetMouseMoveEvent(QMouseEvent* event)
	{
		Vector2D projectedPosition = m_RenderingWidget->Project(event->position());
		QString msg = QString::asprintf("Cursor at: (%.2f, %.2f)", projectedPosition.GetX(), projectedPosition.GetY());
		m_CursorProjectedPosition->setText(msg);

		auto activeInstrumentsCopy = m_ActiveInstruments;
		for (auto& instrument : activeInstrumentsCopy)
		{
			BOOST_ASSERT(instrument);
			auto action = instrument->OnRendererMouseMove(event);
			if (action == CityDraft::Input::Instruments::EventChainAction::Stop)
			{
				return;
			}
		}
	}

	void MainWindow::OnInstrumentFinished(CityDraft::Input::Instruments::Instrument* instrument)
	{
		BOOST_ASSERT(instrument);
		size_t erasedNum = std::erase_if(m_ActiveInstruments, [instrument](const auto& ptr) {return ptr == instrument; });
		BOOST_ASSERT(erasedNum == 1);
		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget)
	{
		BOOST_ASSERT(widget == m_RenderingWidget);

		CreateAssetManager(m_AssetsRootDirectory);

		m_Scene = Scene::LoadSceneFromFile("mock_file.json", m_AssetManager, Logging::LogManager::CreateLogger("Scene"));
		m_RenderingWidget->SetScene(m_Scene);
	}

}
