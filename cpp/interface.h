#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <vector>
#include <set>
#include <string>
#include "common.h"

typedef std::vector<int> Arguments;

struct Plant {
    Plant(const Point &position,float eff);
    const Point &position;
    const float eff;
};
typedef std::set<Plant, PositionLess<Plant> > Plants;

struct Agent {
    Agent(const std::string &player_name, const Point &position);
    const Point &position;
    const std::string &player;
};
typedef std::set<Agent, PositionLess<Agent> > Agents;

struct AgentMe : public Agent {
    AgentMe(const std::string &player, const Point &position, const Arguments &arguments, float energy, bool loaded, const Agents &agents_viewed, const Plants &plants_viewed, const Map<float> &energy_map);
    const Arguments &arguments;
    const Map<float> &energy_map;
    const float energy;
    const bool loaded;
    const Agents &agents_viewed;
    const Plants &plants_viewed;
};

struct Action {
    static Action doNothing();
    static Action spawn(const Point &dest, const Arguments &arguments);
    static Action moveTo(const Point &dest);
    static Action eat();
    static Action attack(const Point &dest);
    static Action lift();
    static Action drop(); 
    enum Type {DONOTHING=0, SPAWN=1, MOVE=2, EAT=3, ATTACK=4, LIFT=5, DROP=6};
    Type type;
    Point data;
    Arguments arguments;
private:
    Action(Type type, const Point &data=Point(), const Arguments &arguments=Arguments());
};

typedef Action Mind(const AgentMe &me);


#endif

