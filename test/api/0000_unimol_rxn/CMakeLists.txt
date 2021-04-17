set(SOURCE_FILES
  test.cpp
  ../shared/test_utils.cpp
)

add_executable(test
  ${SOURCE_FILES}
)

target_link_libraries(test
  bnglib
  nauty
  ${STDC_FS}
)
