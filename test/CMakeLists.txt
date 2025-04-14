cmake_minimum_required(VERSION 3.20)

project(CityDraft.Test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CityDraft.Test.Sources 
	"CityDraft/Assets/AssetManagerTest.cpp"
	"CityDraft/LogManager.cpp"
	"CityDraft/LogManager.h"
)

enable_testing()

add_executable(CityDraft.Test ${CityDraft.Test.Sources} )
target_link_libraries(CityDraft.Test PRIVATE CityDraft)

find_package(spdlog REQUIRED)
target_link_libraries(CityDraft.Test PRIVATE spdlog::spdlog)

find_package(gtest REQUIRED)
target_link_libraries(CityDraft.Test PRIVATE GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(CityDraft.Test)

set_property(TARGET CityDraft.Test PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
