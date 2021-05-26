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


link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/bng/
  ${CMAKE_CURRENT_BINARY_DIR}/bng/nauty
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
  if ((NOT ${SUBDIR} STREQUAL shared) AND (NOT ${SUBDIR} STREQUAL __pycache__))
    add_subdirectory(${SUBDIR})
  endif()
endforeach()
