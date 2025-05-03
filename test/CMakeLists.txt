cmake_minimum_required(VERSION 3.20)

project(CityDraft.Test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CityDraft.Test.Sources 
	"CityDraft/Assets/AssetManagerTest.cpp"
	"CityDraft/AnglesTest.cpp" 
	"CityDraft/LinearColorTest.cpp"
	"CityDraft/Drafts/Properties/PropertiesTest.cpp"
)

enable_testing()

add_executable(CityDraft.Test ${CityDraft.Test.Sources} )
target_link_libraries(CityDraft.Test PRIVATE CityDraft)
target_include_directories(CityDraft.Test PRIVATE "./")

find_package(spdlog REQUIRED)
target_link_libraries(CityDraft.Test PRIVATE spdlog::spdlog)

find_package(gtest REQUIRED)
target_link_libraries(CityDraft.Test PRIVATE GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(CityDraft.Test)

if(MSVC)
	target_compile_options(CityDraft PUBLIC /EHa)
endif()

set_property(TARGET CityDraft.Test PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
