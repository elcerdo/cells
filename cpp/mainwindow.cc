#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "worldwidget.h"

World::Player::Action action_player1(World::Player::Data &data) {
    Q_UNUSED(data);
    qDebug("player 1 agent");
    World::Player::Action action;
    return action;
}

World::Player::Action action_player2(World::Player::Data &data) {
    Q_UNUSED(data);
    qDebug("player 2 agent");
    World::Player::Action action;
    return action;
}

MainWindow::MainWindow(int width, int height, QWidget *parent) : QMainWindow(parent), world(width,height)
{
    {
        world.addPlayer("player1",qRgb(255,0,0),action_player1);
        world.addPlayer("player2",qRgb(0,0,255),action_player2);
        world.printReport();
    }

    QWidget *central_widget = new QWidget(this);
    QVBoxLayout *central_layout = new QVBoxLayout(central_widget);
    central_widget->setLayout(central_layout);

    {
        WorldWidget *world_widget = new WorldWidget(world,central_widget);
        central_layout->addWidget(world_widget);
    }

    {
        QPushButton *world_tick = new QPushButton("tick world",central_widget);
        central_layout->addWidget(world_tick);
        connect(world_tick,SIGNAL(clicked()),SLOT(tickWorld()));
    }

    setCentralWidget(central_widget);
    resize(800,700);
}

void MainWindow::tickWorld()
{
    world.tick();
    qDebug("ticked");
}
