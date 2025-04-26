set(
	CityDraft.Sources
	"CityDraft/Scene.cpp"
	"CityDraft/Drafts/Image.cpp"
	"CityDraft/Drafts/Draft.cpp"
	"CityDraft/Assets/Asset.cpp"
	"CityDraft/Assets/Image.cpp"
	"CityDraft/Assets/AssetManager.cpp"
	"CityDraft/Utils/ImageLoader.cpp"
	"CityDraft/Serialization/BoostArchive.cpp"
	
 "CityDraft/Utils/ImageUtils.cpp" "CityDraft/Curves/LineSegment.cpp" "CityDraft/Drafts/Curve.cpp")

add_library(CityDraft ${CityDraft.Sources})
target_include_directories(CityDraft PUBLIC "../public")
set_property(TARGET CityDraft PROPERTY CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS signals2 url assert geometry math serialization)
target_link_libraries(CityDraft PUBLIC Boost::signals2 Boost::url Boost::assert Boost::geometry Boost::math Boost::serialization)

find_package(spdlog REQUIRED)
target_link_libraries(CityDraft PUBLIC spdlog::spdlog)

find_package(Stb REQUIRED)
target_include_directories(CityDraft PUBLIC ${Stb_INCLUDE_DIR})

if(MSVC)
	target_compile_options(CityDraft PUBLIC /EHa)
endif()

set_property(TARGET CityDraft PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
