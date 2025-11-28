cmake_minimum_required(VERSION 3.15)
project(ara-diag LANGUAGES CXX)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif()

set(ARA_ROOT "")
# Support two layouts:
# 1) Local development: this CMakeLists lives in `buildconfig/` and sources are in `../dev`.
# 2) Conan create: the recipe files are copied into the Conan `source_folder` root, and
#    `source()` in the recipe copies `dev/` into that root. In that case `dev/` is a child
#    of this CMakeLists directory.
if(DEFINED ARA_DEV_ROOT AND EXISTS "${ARA_DEV_ROOT}")
    set(ARA_ROOT "${ARA_DEV_ROOT}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/dev")
    set(ARA_ROOT "${CMAKE_CURRENT_LIST_DIR}")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../dev")
    set(ARA_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")
else()
    message(FATAL_ERROR "Could not locate 'dev' directory relative to ${CMAKE_CURRENT_LIST_DIR} and ARA_DEV_ROOT is not set or invalid")
endif()

message(STATUS "ARA root: ${ARA_ROOT}")
file(GLOB_RECURSE ARA_SOURCES
    "${ARA_ROOT}/dev/src/*.cpp"
)

message(STATUS "Found ARA sources: ${ARA_SOURCES}")
if(NOT ARA_SOURCES)
    message(FATAL_ERROR "No source files found for ara-diag in ${ARA_ROOT}/dev/src")
endif()

add_library(ara-diag SHARED ${ARA_SOURCES})
target_include_directories(ara-diag PUBLIC "${ARA_ROOT}/dev/inc/public")

# ara-diag is intentionally independent and does not link to diagnostic-manager here.

set_target_properties(ara-diag PROPERTIES POSITION_INDEPENDENT_CODE ON)

install(TARGETS ara-diag
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin)
install(DIRECTORY "${ARA_ROOT}/dev/inc/public/"
    DESTINATION include
    FILES_MATCHING PATTERN "*.h")

set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(ARA_DIAG_PUBLIC_INC "${PROJECT_ROOT}/ara-diag/dev/inc/public")
target_include_directories(ara-diag PUBLIC "${ARA_DIAG_PUBLIC_INC}")
