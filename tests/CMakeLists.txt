# -------------PythonQt Profile---------------
# Author: Mortie - August 19 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PythonQtTest)
find_package(Qt5 REQUIRED COMPONENTS Core OpenGL Widgets Test)

link_directories(${PYTHON_PATH}/bin)

set(TestSRCS PythonQtTestMain.cpp PythonQtTests.cpp)

qt5_wrap_cpp(MOC_TestSRCS ${TestSRCS})

add_executable(PythonQtTest ${TestSRCS} ${MOC_TestSRCS})
target_link_libraries(PythonQtTest Qt5::Core Qt5::OpenGL Qt5::Widgets Qt5::Test)
target_link_libraries(PythonQtTest PythonQt-Qt5-Python)
#target_link_libraries(PythonQtTest python${PYTHON_VERSION_USEFUL})
set_target_properties(PythonQtTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PythonQtTest PROPERTIES OUTPUT_NAME PythonQtTest${BUILD_POSTFIX})
