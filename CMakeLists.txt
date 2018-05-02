# CMakeList.txt : CMake project for CMakeProject1, include source and define
# project specific logic here.
#
# Make sure that the build is always 64 bit

cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add source to this project's executable.
add_executable (CMakeProject1 "CMakeProject1.cpp" "CMakeProject1.h")

find_package(VTK REQUIRED)
find_package(Qt5Core REQUIRED)
target_include_directories(CMakeProject1 SYSTEM PUBLIC "${VTK_INCLUDE_DIRECTORIES}")
target_include_directories(CMakeProject1 SYSTEM PUBLIC "${Qt5Core_INCLUDE_DIRECTORIES}")
target_link_libraries(CMakeProject1 "${VTK_LIBRARIES}")
target_link_libraries(CMakeProject1 "${Qt5Core_LIBRARIES}")

include("${VTK_USE_FILE}")
# TODO: Add tests and install targets if needed.
