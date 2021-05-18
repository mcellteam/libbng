project(0000_unimol_rxn)

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
