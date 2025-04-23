#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Logging/LogManager.h"
#include <qstatusbar.h>
#include "CityDraft/Input/Instruments/Panner.h"
#include "CityDraft/Input/Instruments/Selector.h"
#include "CityDraft/Input/Utils.h"
#include <algorithm>
#include "CityDraft/Logging/LogManager.h"
#include "CityDraft/UI/Colors/Factory.h"
#include "CityDraft/Input/Instruments/ImageDraftEditor.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

namespace CityDraft::UI {

MainWindow::MainWindow(QString assetsRoot, QWidget* parent)
    : QMainWindow(parent),
      m_AssetsRootDirectory(std::move(assetsRoot)) {

	MainWindow::MainWindow(const QString& assetsRoot, const QString& scenePath, QWidget* parent):
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

		m_Logger->info("MainWindow created");
	}

	MainWindow::~MainWindow() = default;

void MainWindow::ReplacePlaceholdersWithSplitter() {
    if (!m_ImageSelectionWidget) {
        m_ImageSelectionWidget = new ImageSelectionWidget(this);
    }

		auto* splitter = new QSplitter(Qt::Horizontal, this);
		splitter->addWidget(m_ImageSelectionWidget);
		splitter->addWidget(m_RenderingWidget);
		splitter->setStretchFactor(0, 0);
		splitter->setStretchFactor(1, 1);
		splitter->setCollapsible(0, false);
		splitter->setCollapsible(1, false);
		splitter->setSizes({ 230, 774 });

		QWidget* imagePlaceholder = m_Ui.imageSelectionPlaceholder;

		auto* layout = dynamic_cast<QBoxLayout*>(imagePlaceholder->parentWidget()->layout());
		if (!layout) {
			qWarning("Placeholder layout is not a QBoxLayout!");
			return;
		}

		layout->removeWidget(imagePlaceholder);
		delete imagePlaceholder;

		layout->addWidget(splitter);
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

		QBoxLayout* layout = dynamic_cast<QBoxLayout*>(m_Ui.renderingWidgetPlaceholder->parentWidget()->layout());
		int index = layout->indexOf(m_Ui.renderingWidgetPlaceholder);
		layout->removeWidget(m_Ui.renderingWidgetPlaceholder);
		delete m_Ui.renderingWidgetPlaceholder;
		m_Ui.renderingWidgetPlaceholder = nullptr;

		layout->insertWidget(index, m_RenderingWidget);
	}

	void MainWindow::LoadImagesToSelectionWidget() const
	{
		std::vector<std::shared_ptr<Assets::ImageVariantGroup>> variantImageGroups;

		for (const auto& group : m_AssetManager->GetVariantImages()) {
			variantImageGroups.push_back(group);
		}


		std::vector<std::shared_ptr<Assets::Image>> invariantImages;

		for (const auto& image : m_AssetManager->GetInvariantImages()) {
			invariantImages.push_back(image);
		}

    std::vector<std::shared_ptr<Assets::ImageVariantGroup>> variantImageGroups;

    for (const auto& group : m_AssetManager->GetVariantImages()) {
        variantImageGroups.push_back(group);
    }

		m_ImageSelectionWidget->loadImagesFromAssets(imageAssets);

		QWidget* placeholder = m_Ui.imageSelectionPlaceholder;
		auto* layout = dynamic_cast<QBoxLayout*>(placeholder->parentWidget()->layout());

		const int index = layout->indexOf(placeholder);
		layout->removeWidget(placeholder);
		delete placeholder;

		layout->insertWidget(index, m_ImageSelectionWidget);
	}

	void MainWindow::CreateAssetManager(const QString& assetsRoot)
	{
		auto assetManagerLogger = CityDraft::Logging::LogManager::CreateLogger("Assets");
		std::filesystem::path assetsRootPath(assetsRoot.toStdString());
		m_AssetManager = std::make_shared<CityDraft::Assets::SkiaAssetManager>(assetsRootPath, assetManagerLogger, m_RenderingWidget->GetDirectContext(), m_RenderingWidget->GetGlFunctions());
		BOOST_ASSERT(m_AssetManager);
		m_AssetManager->LoadAssetInfos(assetsRootPath, true);
		CreateImageSelectionWidget();
	}

	void MainWindow::CreateStatusBar()
	{
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
		for (const auto[descryptor, description] : toolDescriptions)
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

		m_UndoStack->clear();
		m_Scene = sceneOpened;
		m_ScenePath = filename;
		m_RenderingWidget->SetScene(m_Scene);
		CreateInstruments();
	}

	void MainWindow::OnNewSceneClicked()
	{
		m_UndoStack->clear();
		const auto sceneLogger = Logging::LogManager::CreateLogger("Scene");
		m_Scene = Scene::NewScene("New Scene", m_AssetManager, sceneLogger);
		m_ScenePath = "";
		m_RenderingWidget->SetScene(m_Scene);
		m_RenderingWidget->Repaint();
		CreateInstruments();
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

		const auto sceneLogger = Logging::LogManager::CreateLogger("Scene");
		if (!m_ScenePath.isEmpty())
		{
			std::filesystem::path scenePath = m_ScenePath.toStdString();
			m_Scene = Scene::LoadFromFile(scenePath, m_AssetManager, sceneLogger);
		}

		if (!m_Scene)
		{
			m_Scene = Scene::NewScene("New Scene", m_AssetManager, sceneLogger);
		}

		BOOST_ASSERT(m_Scene);
		setWindowTitle(QString::fromStdString(m_Scene->GetName()));
		m_RenderingWidget->SetScene(m_Scene);
		CreateInstruments();
	}

}
