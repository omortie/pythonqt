# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyLauncher)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/extensions/PythonQt_QtAll)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
include_directories(${INCLUDE_PATH})

set(SRCS main.cpp)

add_executable(PyLauncher ${SRCS})
target_link_libraries(PyLauncher Qt5::Core Qt5::Widgets)
target_link_libraries(PyLauncher PythonQt-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
target_link_libraries(PyLauncher PythonQt_QtAll-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
set_target_properties(PyLauncher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyLauncher PROPERTIES OUTPUT_NAME PyLauncher${BUILD_POSTFIX})
