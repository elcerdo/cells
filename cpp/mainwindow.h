#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include "world.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(int width, int height, QWidget *parent=NULL);
public slots:
    void tickWorld();
    void startTickWorldTimer(bool);
    void setSpeed(int speed);
signals:
    void worldTicked();
protected:
    QTimer *world_tick_timer;
    QTime world_tick_time;
    World world;
    int nworld_tick;
    int speed;
};


#endif
