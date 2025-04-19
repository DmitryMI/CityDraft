#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include <QString>
#include "CityDraft/Logging/LogManager.h"
#include <qstatusbar.h>
#include "CityDraft/Input/Instruments/Panner.h"
#include "CityDraft/Input/Instruments/Selector.h"
#include <algorithm>
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/UI/Colors/Factory.h"
#include "CityDraft/Input/Instruments/ImageDraftEditor.h"

namespace CityDraft::UI
{

	MainWindow::MainWindow(const QString& assetsRoot, QWidget* parent):
		QMainWindow(parent),
		m_AssetsRootDirectory(assetsRoot)
	{
		m_Ui.setupUi(this);

		m_Logger = CityDraft::Logging::LogManager::CreateLogger("MainWindow");
		CreateMenuBar();
		CreateUndoRedoStack(m_EditMenu);
		
		m_KeyBindingProvider = CityDraft::Input::Factory::CreateKeyBindingProvider();
		m_ColorsProvider = CityDraft::UI::Colors::Factory::CreateColorsProviderProvider();

		CreateRenderingWidget();
		CreateStatusBar();

		m_Logger->info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		
	}

	void MainWindow::CreateUndoRedoStack(QMenu* menu)
	{
		BOOST_ASSERT(menu);
		m_UndoStack = new QUndoStack(this);
		QAction* undoAction = m_UndoStack->createUndoAction(this, tr("&Undo"));
		QAction* redoAction = m_UndoStack->createRedoAction(this, tr("&Redo"));
		menu->addAction(undoAction);
		menu->addAction(redoAction);
		undoAction->setShortcut(QKeySequence::Undo);
		redoAction->setShortcut(QKeySequence::Redo);
	}

