#include <QApplication>
#include "mainwindow.h"
#include "mindspython.h"

int main(int argc, char *argv[])
{
    Python::init();

    srand(time(NULL));
    QApplication app(argc,argv);

    MainWindow widget;
    widget.show();

    int ret = app.exec();

    Python::destroy();

    return ret;
}

