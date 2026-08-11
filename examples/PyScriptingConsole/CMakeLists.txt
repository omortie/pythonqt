# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyScriptingConsole)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(SRCS main.cpp PyExampleObject.cpp)
set(RSRCS PyScriptingConsole.qrc)

add_executable(PyScriptingConsole ${SRCS} ${RSRCS})
target_link_libraries(PyScriptingConsole Qt5::Core Qt5::Widgets)
target_link_libraries(PyScriptingConsole PythonQt-Qt5-Python)
target_link_libraries(PyScriptingConsole PythonQt_QtAll-Qt5-Python)
set_target_properties(PyScriptingConsole PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyScriptingConsole PROPERTIES OUTPUT_NAME PyScriptingConsole${BUILD_POSTFIX})
