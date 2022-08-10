PROJECT(Histogram_lib)

add_library(${PROJECT_NAME} histogram.h histogram.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)