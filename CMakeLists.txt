#-----------------------------------------------------------------------------
# NOTE: The CMake files have been contributed to PythonQt
#
# If you are not a CMake expert, you should better use the provided qmake profiles.
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.15)
project(PythonQt)

# ----------------------------------Qt-----------------------------------
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
# --------------------------------Python-----------------------------------
set(PYTHON_VERSION 312)
set(PYTHON_VERSION_USEFUL 3.12)
set(PYTHON_PATH /usr)
include_directories(${PYTHON_PATH}/include/python${PYTHON_VERSION_USEFUL})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(PYTHONQT_STATIC False)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set(BUILD_POSTFIX _d)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

if(${Qt5_VERSION_MAJOR} STREQUAL 5)
	set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_515)
endif(${Qt5_VERSION_MAJOR} STREQUAL 5)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#add_subdirectory(generator)
add_subdirectory(src)
add_subdirectory(extensions)
add_subdirectory(tests)
add_subdirectory(examples)
