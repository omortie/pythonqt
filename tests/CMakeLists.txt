# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PythonQtTest)
find_package(Qt5 REQUIRED COMPONENTS Core OpenGL Widgets Test)

set(INCLUDE_PATH ${CMAKE_SOURCE_DIR}/src)
include_directories(${INCLUDE_PATH})
link_directories(${PYTHON_PATH}/bin)

set(TestSRCS PythonQtTestMain.cpp PythonQtTests.cpp)
set(TestHDRS PythonQtTests.h)

qt5_wrap_cpp(MOC_TestHDRS ${TestHDRS})

add_executable(PythonQtTest ${TestSRCS} ${MOC_TestHDRS})
target_link_libraries(PythonQtTest Qt5::Core Qt5::OpenGL Qt5::Widgets Qt5::Test)
target_link_libraries(PythonQtTest PythonQt-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
target_link_libraries(PythonQtTest python${PYTHON_VERSION_USEFUL})
set_target_properties(PythonQtTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PythonQtTest PROPERTIES OUTPUT_NAME PythonQtTest${BUILD_POSTFIX})
