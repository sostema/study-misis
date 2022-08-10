PROJECT(Superpixel_lib)

add_library(${PROJECT_NAME} SHARED superpixel.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        PUBLIC_HEADER superpixel.h
        OUTPUT_NAME "superpixel"
        CXX_STANDARD 11
        )

target_include_directories(${PROJECT_NAME} PRIVATE .)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Debug
        LIBRARY
        DESTINATION dbg/lib
        PUBLIC_HEADER
        DESTINATION dbg/include)

install(TARGETS ${PROJECT_NAME}
        CONFIGURATIONS Release
        LIBRARY
        DESTINATION rel/lib
        PUBLIC_HEADER
        DESTINATION rel/include)