
set(AppName CityDraft.Gui)

find_package(Qt6
    COMPONENTS
        Core
        Gui
        Widgets
		OpenGL
		OpenGLWidgets
)

find_package(unofficial-skia REQUIRED)
find_package(spdlog REQUIRED)

qt_standard_project_setup()

qt_add_executable (
    ${AppName} 
    "CityDraft/main.cpp" 
	"CityDraft/UI/MainWindow.cpp"
	"CityDraft/UI/Rendering/SkiaWidget.cpp"
	"CityDraft/Logging/LogManager.cpp"
	"CityDraft/Assets/SkiaAssetManager.cpp"
	"CityDraft/Assets/SkiaImage.cpp"
	"CityDraft/Drafts/SkiaImage.cpp"
)

qt_add_resources(${AppName} "Resources.qrc")

target_link_libraries(${AppName} PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
	Qt6::OpenGL
	Qt6::OpenGLPrivate
	Qt6::OpenGLWidgets
	Qt6::OpenGLWidgetsPrivate
	spdlog::spdlog
	unofficial::skia::skia
	CityDraft
)

find_package(Boost REQUIRED COMPONENTS assert)
target_link_libraries(${AppName} PUBLIC Boost::assert)

target_include_directories(${AppName} PRIVATE ./)

set_property(TARGET ${AppName} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${AppName} PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

qt_generate_deploy_app_script(
    TARGET ${AppName}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
