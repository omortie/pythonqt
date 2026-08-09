# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyGuiExample)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SRCS main.cpp)
set(RSRCS PyGuiExample.qrc)

qt5_add_resources(W_RSRCS ${RSRCS})

add_executable(PyGuiExample ${SRCS} ${MOC_HDRS} ${W_RSRCS})
target_link_libraries(PyGuiExample Qt5::Core Qt5::Widgets)
target_link_libraries(PyGuiExample PythonQt-Qt5-Python)
target_link_libraries(PyGuiExample PythonQt_QtAll-Qt5-Python)
set_target_properties(PyGuiExample PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyGuiExample PROPERTIES OUTPUT_NAME PyGuiExample${BUILD_POSTFIX})
