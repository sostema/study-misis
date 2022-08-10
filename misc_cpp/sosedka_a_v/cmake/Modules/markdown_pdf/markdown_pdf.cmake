add_custom_target(markdown2pdf ALL)

FILE(GLOB_RECURSE reports "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/**/*.md")
IF(reports)
    foreach(report IN ITEMS ${reports})
        string(REPLACE ".md" ".pdf" new_report ${report})
        add_custom_command(TARGET markdown2pdf
                COMMAND pandoc ${report} -s -o ${new_report})
    endforeach()
ENDIF()