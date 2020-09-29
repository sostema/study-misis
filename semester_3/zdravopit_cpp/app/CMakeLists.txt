set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${LIB_DIR})
include_directories(${PROJECT_BINARY_DIR})

if (WIN32)
    add_executable(${APP_NAME} WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Core> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Gui> $<TARGET_FILE_DIR:${PROJECT_NAME}>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::Widgets> $<TARGET_FILE_DIR:${PROJECT_NAME}> )
    get_target_property(__dll_dbg opencv_world IMPORTED_LOCATION_DEBUG)
    get_target_property(__dll_release opencv_world  IMPORTED_LOCATION_RELEASE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event the TARGET
    COMMAND ${CMAKE_COMMAND} -E copy_if_different           # which executes "cmake - E copy_if_different..."
    "$<$<CONFIG:debug>:${__dll_dbg}>$<$<CONFIG:release>:${__dll_release}>"      # <--this is in-file
    $<TARGET_FILE_DIR:${PROJECT_NAME}>                        # <--this is out-file path
        # another dll copy if needed here
    COMMENT "    [1718_34_ATK] copy dlls realsense2 and opencv_world")
elseif (APPLE)
    add_executable(${APP_NAME} MACOSX_BUNDLE)
    set_target_properties(${APP_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_FRAMEWORK_IDENTIFIER org.cmake.ExecutableTarget
            MACOSX_BUNDLE_INFO_PLIST ${RESOURCES_DIR}/MacOSXBundleInfo.plist.in

            # This property is required:
            MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_NAME}-${PROJECT_VERSION}"

            # Those properties are not required:
            MACOSX_BUNDLE_INFO_STRING "${APP_NAME} ${PROJECT_VERSION}"
            MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION}
            MACOSX_BUNDLE_BUNDLE_NAME "${APP_NAME}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
            MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
            )
else ()
    add_executable(${APP_NAME})
endif ()

set(SRC_FILES main.cpp mainwindow.cpp mainwindow.h mainwindow.ui)
if(COMMAND qt5_add_big_resources)
    qt5_add_big_resources(RESOURCE_FILES ${PROJECT_BINARY_DIR}/model/res.qrc)
else()
    set(RESOURCE_FILES ${PROJECT_BINARY_DIR}/model/res.qrc)
endif()
target_sources(${APP_NAME} PUBLIC ${SRC_FILES} ${RESOURCE_FILES})
target_link_libraries(${APP_NAME} Qt5::Widgets ${OpenCV_LIBS} qworker_lib qitembox_lib)

set_property(TARGET ${APP_NAME} PROPERTY CXX_STANDARD 17)
