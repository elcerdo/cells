#include <QApplication>
#include "mainwindow.h"
#include "minds.h"

int main(int argc, char *argv[])
{
    PythonMinds::init();

    srand(time(NULL));
    QApplication app(argc,argv);

    MainWindow widget;
    widget.show();

    int ret = app.exec();

    PythonMinds::destroy();

    return ret;
}

