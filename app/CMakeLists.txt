option(CITYDRAFT_NO_TERMINAL "Disable terminal window" OFF)


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

set(AppSources
	"CityDraft/main.cpp" 
	"CityDraft/UI/MainWindow.cpp"
	"CityDraft/UI/Rendering/SkiaWidget.cpp"
	"CityDraft/UI/Colors/Factory.cpp"
	"CityDraft/Logging/LogManager.cpp"
	"CityDraft/Assets/SkiaAssetManager.cpp"
	"CityDraft/Assets/SkiaImage.cpp"
	"CityDraft/Drafts/SkiaImage.cpp"
	"CityDraft/Input/Factory.cpp"
	"CityDraft/Input/Instruments/Instrument.cpp"
	"CityDraft/Input/Instruments/Panner.cpp"
	"CityDraft/Input/Instruments/Selector.cpp"
	"CityDraft/Input/Instruments/ImageDraftEditor.cpp"
	"CityDraft/UI/Rendering/SkiaPainters/Image.cpp"
	"CityDraft/UI/Rendering/SkiaPainters/Rect.cpp"
	"CityDraft/UI/Rendering/SkiaPainters/Circle.cpp"
)

if(${CITYDRAFT_NO_TERMINAL})
	qt_add_executable (
		${AppName}
		WIN32
		${AppSources}
	)
else()
	qt_add_executable (
		${AppName}
		${AppSources}
	)
endif()

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

if(MSVC)
	target_compile_options(CityDraft PUBLIC /EHa)
endif()

install(SCRIPT ${deploy_script})
