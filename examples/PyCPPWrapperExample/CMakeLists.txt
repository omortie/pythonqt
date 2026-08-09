# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyCPPWrapperExample)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SRCS main.cpp CustomObjects.cpp)
set(RSRCS PyCPPWrapperExample.qrc)

add_executable(PyCPPWrapperExample ${SRCS} ${RSRCS})
target_link_libraries(PyCPPWrapperExample Qt5::Core Qt5::Widgets)
target_link_libraries(PyCPPWrapperExample PythonQt-Qt5-Python)
set_target_properties(PyCPPWrapperExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyCPPWrapperExample PROPERTIES OUTPUT_NAME PyCPPWrapperExample${BUILD_POSTFIX})
