# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(CPPyWrapperExample)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SRCS CPPPyWrapperExample.cpp)
set(RSRCS CPPPyWrapperExample.qrc)

add_executable(CPPyWrapperExample ${SRCS})
target_link_libraries(CPPyWrapperExample Qt5::Core Qt5::Widgets)
target_link_libraries(CPPyWrapperExample PythonQt-Qt5-Python)
set_target_properties(CPPyWrapperExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(CPPyWrapperExample PROPERTIES OUTPUT_NAME CPPyWrapperExample${BUILD_POSTFIX})
