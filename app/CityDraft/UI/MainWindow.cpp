#include "MainWindow.h"


namespace CityDraft::UI
{

	MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
	{
		m_Ui.setupUi(this);

		m_Renderer = UI::Rendering::RenderingWidget::Factory("skia", this);
		setCentralWidget(m_Renderer);
		spdlog::info("MainWindow created");
	}

	MainWindow::~MainWindow()
	{
		
	}


}
