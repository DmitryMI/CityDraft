set(
	CityDraft.Sources
	"CityDraft/DraftScene.cpp"
	"CityDraft/DraftObject.cpp"
	"CityDraft/Assets/Asset.cpp"
	"CityDraft/Assets/ImageAsset.cpp"
	"CityDraft/Assets/AssetManager.cpp"
)

add_library(CityDraft ${CityDraft.Sources})
target_include_directories(CityDraft PUBLIC "../public")
set_property(TARGET CityDraft PROPERTY CXX_STANDARD 20)

find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(CityDraft PUBLIC Eigen3::Eigen)

find_package(Boost REQUIRED COMPONENTS signals2 url assert)
target_link_libraries(CityDraft PUBLIC Boost::signals2 Boost::url Boost::assert)

find_package(spdlog REQUIRED)
target_link_libraries(CityDraft PUBLIC spdlog::spdlog)

if(MSVC)
	target_compile_options(CityDraft PUBLIC /EHa)
endif()

set_property(TARGET CityDraft PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
