# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyCustomMetaTypeExample)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SRCS main.cpp CustomObject.cpp)
set(RSRCS PyCustomMetaTypeExample.qrc)

add_executable(PyCustomMetaTypeExample ${SRCS} ${RSRCS})
target_link_libraries(PyCustomMetaTypeExample Qt5::Core Qt5::Widgets)
target_link_libraries(PyCustomMetaTypeExample PythonQt-Qt5-Python)
set_target_properties(PyCustomMetaTypeExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyCustomMetaTypeExample PROPERTIES OUTPUT_NAME PyCustomMetaTypeExample${BUILD_POSTFIX})
