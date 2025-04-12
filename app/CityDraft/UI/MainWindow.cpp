#include "MainWindow.h"


namespace CityDraft
{

	MainWindow::MainWindow(QWidget* parent): QMainWindow(parent)
	{
		m_Ui.setupUi(this);

		m_Renderer = UI::Rendering::RenderingWidget::Factory("skia", this);
		setCentralWidget(m_Renderer);
	}

	MainWindow::~MainWindow()
	{
		
	}


}
