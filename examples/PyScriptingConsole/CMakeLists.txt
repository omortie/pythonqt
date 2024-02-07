# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyScriptingConsole)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/extensions/PythonQt_QtAll)
include_directories(${INCLUDE_PATH})

set(SRCS main.cpp PyExampleObject.cpp)
set(HDRS PyExampleObject.h)
set(RSRCS PyScriptingConsole.qrc)

qt5_wrap_cpp(MOC_HDRS ${HDRS})
qt5_add_resources(W_RSRCS ${RSRCS})

add_executable(PyScriptingConsole ${SRCS} ${MOC_HDRS} ${W_RSRCS})
target_link_libraries(PyScriptingConsole Qt5::Core Qt5::Widgets)
target_link_libraries(PyScriptingConsole PythonQt-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
target_link_libraries(PyScriptingConsole PythonQt_QtAll-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
set_target_properties(PyScriptingConsole PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyScriptingConsole PROPERTIES OUTPUT_NAME PyScriptingConsole${BUILD_POSTFIX})
