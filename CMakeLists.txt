#
# Master Opencog CMake file.
#
# Need to have cmake-2.8.8 in order to be able to check the compiler
# version.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)

PROJECT(generate)

# default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# add the 'lib' dir to cmake's module search path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")

# Cogutil
FIND_PACKAGE(CogUtil)
IF (COGUTIL_FOUND)
        MESSAGE(STATUS "CogUtil found.")
        ADD_DEFINITIONS(-DHAVE_COGUTIL)
        SET(HAVE_COGUTIL 1)
        INCLUDE_DIRECTORIES(${COGUTIL_INCLUDE_DIR})
ELSE (COGUTIL_FOUND)
        MESSAGE(FATAL_ERROR "CogUtil missing: it is needed for everything!")
ENDIF (COGUTIL_FOUND)

# add the 'cmake' directory from cogutil to search path
list(APPEND CMAKE_MODULE_PATH  ${COGUTIL_DATA_DIR}/cmake)

include(${COGUTIL_DATA_DIR}/cmake/OpenCogGccOptions.cmake)
include(${COGUTIL_DATA_DIR}/cmake/Summary.cmake)

# ===================================================================
# Check for existance of various required, optional packages.

# AtomSpace
FIND_PACKAGE(AtomSpace 5.0.3 REQUIRED)
IF (ATOMSPACE_FOUND)
	MESSAGE(STATUS "AtomSpace found.")
	ADD_DEFINITIONS(-DHAVE_ATOMSPACE)
	SET(HAVE_ATOMSPACE 1)
	INCLUDE_DIRECTORIES(${ATOMSPACE_INCLUDE_DIR})
ELSE (ATOMSPACE_FOUND)
	MESSAGE(FATAL_ERROR "AtomSpace missing: it is needed for everything!")
ENDIF (ATOMSPACE_FOUND)

# ----------------------------------------------------------
# Needed for unit tests.

FIND_PACKAGE(Cxxtest)
IF (CXXTEST_FOUND)
	MESSAGE(STATUS "CxxTest found.")
ELSE (CXXTEST_FOUND)
	MESSAGE(STATUS "CxxTest missing: needed for unit tests.")
ENDIF (CXXTEST_FOUND)

# ----------------------------------------------------------
# This is required for Guile
FIND_LIBRARY(GMP_LIBRARY gmp)
FIND_PATH(GMP_INCLUDE_DIR gmp.h)

# Gnu Guile scheme interpreter
# Version 2.2.2 is already specified by the atomspace.
FIND_PACKAGE(Guile 2.2.2.)
IF (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)
	ADD_DEFINITIONS(-DHAVE_GUILE)
	SET(HAVE_GUILE 1)
	INCLUDE_DIRECTORIES(${GUILE_INCLUDE_DIR})
ELSE (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)
	SET(GUILE_DIR_MESSAGE "Guile was not found; needed for guile benchmarks.")
	MESSAGE(STATUS "${GUILE_DIR_MESSAGE}")
ENDIF (GUILE_FOUND AND GMP_LIBRARY AND GMP_INCLUDE_DIR)

# ===================================================================
# Global includes

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}
	${COGUTIL_INCLUDE_DIR})

# -------------------------------------------------
# Install configuration

# Only list install files that have actually changed.
SET(CMAKE_INSTALL_MESSAGE "LAZY")

# ===================================================================
# Add subdirectories

ADD_SUBDIRECTORY(opencog)

IF (CXXTEST_FOUND)
	ADD_CUSTOM_TARGET(tests)
	ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)
	ADD_CUSTOM_TARGET(test
		DEPENDS tests
		WORKING_DIRECTORY tests
		COMMAND ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure $(ARGS)
		COMMENT "Running tests..."
	)
ENDIF (CXXTEST_FOUND)

# ===================================================================
# Show a summary of what we got

SUMMARY_ADD("Generate" "Graph Generation" HAVE_ATOMSPACE)
SUMMARY_ADD("Scheme bindings" "Scheme (guile) bindings" HAVE_GUILE)
SUMMARY_ADD("Unit tests" "Unit tests" CXXTEST_FOUND)
SUMMARY_SHOW()
