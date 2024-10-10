
# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file COPYING-CMAKE-SCRIPTS or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindSQLite3
-----------
Find the SQLite libraries, v3
IMPORTED targets
^^^^^^^^^^^^^^^^
This module defines the following :prop_tgt:`IMPORTED` target:
``SQLite::SQLite3``
Result variables
^^^^^^^^^^^^^^^^
This module will set the following variables if found:
``SQLite3_INCLUDE_DIRS``
  where to find sqlite3.h, etc.
``SQLite3_LIBRARIES``
  the libraries to link against to use SQLite3.
``SQLite3_VERSION``
  version of the SQLite3 library found
``SQLite3_FOUND``
  TRUE if found

  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
  Copyright (c) 2016 NextGIS <info@nextgis.com>
  Copyright (c) 2018,2021 Hiroshi Miura
  Copyright (c) 2019 Chuck Atkins
#]=======================================================================]

include (CMakePushCheckState)
include(CheckCSourceCompiles)
# Accept upper case variant for SQLite3_INCLUDE_DIR
if(SQLITE3_INCLUDE_DIR)
  if(SQLite3_INCLUDE_DIR AND NOT "${SQLite3_INCLUDE_DIR}" STREQUAL "${SQLITE3_INCLUDE_DIR}")
    message(FATAL_ERROR "Both SQLite3_INCLUDE_DIR and SQLITE3_INCLUDE_DIR are defined, but not at same value")
  endif()
  set(SQLite3_INCLUDE_DIR "${SQLITE3_INCLUDE_DIR}" CACHE STRING "Path to a file" FORCE)
  unset(SQLITE3_INCLUDE_DIR CACHE)
endif()

# Accept upper case variant for SQLite3_LIBRARY
if(SQLITE3_LIBRARY)
  if(SQLite3_LIBRARY AND NOT "${SQLite3_LIBRARY}" STREQUAL "${SQLITE3_LIBRARY}")
    message(FATAL_ERROR "Both SQLite3_LIBRARY and SQLITE3_LIBRARY are defined, but not at same value")
  endif()
  set(SQLite3_LIBRARY "${SQLITE3_LIBRARY}" CACHE FILEPATH "Path to a library" FORCE)
  unset(SQLITE3_LIBRARY CACHE)
endif()

if(SQLite3_INCLUDE_DIR AND SQLite3_LIBRARY)
  set(SQLite3_FIND_QUIETLY TRUE)
else()
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
      pkg_check_modules(PC_SQLITE3 QUIET sqlite3)
  endif()

  find_path(SQLite3_INCLUDE_DIR
            NAMES  sqlite3.h
            HINTS ${PC_SQLITE3_INCLUDE_DIRS})
  find_library(SQLite3_LIBRARY
               NAMES sqlite3 sqlite3_i
               HINTS ${PC_SQLITE3_LIBRARY_DIRS})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3
                                  FOUND_VAR SQLite3_FOUND
                                  REQUIRED_VARS SQLite3_LIBRARY SQLite3_INCLUDE_DIR
                                  VERSION_VAR SQLite3_VERSION)

if(SQLite3_FOUND)
  set(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
  set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
  if(NOT TARGET SQLite::SQLite3)
    add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
    set_target_properties(SQLite::SQLite3 PROPERTIES
                          INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIRS}"
                          IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                          IMPORTED_LOCATION "${SQLite3_LIBRARY}")
    if(SQLite3_HAS_COLUMN_METADATA)
        set_property(TARGET SQLite::SQLite3 APPEND PROPERTY
                     INTERFACE_COMPILE_DEFINITIONS "SQLite3_HAS_COLUMN_METADATA")
    endif()
    if(SQLite3_HAS_RTREE)
        set_property(TARGET SQLite::SQLite3 APPEND PROPERTY
                     INTERFACE_COMPILE_DEFINITIONS "SQLite3_HAS_RTREE")
    endif()
  endif()
else()
  message(SQLite3_INCLUDE_DIR=${SQLite3_INCLUDE_DIRS})
  message(SQLite3_LIBRARIE=${SQLite3_LIBRARIES})
  message(FATAL_ERROR "Could not find SQLite3")
endif()
