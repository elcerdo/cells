#include <QApplication>
#include "worldwidget.h"

World::Player::Action action_player1(World::Player::Data &data) {
    return World::Player::SPAWN;
}

World::Player::Action action_player2(World::Player::Data &data) {
    return World::Player::EAT;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication app(argc,argv);
    World world;
    world.addPlayer("player1",qRgb(255,0,0),action_player1);
    world.addPlayer("player2",qRgb(0,0,255),action_player2);
    world.printReport();
    WorldWidget widget(world);

    widget.show();

    return app.exec();
}

