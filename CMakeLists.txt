cmake_minimum_required(VERSION 3.25...4.3)

if(MSVC)
  # compilation option required for MSVC
  add_definitions(/EHsc)
endif(MSVC)

project(top_level_libbng)

add_subdirectory(bng)
add_subdirectory(test/api)