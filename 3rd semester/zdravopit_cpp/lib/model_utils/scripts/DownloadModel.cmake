set(MODEL_URL CACHE STRING "URL from where to download model.")
set(MODEL_ZIP CACHE FILEPATH "Path where to download model to.")
set(MODEL_FINAL_DIR CACHE PATH "Path to directory where model will be stored.")

IF (NOT EXISTS ${MODEL_ZIP})
	MESSAGE("Downloading model to ${MODEL_ZIP}.")
	file(DOWNLOAD ${MODEL_URL} ${MODEL_ZIP} SHOW_PROGRESS)
endif()

IF (EXISTS ${MODEL_ZIP} AND IS_DIRECTORY ${MODEL_ZIP})
	FILE(REMOVE_RECURSE ${MODEL_FINAL_DIR})
	FILE(MAKE_DIRECTORY ${MODEL_FINAL_DIR})
else()
	FILE(MAKE_DIRECTORY ${MODEL_FINAL_DIR})
endif()

SET(TEMP_DIR ${CMAKE_BINARY_DIR}/TEMP)
IF (EXISTS ${TEMP_DIR} AND IS_DIRECTORY ${TEMP_DIR})
	FILE(REMOVE_RECURSE ${TEMP_DIR})
	FILE(MAKE_DIRECTORY ${TEMP_DIR})
else()
	FILE(MAKE_DIRECTORY ${TEMP_DIR})
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E tar zxf ${MODEL_ZIP}
        WORKING_DIRECTORY ${TEMP_DIR})
file(GLOB MODEL_GRAPH ${TEMP_DIR}/*/frozen_inference_graph.pb)
file(RENAME ${MODEL_GRAPH} ${MODEL_FINAL_DIR}/model.pb)
FILE(REMOVE_RECURSE ${TEMP_DIR})