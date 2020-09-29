PROJECT(qitembox_lib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(${PROJECT_NAME} qitembox.h qitembox.cpp qitembox.ui)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

LIST(APPEND UIC_HDS_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include_Debug/"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include_Release/"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include/")
target_include_directories(${PROJECT_NAME} INTERFACE ${UIC_HDS_DIR})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)