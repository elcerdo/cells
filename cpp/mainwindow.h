#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include "engine.h"
#include "worldwidget.h"
#include "mindsmodel.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(QWidget *parent=NULL);
    ~MainWindow();
    static void deadPlayerCallback(const Player &player, void *data);
public slots:
    void loadMind();
    void setWorld(World *world);
    void tickWorld();
    void startTickWorldTimer(bool);
    void setSpeed(int speed);
    void updateReport();
signals:
    void worldTicked();
    void playerDied();
protected:
    QTimer *world_tick_timer;
    QLabel *report_label;
    WorldWidget *world_widget;
    MindsModel *minds_model;
    World *world;
    QTime world_tick_time;
    int nworld_tick;
    int speed;
};


#endif
