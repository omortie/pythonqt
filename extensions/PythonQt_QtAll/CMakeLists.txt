# -------------PythonQt Profile---------------
# Author: Mortie - August 1 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

find_package(Qt5 REQUIRED COMPONENTS Gui Svg Sql Network Xml XmlPatterns OpenGL 
Widgets PrintSupport Multimedia MultimediaWidgets
Quick Qml QuickWidgets UiTools)

project(PythonQt_QtAll-Qt5-Python)

#if(CMAKE_BUILD_TYPE STREQUAL Release)
	#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Zc:strictStrings")
#endif()

set(INCLUDE_PATH
${CMAKE_CURRENT_SOURCE_DIR}
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_multimedia
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_network
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_opengl
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_qml
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_quick
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_sql
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_svg
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_uitools
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_webkit
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_xml
${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_xmlpatterns
${CMAKE_SOURCE_DIR}/src)

set(PYTHONQTALL_SRCS PythonQt_QtAll.cpp)
set(PYTHONQTALL_HDRS PythonQt_QtAll.h)

file (GLOB PythonQt_Core_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core/*.cpp)

file (GLOB PythonQt_GUI_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_*.cpp)

file (GLOB PythonQt_Multimedia_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_multimedia/*.cpp)

file (GLOB PythonQt_Network_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_network/*.cpp)

file (GLOB PythonQt_OpenGL_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_opengl/*.cpp)

file (GLOB PythonQt_QML_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_qml/*.cpp)

file (GLOB PythonQt_Quick_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_quick/*.cpp)

file (GLOB PythonQt_SQL_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_sql/*.cpp)

file (GLOB PythonQt_SVG_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_svg/*.cpp)

file (GLOB PythonQt_UI_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_uitools/*.cpp)

file (GLOB PythonQt_xml_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_xml/*.cpp)

file (GLOB PythonQt_xmlPatterns_SRCS ${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_xmlpatterns/*.cpp)

set(TARGET_NAME PythonQt_QtAll-Qt5-Python)

if(PYTHONQT_STATIC)
	set(LIBRARY_TYPE STATIC)
else(PYTHONQT_STATIC)
	set(LIBRARY_TYPE SHARED)
endif(PYTHONQT_STATIC)

add_library(PythonQt_QtAll-Qt5-Python ${LIBRARY_TYPE} 
${PYTHONQTALL_SRCS}
${PythonQt_Core_SRCS}
${PythonQt_GUI_SRCS}
${PythonQt_Multimedia_SRCS}
${PythonQt_Network_SRCS}
${PythonQt_OpenGL_SRCS}
${PythonQt_QML_SRCS}
${PythonQt_Quick_SRCS}
${PythonQt_SQL_SRCS}
${PythonQt_SVG_SRCS}
${PythonQt_UI_SRCS}
${PythonQt_xml_SRCS}
${PythonQt_xmlPatterns_SRCS})

target_link_libraries(PythonQt_QtAll-Qt5-Python PUBLIC Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Svg Qt5::Sql
Qt5::Network Qt5::Xml Qt5::XmlPatterns Qt5::OpenGL Qt5::PrintSupport Qt5::Multimedia Qt5::MultimediaWidgets
Qt5::Quick Qt5::Qml Qt5::QuickWidgets Qt5::UiTools)
target_link_directories(PythonQt_QtAll-Qt5-Python PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_link_libraries(PythonQt_QtAll-Qt5-Python PUBLIC PythonQt-Qt5-Python)
target_include_directories(PythonQt_QtAll-Qt5-Python PUBLIC ${INCLUDE_PATH})

if(${LIBRARY_TYPE} STREQUAL SHARED)
	set(OUTPUT_TYPE RUNTIME)
else(${LIBRARY_TYPE} STREQUAL SHARED)
	set(OUTPUT_TYPE ARCHIVE)
endif(${LIBRARY_TYPE} STREQUAL SHARED)

set_target_properties(PythonQt_QtAll-Qt5-Python PROPERTIES ${OUTPUT_TYPE}_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(PythonQt_QtAll-Qt5-Python PROPERTIES OUTPUT_NAME ${TARGET_NAME})
set_target_properties(PythonQt_QtAll-Qt5-Python PROPERTIES DEFINE_SYMBOL PYTHONQT_QTALL_EXPORTS)
