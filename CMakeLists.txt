cmake_minimum_required (VERSION 3.20)

set(VCPKG_OVERLAY_TRIPLETS "vcpkg_overlays/triplets")

if(WIN32)
	set(VCPKG_TARGET_TRIPLET "x64-windows-static")
	set(VCPKG_HOST_TRIPLET "x64-windows-static")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CityDraft")

enable_testing()

add_subdirectory("src")
add_subdirectory("test")
add_subdirectory("app")