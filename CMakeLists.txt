cmake_minimum_required(VERSION 3.5.0...3.31.2)
find_package(Git)
execute_process(
    COMMAND
        ${GIT_EXECUTABLE} describe --tags --dirty --abbrev=0 --match "v*.*.*"
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    COMMAND_ERROR_IS_FATAL ANY
)

string(REGEX REPLACE "^v" "" PROJECT_VERSION ${GIT_DESCRIBE_VERSION})
string(REGEX REPLACE "-dirty" ".1" PROJECT_VERSION ${PROJECT_VERSION})
project(TrendyAchievementSuggestions VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

find_package(rcgg-nexus-api)
find_package(nlohmann_json)
find_package(httplib)
find_package(enumflags)
find_package(imgui)

set(FALLBACK_DATA "${CMAKE_BINARY_DIR}/fallback_data.json")

file(DOWNLOAD
    "http://edge.gw2efficiency.com/tracking/unlocks?v=2021-07-24T00%3A00%3A00Z&id=achievements"
    ${FALLBACK_DATA}
    SHOW_PROGRESS
)

file(READ ${FALLBACK_DATA} FALLBACK_DATA_CONTENTS)

file(WRITE ${CMAKE_BINARY_DIR}/include/FallbackData.hpp "
// Auto-generated header. Do not edit manually.
#pragma once

static const char* UNLOCKS_DATA = R\"(${FALLBACK_DATA_CONTENTS})\";
")


include(${CMAKE_CURRENT_SOURCE_DIR}/src-re/GW2RE.cmake)
file(GLOB SRCs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]pp)
add_library(${PROJECT_NAME}
    SHARED
        ${SRCs}
        ${RE_SRCs}
)

target_link_libraries(${PROJECT_NAME}
    rcgg-nexus-api::rcgg-nexus-api
    nlohmann_json::nlohmann_json
    httplib::httplib
    EnumFlags::EnumFlags
    imgui::imgui
    ws2_32
    crypt32
    ${RE_LIBs}
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        ADDON_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        ADDON_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        ADDON_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        ADDON_VERSION_REVISION=$<IF:$<BOOL:${PROJECT_VERSION_TWEAK}>,${PROJECT_VERSION_TWEAK},0>
        ADDON_NAME="${PROJECT_NAME}"
)



target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/include
        ${RE_INCLUDE_DIRS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Trendy_Achievement_Suggestions")
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "."
)
