option(TEST_SOLUTION "Build solution" OFF)

include(FetchContent)
# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

function(filter_existing OUT_VAR)
  foreach(_file ${ARGN})
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${_file})
      list(APPEND RESULT ${_file})
    endif()
  endforeach()
  set(${OUT_VAR} ${RESULT} PARENT_SCOPE)
endfunction()

function(patch_include_directories TARGET)
  if (TEST_SOLUTION)
    target_include_directories(${TARGET} PRIVATE .solution)
  endif()

  target_include_directories(${TARGET}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(prepend VAR PREFIX)
  set(LIST_VAR "")
  foreach(ELEM ${ARGN})
    list(APPEND LIST_VAR "${PREFIX}/${ELEM}")
  endforeach()
  set(${VAR} "${LIST_VAR}" PARENT_SCOPE)
endfunction()

add_custom_target(test-all)

function(add_patched_task NAME)
  set(MULTI_VALUE_ARGS PRIVATE_TESTS SOLUTION_SRCS)
  cmake_parse_arguments(SHAD_LIBRARY "" "" "${MULTI_VALUE_ARGS}" ${ARGN})

  if (TEST_SOLUTION)
    prepend(SHAD_LIBRARY_SOLUTION_SRCS ".solution" ${SHAD_LIBRARY_SOLUTION_SRCS})
  endif()

  add_executable(${NAME}
    ${SHAD_LIBRARY_UNPARSED_ARGUMENTS}
    ${SHAD_LIBRARY_SOLUTION_SRCS}
    ${SHAD_LIBRARY_PRIVATE_TESTS})

  if (TEST_SOLUTION)
    add_custom_target(
      run_${TARGET}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${TARGET}
      COMMAND ${CMAKE_BINARY_DIR}/${TARGET})
    add_dependencies(test-all run_${TARGET})
  endif()

  patch_include_directories(${NAME})
endfunction()

# function(add_patched_task TARGET)
#   add_executable(${TARGET} ${ARGN})
#   patch_include_directories(${TARGET})

#   if (TEST_SOLUTION)
#     add_custom_target(
#       run_${TARGET}
#       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#       DEPENDS ${TARGET}
#       COMMAND ${CMAKE_BINARY_DIR}/${TARGET})
#     add_dependencies(test-all run_${TARGET})
#   endif()
# endfunction()

function(add_catch TARGET)
  filter_existing(SRCS ${ARGN})

  if (SRCS)
    add_patched_task(${TARGET} ${SRCS})
    target_link_libraries(${TARGET} contrib_catch_main)
  endif()
endfunction()

function(add_gtest TARGET)
  set(SRCS ${ARGN})
  filter_existing(SRCS ${SRCS})

  if (SRCS)
    add_patched_task(${TARGET} ${SRCS})
    target_link_libraries(${TARGET} gtest_main)
  endif()
endfunction()

include(GoogleTest)
