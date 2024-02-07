# -------------PythonQt Profile---------------
# Author: Mortie - July 14 2018
# Project: add support for CMake build to PythonQt
# --------------------------------------------

project(PythonQt-Qt5-Python)

#if(CMAKE_BUILD_TYPE STREQUAL Release)
	#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Zc:strictStrings")
#endif()

#set(CMAKE_AUTOMOC ON)

set(INCLUDE_PATH
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/gui/scriptdebugger/inc
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin)

include_directories(${INCLUDE_PATH})

link_directories(${PYTHON_PATH}/bin)

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

set(HDRS
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQt.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtClassInfo.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtImporter.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtObjectPtr.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtProperty.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSignal.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSlot.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSlotDecorator.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtStdIn.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtStdOut.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtMisc.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtMethodInfo.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtImportFileInterface.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtConversion.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSignalReceiver.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtInstanceWrapper.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtClassWrapper.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtCppWrapperFactory.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtQFileImporter.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtQFileImporter.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtVariants.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtSystem.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtUtils.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtBoolResult.h
		${CMAKE_CURRENT_SOURCE_DIR}/gui/PythonQtScriptingConsole.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtStdDecorators.h
		${CMAKE_CURRENT_SOURCE_DIR}/PythonQtThreadSupport.h)

# -------------------------------------Script Debugger----------------
set(ScriptDebugger_SRCS gui/scriptdebugger/src/codeeditor.cpp
		gui/scriptdebugger/src/scriptdebugger.cpp
		gui/scriptdebugger/src/scripthighlighter.cpp)

set(ScriptDebugger_HDRS gui/scriptdebugger/inc/codeeditor.h
		gui/scriptdebugger/inc/scriptdebugger.h
		gui/scriptdebugger/inc/scripthighlighter.h)

set(ScriptDebugger_RES gui/scriptdebugger/res/scriptdebugger.qrc)

# -------------------------------------QtCore-------------------------
set(CoreSRCS
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin_init.cpp
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.cpp)
		   
set(CoreHDRS
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_core_builtin/com_trolltech_qt_core_builtin0.h)
	
# --------------------------------------QtGuiBuiltin----------------------
set(GUISRCS
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.cpp
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin_init.cpp)

set(GUIHDRS
		${PYTHONQT_GENERATED_PATH}/com_trolltech_qt_gui_builtin/com_trolltech_qt_gui_builtin0.h)
	
qt5_wrap_cpp(MOC_SRCS ${HDRS})
qt5_wrap_cpp(MOC_CoreSRCS ${CoreHDRS})
qt5_wrap_cpp(MOC_GUISRCS ${GUIHDRS})
qt5_wrap_cpp(MOC_ScriptDebugger_SRCS ${ScriptDebugger_HDRS})
qt5_add_resources(WScriptDebugger_RES ${ScriptDebugger_RES})

set(TARGET_NAME PythonQt-Qt5-Python${PYTHON_VERSION}${BUILD_POSTFIX}3)
	
if(PYTHONQT_STATIC)
	set(LIBRARY_TYPE STATIC)
else(PYTHONQT_STATIC)
	set(LIBRARY_TYPE SHARED)
endif(PYTHONQT_STATIC)

add_library(PythonQt-Qt5-Python ${LIBRARY_TYPE} ${SRCS} ${MOC_SRCS}
		${ScriptDebugger_SRCS} ${MOC_ScriptDebugger_SRCS} ${WScriptDebugger_RES}
		${CoreSRCS} ${MOC_CoreSRCS}
		${GUISRCS} ${MOC_GUISRCS} ${WScriptDebugger_RES})
target_link_libraries(PythonQt-Qt5-Python Qt5::CorePrivate Qt5::Widgets)
target_link_libraries(PythonQt-Qt5-Python python${PYTHON_VERSION_USEFUL})

if(${LIBRARY_TYPE} STREQUAL SHARED)
	set(OUTPUT_TYPE RUNTIME)
else(${LIBRARY_TYPE} STREQUAL SHARED)
	set(OUTPUT_TYPE ARCHIVE)
endif(${LIBRARY_TYPE} STREQUAL SHARED)

set_target_properties(PythonQt-Qt5-Python PROPERTIES ${OUTPUT_TYPE}_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set_target_properties(PythonQt-Qt5-Python PROPERTIES OUTPUT_NAME ${TARGET_NAME})
set_target_properties(PythonQt-Qt5-Python PROPERTIES DEFINE_SYMBOL PYTHONQT_EXPORTS)
