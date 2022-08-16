#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator translator;

    if (translator.load(QLocale(),
                        "blocks",
                        "_",
                        app.applicationDirPath() + "/translations/blocks"))
        app.installTranslator(&translator);

    QString path = (argc == 2) ? argv[1] : "";
    MainWindow w(path);

    w.show();
    return app.exec();
}
