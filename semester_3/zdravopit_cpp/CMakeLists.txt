CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
PROJECT(Zdravopit VERSION 0.5 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")
include(cmake/build_prefix.cmake)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

SET(APP_NAME ${PROJECT_NAME})

set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

SET(MODEL_URL "http://download.tensorflow.org/models/object_detection/ssd_resnet50_v1_fpn_shared_box_predictor_640x640_coco14_sync_2018_07_03.tar.gz" CACHE STRING "URL from where to download model. (OPTIONAL)")
OPTION(INIT_SETUP "Initialize default model setup." ON)
OPTION(PREPARE_MODEL "Prepare model via Python script." OFF)

SET(QT_PROJECT_PATH CACHE STRING "Path to QT directory.")
SET(OPENCV_PROJECT_PATH CACHE STRING "Path to OpenCV directory.")
LIST(APPEND CMAKE_PREFIX_PATH ${QT_PROJECT_PATH} ${OPENCV_PROJECT_PATH})

SET(SCRIPT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/lib/model_utils/scripts)
if (INIT_SETUP)
	execute_process(COMMAND ${CMAKE_COMMAND} -DMODEL_URL=${MODEL_URL} -DMODEL_ZIP=${CMAKE_BINARY_DIR}/model.tar.gz -DMODEL_FINAL_DIR=${CMAKE_CURRENT_BINARY_DIR}/model -P ${SCRIPT_FOLDER}/DownloadModel.cmake)
endif ()
if (PREPARE_MODEL)
	execute_process(COMMAND ${CMAKE_COMMAND} -DMODEL_DIR=${CMAKE_CURRENT_BINARY_DIR}/model -DSCRIPT_PATH=${SCRIPT_FOLDER}/tf_text_graph_ssd.py -P ${SCRIPT_FOLDER}/PrepareModel.cmake)
endif()

find_package(Doxygen)
IF(DOXYGEN_FOUND)
	add_subdirectory(doc)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

SET(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res")
SET(APPLICATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/app")
SET(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

SET(UI_H_DIR "${CMAKE_CURRENT_BINARY_DIR}/lib/ui_h")

add_subdirectory(res)
add_subdirectory(lib)
add_subdirectory(app)