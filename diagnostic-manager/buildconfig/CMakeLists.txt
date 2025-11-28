cmake_minimum_required(VERSION 3.5)
project(diagnostic-manager VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# If Conan generated the helper, include it
if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup()
endif()

# Project root is parent of this buildconfig folder
set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(DM_INCLUDE_DIR "${PROJECT_ROOT}/dev/inc")
set(ARA_DIAG_PUBLIC_INC "${PROJECT_ROOT}/ara-diag/dev/inc/public")


# Collect implementation sources under dev/src in project root
file(GLOB_RECURSE DM_SOURCES
     CONFIGURE_DEPENDS
     "${PROJECT_ROOT}/dev/src/*.cpp"
     "${PROJECT_ROOT}/dev/src/*.cc"
     "${PROJECT_ROOT}/dev/src/*.c")

if(DM_SOURCES)
  # Build diagnostic-manager as an executable (binary)
  add_executable(diagnostic-manager ${DM_SOURCES})
  target_include_directories(diagnostic-manager PUBLIC
    "${ARA_DIAG_PUBLIC_INC}"
    "${DM_INCLUDE_DIR}"
  )
  # Link to ara-diag only if provided by Conan
  if(TARGET CONAN_PKG::ara-diag)
    target_link_libraries(diagnostic-manager PUBLIC CONAN_PKG::ara-diag)
  else()
    message(STATUS "Building diagnostic-manager without linking ara-diag")
  endif()
else()
  message(STATUS "No implementation sources found under ${PROJECT_ROOT}/dev/src — creating INTERFACE target (header-only).")
  add_library(diagnostic-manager INTERFACE)
  target_include_directories(diagnostic-manager INTERFACE "${DM_INCLUDE_DIR}")
endif()

install(DIRECTORY "${DM_INCLUDE_DIR}/" DESTINATION include FILES_MATCHING PATTERN "*.h")
if(TARGET diagnostic-manager AND NOT "${CMAKE_PROJECT_NAME}" STREQUAL "ara-diag")
  install(TARGETS diagnostic-manager RUNTIME DESTINATION bin)
endif()

enable_testing()
find_package(GTest REQUIRED)

file(GLOB TEST_SOURCES "${PROJECT_ROOT}/dev/tests/*.cpp")
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests PRIVATE diagnostic-manager GTest::gtest_main)
target_include_directories(run_tests PRIVATE
    "${DM_INCLUDE_DIR}"
    "${ARA_DIAG_PUBLIC_INC}"
)
add_test(NAME AllTests COMMAND run_tests)
