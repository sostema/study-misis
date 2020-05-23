PROJECT(Object_Detection_Lib)

add_library(object_detection_lib object_detection.h object_detection.cpp)
target_link_libraries(object_detection_lib ${OpenCV_LIBS})

set_property(TARGET object_detection_lib PROPERTY CXX_STANDARD 17)