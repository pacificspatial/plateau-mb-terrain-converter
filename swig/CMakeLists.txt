
# Find Python 3
set(CMAKE_SWIG_FLAGS)

find_package(SWIG 4.0 COMPONENTS python)
if(SWIG_FOUND)
  message("SWIG found: ${SWIG_EXECUTABLE}")
endif()
include(UseSWIG)

if(UNIX AND NOT APPLE)
  list(APPEND CMAKE_SWIG_FLAGS "-DSWIGWORDSIZE64")
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
if(NOT Python3_FOUND)
  message(SEND_ERROR "Python3 not found.")
  return()
endif()

list(APPEND CMAKE_SWIG_FLAGS "-py3" "-DPY3")

set(PYTHON_PROJECT pyplateauterrainrgb)
set(PYTHON_PROJECT_DIR ${PROJECT_BINARY_DIR}/python/${PYTHON_PROJECT})

set(UseSWIG_TARGET_NAME_PREFERENCE STANDARD)
set_property(SOURCE plateau-mb-terrain-converter.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE plateau-mb-terrain-converter.i PROPERTY SWIG_MODULE_NAME pyplateauterrainrgb)

swig_add_library(pyplateauterrainrgb
  TYPE MODULE
  LANGUAGE python
  OUTPUT_DIR ${PYTHON_PROJECT_DIR}
  SOURCES plateau-mb-terrain-converter.i)

add_library(${PROJECT_NAMESPACE}::pyplateauterrainrgb ALIAS pyplateauterrainrgb)
target_include_directories(pyplateauterrainrgb PRIVATE ../libplateauterrainrgb ${Python3_INCLUDE_DIRS})
set_property(TARGET pyplateauterrainrgb PROPERTY SWIG_USE_TARGET_INCLUDE_DIRECTORIES ON)

if(APPLE)
  set_target_properties(pyplateauterrainrgb PROPERTIES
    SUFFIX ".so"
    INSTALL_RPATH "@loader_path;@loader_path/../../${PYTHON_PROJECT}/.libs"
    )
  set_property(TARGET pyplateauterrainrgb APPEND PROPERTY
    LINK_FLAGS "-flat_namespace -undefined suppress"
    )
elseif(UNIX)
  set_target_properties(pyplateauterrainrgb PROPERTIES
    INSTALL_RPATH "$ORIGIN:$ORIGIN/../../${PYTHON_PROJECT}/.libs"
    )
endif()
target_link_libraries(pyplateauterrainrgb PRIVATE ${PROJECT_NAMESPACE}::libplateauterrainrgb)

# Variable PYTHON_LIBRARIES can contains keyword `optimized`
# which won't be interpreted inside a generator expression.
# i.e. we can't use: $<$<PLATFORM_ID:Windows>:${PYTHON_LIBRARIES}>
# see: https://cmake.org/cmake/help/git-stage/command/target_link_libraries.html#command:target_link_libraries
if(MSVC)
  target_link_libraries(pyplateauterrainrgb PRIVATE ${Python3_LIBRARIES})
endif()

install(TARGETS pyplateauterrainrgb COMPONENT python DESTINATION ${Python3_SITEARCH}/plateauterrainrgb)
install(FILES ${PYTHON_PROJECT_DIR}/pyplateauterrainrgb.py DESTINATION ${Python3_SITEARCH}/plateauterrainrgb)
