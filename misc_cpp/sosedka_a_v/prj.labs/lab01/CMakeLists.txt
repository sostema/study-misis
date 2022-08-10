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

file(READ main.cpp cpp_source)

#run_post_build()
report_post_build()