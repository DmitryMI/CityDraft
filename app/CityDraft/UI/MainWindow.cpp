#include "MainWindow.h"
#include "CityDraft/Assets/AssetManager.h"
#include "CityDraft/Logging/LogManager.h"
#include "Rendering/ImageSelectionWidget.h"

#include <QSplitter>
#include <QLabel>
#include <QBoxLayout>
#include <QStatusBar>
#include <utility>

namespace CityDraft::UI {

MainWindow::MainWindow(QString assetsRoot, QWidget* parent)
    : QMainWindow(parent),
      m_AssetsRootDirectory(std::move(assetsRoot)) {

    m_Ui.setupUi(this);
    m_KeyBindingProvider = Input::Factory::CreateKeyBindingProvider();

    ReplacePlaceholdersWithSplitter();
    CreateStatusBar();

    spdlog::info("MainWindow created");
}

MainWindow::~MainWindow() = default;

void MainWindow::ReplacePlaceholdersWithSplitter() {
    m_ImageSelectionWidget = new ImageSelectionWidget(this);
    m_RenderingWidget = new Rendering::SkiaWidget(m_KeyBindingProvider, this);

    connect(m_RenderingWidget, &Rendering::SkiaWidget::GraphicsInitialized,
            this, &MainWindow::OnGraphicsInitialized);
    connect(m_RenderingWidget, &Rendering::SkiaWidget::CursorPositionChanged,
            this, &MainWindow::OnCursorProjectedPositionChanged);

    auto* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_ImageSelectionWidget);
    splitter->addWidget(m_RenderingWidget);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    splitter->setSizes({230, 774});

    QWidget* imagePlaceholder = m_Ui.imageSelectionPlaceholder;
    QWidget* renderPlaceholder = m_Ui.renderingWidgetPlaceholder;

    auto* layout = dynamic_cast<QBoxLayout*>(imagePlaceholder->parentWidget()->layout());
    if (!layout) {
        qWarning("Placeholder layout is not a QBoxLayout!");
        return;
    }

    layout->removeWidget(imagePlaceholder);
    layout->removeWidget(renderPlaceholder);
    delete imagePlaceholder;
    delete renderPlaceholder;

    layout->addWidget(splitter);
}

void MainWindow::CreateAssetManager(const QString& assetsRoot) {
    auto assetManagerLogger = Logging::LogManager::CreateLogger("Assets");
    std::filesystem::path assetsRootPath(assetsRoot.toStdString());

    m_AssetManager = std::make_shared<Assets::SkiaAssetManager>(
        assetsRootPath,
        assetManagerLogger,
        m_RenderingWidget->GetDirectContext(),
        m_RenderingWidget->GetGlFunctions()
    );

    BOOST_ASSERT(m_AssetManager);
    m_AssetManager->LoadAssets(assetsRootPath, true);

    LoadImagesToSelectionWidget();
}

    void MainWindow::LoadImagesToSelectionWidget() const
{
    std::vector<std::shared_ptr<Assets::Image>> invariantImages;

    for (const auto& image : m_AssetManager->GetInvariantImages()) {
        invariantImages.push_back(image);
    }

    std::vector<std::shared_ptr<Assets::ImageVariantGroup>> variantImageGroups;

    for (const auto& group : m_AssetManager->GetVariantImages()) {
        variantImageGroups.push_back(group);
    }


    m_ImageSelectionWidget->loadImagesFromAssets(invariantImages, variantImageGroups);
}

void MainWindow::CreateStatusBar() {
    m_CursorProjectedPosition = new QLabel("Cursor at: N/A");
    statusBar()->addPermanentWidget(m_CursorProjectedPosition);
}

void MainWindow::OnCursorProjectedPositionChanged(const QPointF& projectedPosition) const {
    const QString msg = QString::asprintf("Cursor at: (%.2f, %.2f)", projectedPosition.x(), projectedPosition.y());
    m_CursorProjectedPosition->setText(msg);
}

void MainWindow::OnGraphicsInitialized(const Rendering::SkiaWidget* widget) {
    BOOST_ASSERT(widget == m_RenderingWidget);

    CreateAssetManager(m_AssetsRootDirectory);
    m_Scene = Scene::LoadSceneFromFile("mock_file.json", m_AssetManager, Logging::LogManager::CreateLogger("Scene"));
    m_RenderingWidget->SetScene(m_Scene);
}

}