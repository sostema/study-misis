cmake_minimum_required(VERSION 3.13)
PROJECT(sosedka_a_v VERSION 0.1 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# Build prefix cmake configuration
include(cmake/build_prefix.cmake)
# Constants and variables to use in your project
include(cmake/config.cmake)
# Macros to use in your project
include(cmake/macros.cmake)
# Custom module to create target to create
# pdf files from built markdown reports
include(cmake/Modules/markdown_pdf/markdown_pdf.cmake)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(prj.res)
add_subdirectory(prj.labs)