#include <boost/assert.hpp>
#include <filesystem>
#include <map>
#include <memory>
#include <qaction.h>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qfiledialog.h>
#include <qkeysequence.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qstandardpaths.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qundostack.h>
#include <qwidget.h>
#include <set>
#include <vector>
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Assets/Image.h"
#include "CityDraft/Assets/ImageVariantGroup.h"
#include "CityDraft/Assets/SkiaAssetManager.h"
#include "CityDraft/Drafts/Draft.h"
#include "CityDraft/Input/Factory.h"
#include "CityDraft/Input/Instruments/ImageDraftEditor.h"
#include "CityDraft/Input/Instruments/Instrument.h"
#include "CityDraft/Input/Instruments/Panner.h"
#include "CityDraft/Input/Instruments/Selector.h"
#include "CityDraft/Input/Utils.h"
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/Scene.h"
#include "CityDraft/UI/Colors/Factory.h"
#include "CityDraft/Vector2D.h"
#include "MainWindow.h"
#include "ImageSelectionWidget.h"
#include "Rendering/SkiaWidget.h"

namespace CityDraft::UI
{

	MainWindow::MainWindow(const QString& assetsRoot, const QString& scenePath, QWidget* parent) :
		QMainWindow(parent),
		m_AssetsRootDirectory(assetsRoot),
		m_ScenePath(scenePath)
	{
		m_Ui.setupUi(this);

		m_Logger = CityDraft::Logging::LogManager::CreateLogger("MainWindow");
		CreateUndoRedoStack(m_Ui.menuEdit);

		m_KeyBindingProvider = CityDraft::Input::Factory::CreateKeyBindingProvider();
		m_ColorsProvider = CityDraft::UI::Colors::Factory::CreateColorsProviderProvider();

		CreateRenderingWidget();
		CreateStatusBar();
		connect(m_Ui.actionSaveAs, &QAction::triggered, this, &MainWindow::OnSaveSceneAsClicked);
		connect(m_Ui.actionOpen, &QAction::triggered, this, &MainWindow::OnOpenSceneClicked);
		connect(m_Ui.actionNewScene, &QAction::triggered, this, &MainWindow::OnNewSceneClicked);

		/*
		QTimer::singleShot(0, this, [this] {
			m_Ui.rootSplitter->setSizes({
				static_cast<int>(m_Ui.rootSplitter->size().width() * 0.20),
				static_cast<int>(m_Ui.rootSplitter->size().width() * 0.70),
				static_cast<int>(m_Ui.rootSplitter->size().width() * 0.10)
				});
			});
		*/

		m_Logger->info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		// Must be destroyed BEFORE RenderingWidget to fix bunch of GL Errors in the log.
		// This is because m_ImageSelectionWidget holds owning shared references to the Assets.
		// If RenderingWidget is destroyed first, OpenGL context is destroyed before the SkiaImage assets free their GPU textures
		// This leads to double-freeing of GPU textures by Skia.
		delete m_ImageSelectionWidget;

		m_LayerAddedConnection.disconnect();
		m_LayerRemovedConnection.disconnect();
		m_LayerZChangedConnection.disconnect();
		m_DraftAddedConnection.disconnect();
		m_DraftRemovedConnection.disconnect();
	}

