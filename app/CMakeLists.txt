
find_package(Qt6
    COMPONENTS
        Core
        Gui
        Widgets
		OpenGL
		OpenGLWidgets
)

find_package(unofficial-skia REQUIRED)

qt_standard_project_setup()

qt_add_executable (
    CityDraft 
    "CityDraft/main.cpp" 
	"CityDraft/UI/MainWindow.cpp"
	"CityDraft/UI/Rendering/SkiaWidget.cpp"
	"CityDraft/UI/Rendering/RenderingWidget.cpp"
)

qt_add_resources(CityDraft "Resources.qrc")

target_link_libraries(CityDraft PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
	Qt6::OpenGL
	Qt6::OpenGLPrivate
	Qt6::OpenGLWidgets
	Qt6::OpenGLWidgetsPrivate
	unofficial::skia::skia
)

set_property(TARGET CityDraft PROPERTY CXX_STANDARD 20)
set_property(TARGET CityDraft PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

qt_generate_deploy_app_script(
    TARGET CityDraft
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
