project(0100_generate_network_simple)

set(SOURCE_FILES
  test.cpp
  ../shared/test_utils.cpp
)

add_executable(${PROJECT_NAME}
  ${SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME}
  bnglib
  nauty
  ${STDC_FS}
)
