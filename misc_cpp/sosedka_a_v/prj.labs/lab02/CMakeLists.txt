get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "." ProjectId ${ProjectId})
set(ProjectRealId "${ProjectId}")
set(ProjectId ${ParentProject}.${ProjectId})

project(${ProjectId})

set_output_directory()

file(GLOB src CONFIGURE_DEPENDS
        "*.h"
        "*.cpp"
        )

file(GLOB report CONFIGURE_DEPENDS
        "report.md.in.txt"
        )

add_executable(${PROJECT_NAME} ${src} ${report})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${ROOT_PATH}/prj.res/cat.jpg" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

file(READ main.cpp cpp_source)
report_post_build()

#run_post_build()