CMAKE_MINIMUM_REQUIRED(VERSION 3.19)
PROJECT(Fingernote VERSION 0.8 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")
include(cmake/build_prefix.cmake)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

SET(OPENCV_PROJECT_PATH CACHE STRING "Path to OpenCV directory.")
SET(GENERATE_DOCUMENTATION OFF CACHE BOOL "Generate doxygen docs.")
LIST(APPEND CMAKE_PREFIX_PATH ${OPENCV_PROJECT_PATH})

find_package(Doxygen)
IF (DOXYGEN_FOUND AND GENERATE_DOCUMENTATION)
    add_subdirectory(doc)
endif ()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
