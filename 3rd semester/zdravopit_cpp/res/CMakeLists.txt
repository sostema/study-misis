execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/res_template.qrc ${PROJECT_BINARY_DIR}/model/res.qrc
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/model/labels.txt ${PROJECT_BINARY_DIR}/model/labels.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/model/urls.txt ${PROJECT_BINARY_DIR}/model/urls.txt)

if ((EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/model) AND (NOT INIT_SETUP))
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/model/model.pb ${PROJECT_BINARY_DIR}/model/model.pb
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/model/model.pbtxt ${PROJECT_BINARY_DIR}/model/model.pbtxt)
endif ()

if ((EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/model) AND (INIT_SETUP))
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/model/model.pbtxt ${PROJECT_BINARY_DIR}/model/model.pbtxt)
endif()