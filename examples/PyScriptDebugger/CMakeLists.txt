# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PyScriptDebugger)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/extensions/PythonQt_QtAll
        ${CMAKE_SOURCE_DIR}/src/gui/scriptdebugger/inc)
include_directories(${INCLUDE_PATH})

set(SRCS main.cpp)
set(RSRCS PyScriptDebugger.qrc)

qt5_wrap_cpp(MOC_HDRS ${HDRS})
qt5_add_resources(W_RSRCS ${RSRCS})

add_executable(PyScriptDebugger ${SRCS} ${MOC_HDRS} ${W_RSRCS})
target_link_libraries(PyScriptDebugger Qt5::Core Qt5::Widgets)
target_link_libraries(PyScriptDebugger PythonQt-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
target_link_libraries(PyScriptDebugger PythonQt_QtAll-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
set_target_properties(PyScriptDebugger PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PyScriptDebugger PROPERTIES OUTPUT_NAME PyScriptDebugger${BUILD_POSTFIX})
