#include <QApplication>
#include "PythonQt.h"
#include "PythonQt_QtAll.h"
#include "scriptdebugger.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PythonQt::init();
    PythonQt_QtAll::init();
    PythonQtObjectPtr *mainModule = new PythonQtObjectPtr(PythonQt::self()->getMainModule());
    ScriptDebugger *scriptor = new ScriptDebugger(mainModule,NULL);
    scriptor->setLanguage(Python);
    scriptor->show();
    return a.exec();
}
