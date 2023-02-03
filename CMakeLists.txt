cmake_minimum_required (VERSION 3.0)

cmake_policy (SET CMP0071 NEW)

set (PROJECT terracon_client_desktop)
project (${PROJECT} VERSION 1.0.0 LANGUAGES CXX)

file (GLOB HEADERS
    SRC/*.h
    )

file (GLOB SOURCES
    SRC/*.cpp
    )

set (RESOURCES_DIR
    SRC/resources
    )

set (RESOURCES
    ${RESOURCES_DIR}/resources.qrc
    )

set (CMAKE_FIND_ROOT_PATH ${ENV_PATH})
set (CMAKE_PREFIX_PATH ${ENV_PATH})

# set output path for executables (".exe" and ".dll" files)                               
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/BIN/${TARGET_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

message (STATUS "Qt version:${QT}")

find_package (Qt6 REQUIRED COMPONENTS Widgets WebSockets Svg SvgWidgets)

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_AUTOMOC ON)

qt6_add_resources (QRC_SOURCES ${RESOURCES})

add_executable (${PROJECT} ${HEADERS} ${SOURCES} ${QRC_SOURCES})

target_link_libraries (${PROJECT} PRIVATE Qt6::Widgets Qt6::WebSockets Qt6::Svg Qt6::SvgWidgets)
