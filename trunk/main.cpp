#include <QtWidgets/QApplication>
#include "MainWindow2.h"

#include <QSettings>



int main(int argc, char *argv[])
{

    try {
        QApplication a(argc, argv);

        // THis part works just with mac. Here we include the libraries and plugins:
        QString sDir = QCoreApplication::applicationDirPath();
        a.addLibraryPath(sDir+"/plugins");
        a.addLibraryPath(sDir+"/plugins/imageformats");
        a.addLibraryPath(sDir+"/plugins/armadillo");
        a.addLibraryPath(sDir+"../Frameworks");
        a.addLibraryPath(sDir+"../Frameworks/QtCore.framework");
        a.addLibraryPath(sDir+"../Frameworks/QtGui.framework");

        MainWindow2 w;
        w.showMaximized();

        return a.exec();
    }catch (...) {  // Catches all the exceptions, but it doesn't show a message.
        // If there is an error we delete settings.ini file to allow the app to start again.
        QSettings settings("options.ini", QSettings::IniFormat);
        settings.clear();
    }

    return 0;
}

