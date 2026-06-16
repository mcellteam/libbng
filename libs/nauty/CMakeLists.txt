cmake_minimum_required(VERSION 3.25...4.3)

project(nauty CXX)

# compiling only sources needed for function Trace
set(SOURCE_FILES
	nauty.c
	traces.c
	schreier.c
	nausparse.c
	naututil.c
	naurng.c
	gtools.c
	nautil.c
)

# nauty's C sources are compiled as C++ so the rest of the (C++) project links
# against a single, consistent ABI.
set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE CXX)

# Forced STATIC: nauty must never become a shared object (a shared nauty fails
# to link into mcell.so with "undefined reference to updatecan").
add_library(nauty STATIC ${SOURCE_FILES})
add_library(Nauty::nauty ALIAS nauty)

# Consumers that link nauty get its headers for free.
target_include_directories(nauty PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Warning suppressions are nauty's own business, not the whole project's.
target_compile_options(nauty PRIVATE
	$<$<NOT:$<PLATFORM_ID:Windows>>:-Wno-maybe-uninitialized;-Wno-unused-but-set-variable;-Wno-sign-compare;-Wno-write-strings>)