	void MainWindow::InitializeUiForScene(std::shared_ptr<CityDraft::Scene> scene)
	{
		m_Scene = scene;
		m_UndoStack->clear();
		m_RenderingWidget->SetScene(scene);
		m_RenderingWidget->Repaint();
		CreateInstruments();
		CreateLayersWidget();
		setWindowTitle(QString::fromStdString(m_Scene->GetName()));

		m_LayerAddedConnection = m_Scene->ConnectToLayerAdded(std::bind(&MainWindow::OnSceneLayerAdded, this, std::placeholders::_1));
		m_LayerRemovedConnection = m_Scene->ConnectToLayerRemoved(std::bind(&MainWindow::OnSceneLayerRemoved, this, std::placeholders::_1));
		m_LayerZChangedConnection = m_Scene->ConnectToLayerZChanged(std::bind(&MainWindow::OnSceneLayerZChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_LayerFlagChangedConnection = m_Scene->ConnectToLayerFlagChanged(std::bind(&MainWindow::OnSceneLayerFlagChanged, this, std::placeholders::_1));
		m_DraftAddedConnection = m_Scene->ConnectToDraftAdded(std::bind(&MainWindow::OnSceneDraftAdded, this, std::placeholders::_1));
		m_DraftRemovedConnection = m_Scene->ConnectToDraftRemoved(std::bind(&MainWindow::OnSceneDraftRemoved, this, std::placeholders::_1));
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
		m_RenderingWidget = new Rendering::SkiaWidget(this);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsInitialized, this, &MainWindow::OnGraphicsInitialized);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::GraphicsPainting, this, &MainWindow::OnGraphicsPainting);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::MouseMoveEvent, this, &MainWindow::OnRenderingWidgetMouseMoveEvent);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::MouseButtonEvent, this, &MainWindow::OnRenderingWidgetMouseButtonEvent);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::MouseWheelEvent, this, &MainWindow::OnRenderingWidgetMouseWheelEvent);
		connect(m_RenderingWidget, &UI::Rendering::SkiaWidget::KeyboardEvent, this, &MainWindow::OnRenderingWidgetKeyboardEvent);

		QSplitter* splitter = dynamic_cast<QSplitter*>(m_Ui.renderingWidgetPlaceholder->parentWidget());
		int index = splitter->indexOf(m_Ui.renderingWidgetPlaceholder);
		splitter->insertWidget(index, m_RenderingWidget);
		delete m_Ui.renderingWidgetPlaceholder;
		m_Ui.renderingWidgetPlaceholder = nullptr;

	}

	void MainWindow::CreateAssetManager(const QString& assetsRoot)
	{
		auto assetManagerLogger = Logging::LogManager::CreateLogger("Assets");
		std::filesystem::path assetsRootPath(assetsRoot.toStdString());

		m_AssetManager = std::make_shared<Assets::SkiaAssetManager>(
			assetsRootPath,
			assetManagerLogger,
			m_RenderingWidget->GetDirectContext(),
			m_RenderingWidget->GetGlFunctions()
		);

		BOOST_ASSERT(m_AssetManager);
		m_AssetManager->LoadAssetInfos(assetsRootPath, true);
	}

	void MainWindow::CreateStatusBar()
	{
		m_CursorProjectedPosition = new QLabel("Cursor at: N/A");
		m_ActiveInstrumentsLabel = new QLabel("");
		m_ActiveInstrumentsLabel->setMinimumWidth(500);
		statusBar()->addPermanentWidget(m_ActiveInstrumentsLabel);

		m_CursorProjectedPosition = new QLabel("Cursor at: N/A");
		m_CursorProjectedPosition->setMinimumWidth(200);
		statusBar()->addPermanentWidget(m_CursorProjectedPosition);
	}

	void MainWindow::CreateInstruments()
	{
		BOOST_ASSERT(m_Scene);
		auto activeInstruments = m_ActiveInstruments;
		for (auto* instrument : activeInstruments)
		{
			DeactivateInstrument(instrument);
		}
		for (auto* instrument : m_InactiveInstruments)
		{
			instrument->deleteLater();
		}
		m_InactiveInstruments.clear();

		CityDraft::Input::Instruments::Dependencies dependencies;
		dependencies.Scene = m_Scene.get();
		dependencies.SelectionManager = this;
		dependencies.KeyBindingProvider = m_KeyBindingProvider.get();
		dependencies.ColorsProvider = m_ColorsProvider.get();
		dependencies.Parent = this;
		dependencies.UndoStack = m_UndoStack;
		dependencies.Renderer = m_RenderingWidget;

		m_InactiveInstruments.insert(
			new CityDraft::Input::Instruments::Selector(dependencies));
		m_InactiveInstruments.insert(
			new CityDraft::Input::Instruments::Panner(dependencies));
		m_InactiveInstruments.insert(
			new CityDraft::Input::Instruments::ImageDraftEditor(dependencies));

		for (auto* instrument : m_InactiveInstruments)
		{
			connect(instrument, &CityDraft::Input::Instruments::Instrument::Finished, this, &MainWindow::OnInstrumentFinished);
		}

		ActivateInstrument<CityDraft::Input::Instruments::Selector>();
		ActivateInstrument<CityDraft::Input::Instruments::Panner>();
	}

	void MainWindow::CreateImageSelectionWidget()
	{
		BOOST_ASSERT(!m_ImageSelectionWidget);
		BOOST_ASSERT(m_AssetManager);
		m_ImageSelectionWidget = new CityDraft::UI::ImageSelectionWidget(m_AssetManager.get(), this);

		auto* splitter = dynamic_cast<QSplitter*>(m_Ui.imageSelectionPlaceholder->parentWidget());
		BOOST_ASSERT(splitter);
		int index = splitter->indexOf(m_Ui.imageSelectionPlaceholder);

		delete m_Ui.imageSelectionPlaceholder;
		m_Ui.imageSelectionPlaceholder = nullptr;

		splitter->insertWidget(index, m_ImageSelectionWidget);
	}

	void MainWindow::CreateLayersWidget()
	{
		QSplitter* parent;
		int index;
		if(m_LayersWidget)
		{
			parent = dynamic_cast<QSplitter*>(m_LayersWidget->parentWidget());
			index = parent->indexOf(m_LayersWidget);
			delete m_LayersWidget;
			m_LayersWidget = nullptr;
		}
		else
		{
			parent = dynamic_cast<QSplitter*>(m_Ui.layersWidgetPlaceholder->parentWidget());
			index = parent->indexOf(m_Ui.layersWidgetPlaceholder);
			delete m_Ui.layersWidgetPlaceholder;
			m_Ui.layersWidgetPlaceholder = nullptr;
		}

		m_LayersWidget = new Layers::ListWidget(m_Scene.get(), m_UndoStack, this);
		parent->insertWidget(index, m_LayersWidget);
	}

	void MainWindow::UpdateActiveInstrumentsLabel()
	{
		std::map<CityDraft::Input::Instruments::ToolDescryptor, QString> toolDescriptions;

		for (const auto& instrument : m_ActiveInstruments)
		{
			instrument->QueryTools(toolDescriptions);
		}

		if (toolDescriptions.size() == 0)
		{
			m_ActiveInstrumentsLabel->setText("");
			return;
		}

		QStringList toolsMessages;
		for (const auto [descryptor, description] : toolDescriptions)
		{
			QStringList keysList;

			if (descryptor.Modifier.has_value())
			{
				keysList.push_back(QString::fromStdString(CityDraft::Input::Utils::ToString(descryptor.Modifier.value())));
			}
			if (descryptor.Wheel)
			{
				keysList.push_back("Wheel");
			}
			if (descryptor.MouseButton.has_value())
			{
				keysList.push_back(QString::fromStdString(CityDraft::Input::Utils::ToString(descryptor.MouseButton.value())));
			}
			if (descryptor.Key.has_value())
			{
				keysList.push_back(QString::fromStdString(CityDraft::Input::Utils::ToString(descryptor.Key.value())));
			}

			toolsMessages.push_back("[" + keysList.join(" + ") + ": " + description + "]");
		}
		m_ActiveInstrumentsLabel->setText(toolsMessages.join(", "));
	}

	void MainWindow::ProcessInstrumentsMouseButtonEvent(QMouseEvent* event, bool pressed)
	{
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

		UpdateActiveInstrumentsLabel();
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

		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::ProcessInstrumentsMouseWheelEvent(QWheelEvent* event)
	{
		auto activeInstrumentsCopy = m_ActiveInstruments;
		for (auto& instrument : activeInstrumentsCopy)
		{
			BOOST_ASSERT(instrument);
			auto action = instrument->OnRendererMouseWheel(event);
			if (action == CityDraft::Input::Instruments::EventChainAction::Stop)
			{
				return;
			}
		}

		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::ProcessInstrumentsKeyboardEvent(QKeyEvent* event)
	{
		auto activeInstrumentsCopy = m_ActiveInstruments;
		for (auto& instrument : activeInstrumentsCopy)
		{
			BOOST_ASSERT(instrument);
			auto action = instrument->OnRendererKey(event);
			if (action == CityDraft::Input::Instruments::EventChainAction::Stop)
			{
				return;
			}
		}

		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::ActivateInstrument(CityDraft::Input::Instruments::Instrument* instrument)
	{
		BOOST_ASSERT(instrument);
		BOOST_ASSERT(!instrument->IsActive());

		auto iter = std::find_if(m_InactiveInstruments.begin(), m_InactiveInstruments.end(), [instrument](auto* item) {return instrument == item; });
		BOOST_ASSERT(iter != m_InactiveInstruments.end());
		m_InactiveInstruments.erase(iter);
		m_ActiveInstruments.insert(instrument);
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
		m_InactiveInstruments.insert(instrument);
		instrument->SetActive(false);
		UpdateActiveInstrumentsLabel();
	}

	void MainWindow::OnSceneLayerAdded(CityDraft::Layer* layer)
	{
		BOOST_ASSERT(m_RenderingWidget);
		m_RenderingWidget->Repaint();
	}

	void MainWindow::OnSceneLayerRemoved(CityDraft::Layer* layer)
	{
		BOOST_ASSERT(m_RenderingWidget);
		m_RenderingWidget->Repaint();
	}

	void MainWindow::OnSceneLayerZChanged(CityDraft::Layer* layer, int64_t oldZ, int64_t newZ)
	{
		BOOST_ASSERT(m_RenderingWidget);
		m_RenderingWidget->Repaint();
	}

	void MainWindow::OnSceneLayerFlagChanged(CityDraft::Layer* layer)
	{
		BOOST_ASSERT(m_RenderingWidget);
		m_RenderingWidget->Repaint();

		auto selectedDrafts = m_SelectedDrafts;
		for(const auto& draft : selectedDrafts)
		{
			if(draft->GetLayer() == layer && (!layer->IsVisible() || !layer->IsLocked()))
			{
				m_SelectedDrafts.erase(draft);
			}
		}

		if(m_SelectedDrafts.size() == 0)
		{
			auto* editor = FindInstrument<CityDraft::Input::Instruments::ImageDraftEditor>();
			if(editor->IsActive())
			{
				DeactivateInstrument(editor);
			}
		}
	}

	void MainWindow::OnSceneDraftAdded(std::shared_ptr<Drafts::Draft> draft)
	{
		
	}

	void MainWindow::OnSceneDraftRemoved(CityDraft::Drafts::Draft* draft)
	{
		size_t num = std::erase_if(m_SelectedDrafts, [draft](const auto& ptr) {return ptr.get() == draft; });
		m_Logger->info("{} drafts removed from selection due to being removed from scene", num);
		if (m_SelectedDrafts.size() == 0)
		{
			auto* editor = FindInstrument<CityDraft::Input::Instruments::ImageDraftEditor>();
			if (editor->IsActive())
			{
				DeactivateInstrument(editor);
			}
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

		ProcessInstrumentsMouseButtonEvent(event, pressed);
	}

	void MainWindow::OnRenderingWidgetMouseMoveEvent(QMouseEvent* event)
	{
		Vector2D projectedPosition = m_RenderingWidget->Project(event->position());
		QString msg = QString::asprintf("Cursor at: (%.2f, %.2f)", projectedPosition.GetX(), projectedPosition.GetY());
		m_CursorProjectedPosition->setText(msg);

		ProcessInstrumentsMouseMoveEvent(event);
	}

	void MainWindow::OnRenderingWidgetMouseWheelEvent(QWheelEvent* event)
	{
		ProcessInstrumentsMouseWheelEvent(event);
	}

	void MainWindow::OnRenderingWidgetKeyboardEvent(QKeyEvent* event)
	{
		ProcessInstrumentsKeyboardEvent(event);
	}

	void MainWindow::OnInstrumentFinished(CityDraft::Input::Instruments::Instrument* instrument, CityDraft::Input::Instruments::FinishStatus status)
	{
		BOOST_ASSERT(instrument);
		m_Logger->debug("{} finished", instrument->GetName().toStdString());

		DeactivateInstrument(instrument);
	}

	void MainWindow::OnOpenSceneClicked()
	{
		QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/maps";

		QString filename = QFileDialog::getOpenFileName(
			this,
			"Open Scene",
			defaultPath,
			"Text Files (*.citydraft);;All Files (*)"
		);

		if (filename.isEmpty())
		{
			return;
		}

		std::filesystem::path path = filename.toStdString();
		if (!std::filesystem::is_regular_file(path))
		{
			QMessageBox::critical(this, "Filesystem Error", QString("Cannot load scene from %1: path does not point to a file").arg(filename));
			return;
		}
		const auto sceneLogger = Logging::LogManager::CreateLogger("Scene");
		auto sceneOpened = CityDraft::Scene::LoadFromFile(path, m_AssetManager, sceneLogger);
		if (!sceneOpened)
		{
			QMessageBox::critical(this, "Unknown Error", QString("Failed to load scene from %1: unknown error").arg(filename));
			return;
		}

		InitializeUiForScene(sceneOpened);
	}

	void MainWindow::OnNewSceneClicked()
	{
		m_UndoStack->clear();
		const auto sceneLogger = Logging::LogManager::CreateLogger("Scene");
		m_Scene = Scene::NewScene("New Scene", m_AssetManager, sceneLogger);
		
		m_ScenePath = "";
		InitializeUiForScene(m_Scene);
	}

	void MainWindow::OnSaveSceneAsClicked()
	{
		BOOST_ASSERT(m_Scene);

		QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		std::filesystem::create_directories(defaultPath.toStdString() + "/maps");
		defaultPath += "/maps/" + m_Scene->GetName() + ".citydraft";

		QString filename = QFileDialog::getSaveFileName(
			this,
			"Save Scene",
			defaultPath,
			"Text Files (*.citydraft);;All Files (*)"
		);

		if (filename.isEmpty())
		{
			return;
		}

		m_Scene->SaveToFile(filename.toStdString());
		setWindowTitle(QString::fromStdString(m_Scene->GetName()));
	}

	void MainWindow::OnGraphicsInitialized(UI::Rendering::SkiaWidget* widget)
	{
		BOOST_ASSERT(widget == m_RenderingWidget);

		CreateAssetManager(m_AssetsRootDirectory);
		CreateImageSelectionWidget();

		const auto sceneLogger = Logging::LogManager::CreateLogger("Scene");
		std::shared_ptr<CityDraft::Scene> scene;
		if (!m_ScenePath.isEmpty())
		{
			std::filesystem::path scenePath = m_ScenePath.toStdString();
			scene = Scene::LoadFromFile(scenePath, m_AssetManager, sceneLogger);
		}

		if (!m_Scene)
		{
			scene = Scene::NewScene("New Scene", m_AssetManager, sceneLogger);
		}

		BOOST_ASSERT(scene);
		InitializeUiForScene(scene);
	}

}
