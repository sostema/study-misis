SET(MODEL_DIR CACHE STRING "Path to model directory.")
SET(SCRIPT_PATH CACHE STRING "Path to script preparing script (for example, tf_text_graph_ssd.py).")

include(FindPython3)
IF (NOT ${Python3_FOUND})
    MESSAGE("Python3 wasn't found, preparing model is unavailable...")
else ()
    execute_process(COMMAND ${Python3_EXECUTABLE} -c "import cv2" OUTPUT_VARIABLE CHECK_MODULE_CV2)
    #execute_process(COMMAND ${Python3_EXECUTABLE} -c "import tensorflow" OUTPUT_VARIABLE CHECK_MODULE_TENSORFLOW)
    if ("${CHECK_MODULE_CV2}" STREQUAL "")
        if (EXISTS ${MODEL_DIR})
            MESSAGE("Preparing model...")
            execute_process(COMMAND ${Python3_EXECUTABLE} ${SCRIPT_PATH}
                    --input ${MODEL_DIR}/model.pb
                    --output ${MODEL_DIR}/model.pbtxt
                    --config ${MODEL_DIR}/pipeline.config)
            MESSAGE("Model is prepared.")
        else ()
            MESSAGE("There seems to be no ${MODEL_DIR}...")
        endif ()
    else ()
        MESSAGE("Unfortunately, there is no OpenCV module in your Python Interpreter, thus we can't prepare model.")
    endif ()

endif ()