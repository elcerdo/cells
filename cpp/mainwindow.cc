#include "mainwindow.h"

#include <QVBoxLayout>
#include <QPushButton>
#include "worldwidget.h"

World::Player::Action action_player1(World::Player::Data &data) {
    Q_UNUSED(data);
    return World::Player::SPAWN;
}

World::Player::Action action_player2(World::Player::Data &data) {
    Q_UNUSED(data);
    return World::Player::EAT;
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
    qDebug("ticked");
}
