cmake_minimum_required(VERSION 2.8.4)

project(api)

# shared definitions for all tests
include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/../../"
)

include_directories(
  SYSTEM
  "${CMAKE_CURRENT_SOURCE_DIR}/../../libs/sparsehash/src/"
)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  set(BNG_DIR debug)
else ()
  set(BNG_DIR release)
endif ()

link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../build/${BNG_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../build/${BNG_DIR}/nauty
)

if (NOT WIN32 AND NOT APPLE)
  set(STDC_FS stdc++fs)
endif()


# macro for all subdirectories
# from https://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# add all subdirectories
foreach(SUBDIR ${SUBDIRS})
  if (NOT ${SUBDIR} STREQUAL shared)
    add_subdirectory(${SUBDIR})
  endif()
endforeach()
