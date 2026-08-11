# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyGettingStarted)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SRCS main.cpp)
set(RSRCS PyGettingStarted.qrc)

add_executable(PyGettingStarted ${SRCS} ${RSRCS})
target_link_libraries(PyGettingStarted Qt5::Core Qt5::Widgets)
target_link_libraries(PyGettingStarted PythonQt-Qt5-Python)
set_target_properties(PyGettingStarted PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyGettingStarted PROPERTIES OUTPUT_NAME PyGettingStarted${BUILD_POSTFIX})
