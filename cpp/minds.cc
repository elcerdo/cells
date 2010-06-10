#include "minds.h"

#include <iostream>

using std::cout;
using std::endl;

Action MindTest1::act(const AgentMe &data) const {
    cout<<"act"<<endl;
    return Action::moveTo(Point(10,10));
}

Action MindTest2::act(const AgentMe &data) const {
    if (data.arguments.empty()) {
        int nennemies = 0;
        for (Agents::const_iterator i=data.agents_viewed.begin(); i!=data.agents_viewed.end(); i++) if (i->player!=data.player) nennemies++;
        cout<<data.player<<" views "<<data.agents_viewed.size()<<"/"<<nennemies<<" agents/enemies "<<data.plants_viewed.size()<<" plants"<<endl;

        if (data.energy>70) {
            Point target = Point::random(data.energy_map.width,data.energy_map.height);
            Arguments args;
            args.push_back(target.x);
            args.push_back(target.y);
            return Action::spawn(target,args);
        }
        return Action::eat();
    }

    return Action::moveTo(Point(data.arguments[0],data.arguments[1]));
}

Action MindTest3::act(const AgentMe &data) const {
    if (data.arguments.empty()) {
        int nennemies = 0;
        for (Agents::const_iterator i=data.agents_viewed.begin(); i!=data.agents_viewed.end(); i++) if (i->player!=data.player) nennemies++;

        if (data.energy>70) {
            Point target = Point::random(data.energy_map.width,data.energy_map.height);
            Arguments args;
            args.push_back(target.x);
            args.push_back(target.y);
            return Action::spawn(target,args);
        }
        return Action::eat();
    }

    return Action::moveTo(Point(data.arguments[0],data.arguments[1]));
}

