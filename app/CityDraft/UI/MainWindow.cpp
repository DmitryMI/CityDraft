#include "MainWindow.h"


namespace CityDraft::UI
{

	MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
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

		// Determine which widget to use
		m_Renderer = UI::Rendering::RenderingWidget::Factory("skia", this);

		// Replace placeholder with the actual widget
		QBoxLayout* layout = dynamic_cast<QBoxLayout*>(placeholder->parentWidget()->layout());
		int index = layout->indexOf(placeholder);
		layout->removeWidget(placeholder);
		delete placeholder;
		
		layout->insertWidget(index, m_Renderer);
	}


}
