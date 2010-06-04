#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QStatusBar>
#include <QDockWidget>
#include <sstream>
#include "worldwidget.h"

World::Player::Action action_player1(World::Player::Data &data) {
    Q_UNUSED(data);
    return World::Player::Action::moveTo(Point(10,10));
}

World::Player::Action action_player2(World::Player::Data &data) {
    if (data.agent_arguments.empty()) {
        if (data.agent_energy>70) {
            Point target = Point::random(data.world_width,data.world_height);
            World::Player::Arguments args;
            args.push_back(target.x);
            args.push_back(target.y);
            return World::Player::Action::spawn(target,args);
        }
        return World::Player::Action::eat();
    }

    return World::Player::Action::moveTo(Point(data.agent_arguments[0],data.agent_arguments[1]));
}

MainWindow::MainWindow(int width, int height, QWidget *parent) : QMainWindow(parent), world(width,height), nworld_tick(0), speed(1)
{
    {
        world.addPlayer("player1",qRgb(255,0,0),action_player2);
        world.addPlayer("player2",qRgb(0,0,255),action_player1);
        world.addPlayer("player3",qRgb(0,255,255),action_player2);
    }

    QWidget *central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    QVBoxLayout *central_layout = new QVBoxLayout(central_widget);
    central_widget->setLayout(central_layout);

    {
        WorldWidget *world_widget = new WorldWidget(world,central_widget);
        connect(this,SIGNAL(worldTicked()),world_widget,SLOT(imageNeedUpdate()));
        central_layout->addWidget(world_widget);
    }

    {
        world_tick_timer = new QTimer(this);
        world_tick_timer->setInterval(30);
        world_tick_timer->setSingleShot(false);
        connect(world_tick_timer,SIGNAL(timeout()),SLOT(tickWorld()));

        QPushButton *world_tick = new QPushButton("tick world",central_widget);
        world_tick->setCheckable(true);
        connect(world_tick,SIGNAL(toggled(bool)),SLOT(startTickWorldTimer(bool)));
        central_layout->addWidget(world_tick);

        QSlider *world_speed = new QSlider(Qt::Horizontal,this);
        world_speed->setRange(1,10);
        connect(world_speed,SIGNAL(valueChanged(int)),SLOT(setSpeed(int)));
        central_layout->addWidget(world_speed);
    }

    {
        QDockWidget *report_dock = new QDockWidget("report",this);
        addDockWidget(Qt::RightDockWidgetArea,report_dock,Qt::Vertical);
        QWidget *report_widget = new QWidget(report_dock);
        report_dock->setWidget(report_widget);
        QVBoxLayout *report_layout = new QVBoxLayout(report_widget);
        report_widget->setLayout(report_layout);

        QPushButton *report_update = new QPushButton("update report",report_widget);
        connect(report_update,SIGNAL(clicked()),SLOT(updateReport()));
        report_layout->addWidget(report_update);

        report_label = new QLabel(report_widget);
        report_layout->addWidget(report_label);
        updateReport();

        report_layout->addStretch();
        report_widget->setMinimumWidth(300);
    }

    statusBar()->showMessage("welcome to cells!",1);
    resize(900,600);
}

void MainWindow::setSpeed(int new_speed)
{
    if (new_speed<1 or new_speed>10) return;
    speed = new_speed;
    world_tick_timer->setInterval(30./speed);
}

void MainWindow::startTickWorldTimer(bool start)
{
    if (start) {
        nworld_tick = 0;
        world_tick_timer->start();
        world_tick_time.restart();
    } else {
        world_tick_timer->stop();
        float elapsed = world_tick_time.elapsed()/1000.;
        statusBar()->showMessage(QString("simulated %1 ticks in %2s %3ticks/s").arg(nworld_tick).arg(elapsed,0,'f',1).arg(nworld_tick/elapsed,0,'f',1));
    }
}

void MainWindow::tickWorld()
{
    world.tick();
    nworld_tick++;
    if (nworld_tick%speed == 0) emit worldTicked();
}

void MainWindow::updateReport()
{
    std::stringstream ss;
    world.print(ss);
    report_label->setText(QString::fromStdString(ss.str()));
}