	void MainWindow::CreateRenderingWidget()
	{
		QWidget* placeholder = m_Ui.renderingWidgetPlaceholder;

		m_RenderingWidget = new Rendering::SkiaWidget(this);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsInitialized, this, &MainWindow::OnGraphicsInitialized);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsPainting, this, &MainWindow::OnGraphicsPainting);
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
		m_AssetManager->LoadAssetInfos(assetsRootPath, true);
	}

	void MainWindow::CreateStatusBar()
	{
		m_ActiveInstrumentsLabel = new QLabel("");
		m_ActiveInstrumentsLabel->setMinimumWidth(200);
		statusBar()->addPermanentWidget(m_ActiveInstrumentsLabel);

		m_CursorProjectedPosition = new QLabel("Cursor at: N/A");
		m_CursorProjectedPosition->setMinimumWidth(200);
		statusBar()->addPermanentWidget(m_CursorProjectedPosition);
	}

	void MainWindow::CreateMenuBar()
	{
		QMenuBar* menuBar = new QMenuBar(this);
		setMenuBar(menuBar);
		m_EditMenu = menuBar->addMenu(tr("&Edit"));
	}

	void MainWindow::CreateInstruments()
	{
		BOOST_ASSERT(m_Scene);
		for (auto* instrument : m_ActiveInstruments)
		{
			DeactivateInstrument(instrument);
		}
		for (auto* instrument : m_InactiveInstruments)
		{
			instrument->deleteLater();
		}
		m_InactiveInstruments.clear();

		CityDraft::Input::Instruments::InstrumentDependencies dependencies;
		dependencies.Scene = m_Scene.get();
		dependencies.SelectionManager = this;
		dependencies.KeyBindingProvider = m_KeyBindingProvider.get();
		dependencies.ColorsProvider = m_ColorsProvider.get();
		dependencies.Parent = this;
		dependencies.UndoStack = m_UndoStack;
		dependencies.Renderer = m_RenderingWidget;

		m_InactiveInstruments.push_back(
			new CityDraft::Input::Instruments::Selector(dependencies));
		m_InactiveInstruments.push_back(
			new CityDraft::Input::Instruments::Panner(dependencies));
		m_InactiveInstruments.push_back(
			new CityDraft::Input::Instruments::ImageDraftEditor(dependencies));

		for (auto* instrument : m_InactiveInstruments)
		{
			connect(instrument, &CityDraft::Input::Instruments::Instrument::Finished, this, &MainWindow::OnInstrumentFinished);
		}
	}

	void MainWindow::UpdateActiveInstrumentsLabel()
	{
		QString message = "";
		
		for (const auto& instrument : m_ActiveInstruments)
		{
			message += instrument->GetName() + ", ";
		}
		if (m_ActiveInstruments.size() > 0)
		{
			message.removeLast();
			message.removeLast();
		}
		m_ActiveInstrumentsLabel->setText(message);
	}

	void MainWindow::ProcessInstrumentsMouseMoveEvent(QMouseEvent* event)
	{
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

	void MainWindow::ActivateInstrument(CityDraft::Input::Instruments::Instrument* instrument)
	{
		BOOST_ASSERT(instrument);
		BOOST_ASSERT(!instrument->IsActive());

		auto iter = std::find_if(m_InactiveInstruments.begin(), m_InactiveInstruments.end(), [instrument](auto* item) {return instrument == item; });
		BOOST_ASSERT(iter != m_InactiveInstruments.end());
		m_InactiveInstruments.erase(iter);
		m_ActiveInstruments.push_back(instrument);
		instrument->SetActive(true);
		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::DeactivateInstrument(CityDraft::Input::Instruments::Instrument* instrument)
	{
		BOOST_ASSERT(instrument);
		BOOST_ASSERT(instrument->IsActive());

		auto iter = std::find_if(m_ActiveInstruments.begin(), m_ActiveInstruments.end(), [instrument](auto* item) {return instrument == item; });
		BOOST_ASSERT(iter != m_ActiveInstruments.end());
		m_ActiveInstruments.erase(iter);
		m_InactiveInstruments.push_back(instrument);
		instrument->SetActive(false);
		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::StartSelection(QMouseEvent* event, CityDraft::Input::Instruments::Selector* selector)
	{
		if (!event->modifiers().testFlag(m_KeyBindingProvider->GetSelectionAdditiveModifier()))
		{
			m_SelectedDrafts.clear();
		}
	}

	const std::set<std::shared_ptr<CityDraft::Drafts::Draft>>& MainWindow::GetSelectedDrafts() const
	{
		return m_SelectedDrafts;
	}

	void MainWindow::ClearSelectedDrafts()
	{
		m_SelectedDrafts.clear();
		auto* editor = FindInstrument<CityDraft::Input::Instruments::ImageDraftEditor>();
		if (editor->IsActive())
		{
			DeactivateInstrument(editor);
		}
	}

	void MainWindow::AddDraftsToSelection(const std::vector<std::shared_ptr<CityDraft::Drafts::Draft>>& drafts)
	{
		if (drafts.size() == 0)
		{
			return;
		}

		for (const auto& draft : drafts)
		{
			m_SelectedDrafts.insert(draft);
		}

		auto* editor = FindInstrument<CityDraft::Input::Instruments::ImageDraftEditor>();
		if (!editor->IsActive())
		{
			ActivateInstrument(editor);
		}
	}

	void MainWindow::OnGraphicsPainting(UI::Rendering::SkiaWidget* widget)
	{
		for (const auto& instrument : m_ActiveInstruments)
		{
			instrument->OnPaint();
		}
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
			auto* panner = ActivateInstrument<CityDraft::Input::Instruments::Panner>();
			auto action = panner->OnRendererMouseButton(event, pressed);
			if (action == CityDraft::Input::Instruments::EventChainAction::Stop)
			{
				return;
			}
		}
		else if (event->button() == m_KeyBindingProvider->GetMouseSelectionButton() && pressed)
		{
			auto* selector = ActivateInstrument<CityDraft::Input::Instruments::Selector>();
			StartSelection(event, selector);
			auto action = selector->OnRendererMouseButton(event, pressed);
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

		ProcessInstrumentsMouseMoveEvent(event);
	}

	void MainWindow::OnInstrumentFinished(CityDraft::Input::Instruments::Instrument* instrument, CityDraft::Input::Instruments::FinishStatus status)
	{
		BOOST_ASSERT(instrument);
		m_Logger->info("{} finished", instrument->GetName().toStdString());

		DeactivateInstrument(instrument);
	}

	void MainWindow::OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget)
	{
		BOOST_ASSERT(widget == m_RenderingWidget);

		CreateAssetManager(m_AssetsRootDirectory);

		m_Scene = Scene::LoadSceneFromFile("mock_file.json", m_AssetManager, Logging::LogManager::CreateLogger("Scene"));
		m_RenderingWidget->SetScene(m_Scene);
		CreateInstruments();
	}

}
