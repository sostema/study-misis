macro(report_post_build)
    configure_file(${report} ${CMAKE_CURRENT_BINARY_DIR}/${ProjectRealId}.report.md @ONLY)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${ProjectRealId}.report.md" "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endmacro()

macro(run_post_build)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMAND "${PROJECT_NAME}"
            )
endmacro()

macro(set_output_directory)
    string(REPLACE "." "/" ProjectPath ${PROJECT_NAME})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ROOT_PATH}/bin.${PLATFORM}/${ProjectPath}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ROOT_PATH}/bin.${PLATFORM}.dbg/${ProjectPath}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ROOT_PATH}/bin.${PLATFORM}.rel/${ProjectPath}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${ROOT_PATH}/bin.${PLATFORM}.inforelease/${ProjectPath}")
endmacro()