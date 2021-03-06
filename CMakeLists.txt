#-----------------------------------------------------------------------------
# NOTE: The CMake files have been contributed to PythonQt
#
# If you are not a CMake expert, you should better use the provided qmake profiles.
# Author : Morteza Sherafati (Mortie)
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.8)
project(PythonQt)

# ----------------------------------Qt-----------------------------------
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
# --------------------------------Python-----------------------------------
if (UNIX)
set(PYTHON_VERSION 37)
set(PYTHON_VERSION_USEFUL 3.7m)
set(PYTHON_PATH /usr)
include_directories(${PYTHON_PATH}/include/python${PYTHON_VERSION_USEFUL})
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(PYTHONQT_STATIC False)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set(BUILD_POSTFIX _d)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

if(${Qt5_VERSION_MAJOR} STREQUAL 5)
message("${Qt5_VERSION_MINOR}")
	if(${Qt5_VERSION_MINOR} STREQUAL 10)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_56)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 20)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_50)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 30)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_53)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 40)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_54)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 50)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_55)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 60)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_56)
	endif(${Qt5_VERSION_MINOR} STREQUAL 10)
endif(${Qt5_VERSION_MAJOR} STREQUAL 5)
set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_56)
else(UNIX)  
if(WIN32)
set(PYTHON_VERSION 36)
set(PYTHON_VERSION_USEFUL 3.6m)
set(CMAKE_CXX_FLAGS "-D_hypot=hypot -Wa,-mbig-obj -fpermissive")
set(PYTHON_PATH C:/msys64/mingw64)
include_directories(${PYTHON_PATH}/include
${PYTHON_PATH}/include/python${PYTHON_VERSION_USEFUL})

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(PYTHONQT_STATIC False)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set(BUILD_POSTFIX _d)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)

if(${Qt5_VERSION_MAJOR} STREQUAL 5)
	if(${Qt5_VERSION_MINOR} STREQUAL 10)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_56)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 20)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_50)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 30)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_53)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 40)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_54)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 50)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_55)
	elseif(${Qt5_VERSION_MINOR} STREQUAL 60)
		set(PYTHONQT_GENERATED_PATH ${CMAKE_SOURCE_DIR}/generated_cpp_56)
	endif(${Qt5_VERSION_MINOR} STREQUAL 10)
endif(${Qt5_VERSION_MAJOR} STREQUAL 5)
endif(WIN32)
endif(UNIX)

link_directories(${CMAKE_SOURCE_DIR}/lib)

#add_subdirectory(generator)
add_subdirectory(src)
add_subdirectory(extensions)
add_subdirectory(tests)
add_subdirectory(examples)
