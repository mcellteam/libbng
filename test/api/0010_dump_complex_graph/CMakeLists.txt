project(0010_dump_complex_graph)

set(SOURCE_FILES
  test.cpp
  ../shared/test_utils.cpp
)

add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME}
  libbng
  nauty
  ${STDC_FS}
)
