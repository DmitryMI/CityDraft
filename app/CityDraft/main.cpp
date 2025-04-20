#include <QApplication>
#include <QWidget>
#include <QCommandLineParser>
#include "UI/MainWindow.h"
#include "Logging/LogManager.h"
#include "spdlog/spdlog.h"
#include <QStandardPaths>

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

	QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

	QCommandLineOption assetsRootOption(
		"assets-root",
		"Set path to assets root directory",
		"assets-root",
		appDataPath + "/assets"
	);
	parser.addOption(assetsRootOption);

	QCommandLineOption sceneOption(
		"scene",
		"Load scene from this path after startup",
		"scene"
	);
	parser.addOption(sceneOption);

	// Process the command line arguments
	parser.process(app);

	QString levelStr = parser.value(logLevelOption).toLower();
	Logging::LogManager::InitLogging(levelStr);

	QString assetsRoot = parser.value(assetsRootOption);
	QString sceneRoot = parser.value(sceneOption);

	spdlog::info("CityDraft started");

    UI::MainWindow mainWindow(assetsRoot, sceneRoot);
    mainWindow.show();

    return app.exec();
}
