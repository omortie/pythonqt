
TARGET   = PyScriptDebugger
TEMPLATE = app

DESTDIR           = ../../lib

mac:CONFIG-= app_bundle

include ( ../../build/common.prf )
include ( ../../build/PythonQt.prf )
include ( ../../build/PythonQt_QtAll.prf )

contains(QT_MAJOR_VERSION, 5) {
  QT += widgets
}

SOURCES +=                    \
  main.cpp

RESOURCES += PyScriptDebugger.qrc


