CMAKE_MINIMUM_REQUIRED(VERSION 3.17)
PROJECT(Mosaicpp VERSION 2.0 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")
include(cmake/build_prefix.cmake)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 11)
SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install")
list(APPEND CMAKE_INSTALL_RPATH
        ${CMAKE_INSTALL_PREFIX}/rel/lib
        ${CMAKE_INSTALL_PREFIX}/dbg/lib)

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

SET(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Mosaic_lib)

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        OUTPUT_NAME "mosaicpp"
        )

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        DESTINATION rel/bin)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Debug
        DESTINATION dbg/bin)

add_custom_target(install_${PROJECT_NAME}
        "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target install
        DEPENDS ${PROJECT_NAME}
        COMMENT "Installing ${PROJECT_NAME}")