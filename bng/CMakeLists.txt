cmake_minimum_required(VERSION 3.25...4.3)

project(libbng CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libs/nauty ${CMAKE_CURRENT_BINARY_DIR}/nauty)

set(SOURCE_FILES
  shared_defines.cpp
	ast.cpp
	base_flag.cpp
	bng_data.cpp
	bng_engine.cpp
	bng_config.cpp
	cplx.cpp
	elem_mol.cpp
	elem_mol_type.cpp
	parser_utils.cpp
	parser.cpp
	rxn_class.cpp
	rxn_container.cpp
	rxn_rule.cpp
	rxn_compartment_utils.cpp
	species.cpp
	species_container.cpp
	semantic_analyzer.cpp
	graph.cpp
	filesystem_utils.cpp
)


# check for bison/flex and set up code gen
find_package(BISON)
find_package(FLEX)
BISON_TARGET(bngl_parser ${CMAKE_CURRENT_SOURCE_DIR}/bngl_parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/deps/bngl_parser.cpp)

# the cmake flex plugin requires the flex file to be in the 'deps' directory
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bngl_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/deps/bngl_scanner.l COPYONLY)

FLEX_TARGET(bngl_scanner ${CMAKE_CURRENT_SOURCE_DIR}/bngl_scanner.l
  ${CMAKE_CURRENT_BINARY_DIR}/deps/bngl_scanner.cpp
  COMPILE_FLAGS "--header-file=${CMAKE_CURRENT_BINARY_DIR}/deps/bngl_scanner.hpp"
)


# TODO: add bngl_scanner.l|y dependencies
  
ADD_FLEX_BISON_DEPENDENCY(bngl_scanner bngl_parser bngl_l nauty)

set(ALL_SOURCES
    ${SOURCE_FILES}
    ${BISON_bngl_parser_OUTPUTS}
    ${FLEX_bngl_scanner_OUTPUTS}    
)


if (NOT WIN32 AND NOT APPLE)
    set(STDC_FS stdc++fs)
endif()

add_library(${PROJECT_NAME} ${ALL_SOURCES})
add_library(BNG::bng ALIAS ${PROJECT_NAME})

# Usage requirements: anything that links libbng inherits these include dirs,
# the C++17 standard and the YY_NO_UNISTD_H definition.
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}        # libbng/bng
		${CMAKE_CURRENT_SOURCE_DIR}/..     # libbng/  (consumers use "bng/...")
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/deps   # generated flex/bison headers
)
# sparsehash and the bundled libs are third-party; mark them SYSTEM so their
# warnings do not pollute consumers.
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/../libs
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/sparsehash/src
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE YY_NO_UNISTD_H)
target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<NOT:$<PLATFORM_ID:Windows>>:-Wall;-Wno-unused-function>)

target_link_libraries(${PROJECT_NAME}
	PRIVATE Nauty::nauty ${STDC_FS}
)

# Development-only tool: it recompiles all libbng sources alongside test.cpp
# (it does NOT link libbng, which would duplicate every symbol), so it needs
# the same include dirs / definitions spelled out explicitly.
add_executable(parser_tester_${PROJECT_NAME}
  ${ALL_SOURCES}
	test.cpp # only for development
)
target_include_directories(parser_tester_${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/..
		${CMAKE_CURRENT_BINARY_DIR}/deps)
target_include_directories(parser_tester_${PROJECT_NAME} SYSTEM PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../libs
	${CMAKE_CURRENT_SOURCE_DIR}/../libs/sparsehash/src)
target_compile_features(parser_tester_${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(parser_tester_${PROJECT_NAME} PRIVATE YY_NO_UNISTD_H)

target_link_libraries(parser_tester_${PROJECT_NAME}
	PUBLIC Nauty::nauty ${STDC_FS}
)
