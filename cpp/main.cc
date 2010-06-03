#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication app(argc,argv);

    MainWindow widget(200,100);
    widget.show();

    return app.exec();
}

