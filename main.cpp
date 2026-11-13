#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("简单文本编辑器");
    QApplication::setOrganizationName("QtTextEditor");

    MainWindow w;
    w.show();
    return app.exec();
}
