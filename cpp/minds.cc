#include "minds.h"

World::Player::Action mind_test1(World::Player::Data &data) {
    return World::Player::Action::moveTo(Point(10,10));
}

World::Player::Action mind_test2(World::Player::Data &data) {
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

