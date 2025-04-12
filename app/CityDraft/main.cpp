#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>
#include "UI/MainWindow.h"
#include "Logging/LogManager.h"
#include "spdlog/spdlog.h"

using namespace CityDraft;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
	QApplication::setApplicationName("CityDraft");
	QCoreApplication::setApplicationVersion("1.0");

	QCommandLineParser parser;
	parser.setApplicationDescription("CityDraft Help");
	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption logLevelOption(
		"log-level",
		"Set spdlog log level (options: trace, debug, info, warn, error, critical, off)",
		"level",
		"info"
	);
	parser.addOption(logLevelOption);

	// Process the command line arguments
	parser.process(app);

	QString levelStr = parser.value(logLevelOption).toLower();
	Logging::LogManager::InitLogging(levelStr);
	spdlog::info("CityDraft started");

    UI::MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
