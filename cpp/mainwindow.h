#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QTimer>
#include "world.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(int width, int height, QWidget *parent=NULL);
public slots:
    void tickWorld();
    void startTickWorldTimer(bool);
signals:
    void worldTicked();
protected:
    QTimer *world_tick_timer;
    World world;
};


#endif
