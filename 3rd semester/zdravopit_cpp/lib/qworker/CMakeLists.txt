PROJECT(qworker_lib)

set(CMAKE_AUTOMOC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${LIB_DIR})

add_library(${PROJECT_NAME} qworker.h qworker.cpp)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets object_detection_lib)

SET(UIC_HDS_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include")
target_include_directories(${PROJECT_NAME} INTERFACE ${UIC_HDS_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)