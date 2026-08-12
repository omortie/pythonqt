# -------------PythonQt Profile---------------
# Author: Mortie - July 14 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PythonQt-Qt5-Python)

#if(CMAKE_BUILD_TYPE STREQUAL Release)
	#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Zc:strictStrings")
#endif()

set(INCLUDE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/gui
		${CMAKE_CURRENT_SOURCE_DIR}/gui/scriptdebugger
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin
		${ScriptDebugger_INC})

set(SRCS
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQt.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtClassInfo.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtObjectPtr.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtProperty.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtStdIn.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtStdOut.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSignal.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSlot.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSlotDecorator.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtMisc.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtMethodInfo.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtConversion.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSignalReceiver.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtInstanceWrapper.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtQFileImporter.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtClassWrapper.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtBoolResult.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/gui/PythonQtScriptingConsole.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtStdDecorators.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtThreadSupport.cpp)

# -------------------------------------Script Debugger----------------
set(ScriptDebugger_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gui/scriptdebugger/codeeditor.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/gui/scriptdebugger/scriptdebugger.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/gui/scriptdebugger/scripthighlighter.cpp)

set(ScriptDebugger_RES ${CMAKE_CURRENT_SOURCE_DIR}/gui/scriptdebugger/scriptdebugger.qrc)

# -------------------------------------QtCore-------------------------
set(CoreSRCS
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp)

	
# --------------------------------------QtGuiBuiltin----------------------
set(GUISRCS
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp)

set(TARGET_NAME PythonQt-Qt5-Python)
	
if(PYTHONQT_STATIC)
	set(LIBRARY_TYPE STATIC)
else(PYTHONQT_STATIC)
	set(LIBRARY_TYPE SHARED)
endif(PYTHONQT_STATIC)

add_library(PythonQt-Qt5-Python ${LIBRARY_TYPE} ${SRCS}
		${ScriptDebugger_SRCS}
		${ScriptDebugger_RES}
		${CoreSRCS}
		${GUISRCS})
target_link_libraries(PythonQt-Qt5-Python PUBLIC Qt5::CorePrivate Qt5::Widgets)
target_link_directories(PythonQt-Qt5-Python PUBLIC ${PYTHON_PATH}/bin)
target_link_libraries(PythonQt-Qt5-Python PUBLIC python${PYTHON_VERSION_USEFUL})
target_include_directories(PythonQt-Qt5-Python PUBLIC ${INCLUDE_PATH})
if(${LIBRARY_TYPE} STREQUAL SHARED)
	set(OUTPUT_TYPE RUNTIME)
else(${LIBRARY_TYPE} STREQUAL SHARED)
	set(OUTPUT_TYPE ARCHIVE)
endif(${LIBRARY_TYPE} STREQUAL SHARED)

set_target_properties(PythonQt-Qt5-Python PROPERTIES ${OUTPUT_TYPE}_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(PythonQt-Qt5-Python PROPERTIES OUTPUT_NAME ${TARGET_NAME})
set_target_properties(PythonQt-Qt5-Python PROPERTIES DEFINE_SYMBOL PYTHONQT_EXPORTS)
