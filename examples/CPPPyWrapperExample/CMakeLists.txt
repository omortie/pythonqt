# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(CPPyWrapperExample)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src)
include_directories(${INCLUDE_PATH})

set(SRCS CPPPyWrapperExample.cpp)
set(RSRCS CPPPyWrapperExample.qrc)

qt5_add_resources(W_RSRCS ${RSRCS})

add_executable(CPPyWrapperExample ${SRCS} ${W_RSRCS})
target_link_libraries(CPPyWrapperExample Qt5::Core Qt5::Widgets)
target_link_libraries(CPPyWrapperExample PythonQt-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
set_target_properties(CPPyWrapperExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(CPPyWrapperExample PROPERTIES OUTPUT_NAME CPPyWrapperExample${BUILD_POSTFIX})
