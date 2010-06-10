#include "interface.h"

Plant::Plant(const Point &position, float eff) : position(position), eff(eff) {}

Agent::Agent(const std::string &player, const Point &position) : position(position), player(player) {}

AgentMe::AgentMe(const std::string &player, const Point &position, const Arguments &arguments, float energy, bool loaded, const Agents &agents_viewed, const Plants &plants_viewed, const Map<float> &energy_map) :
    Agent(player,position),
    arguments(arguments),
    energy_map(energy_map),
    energy(energy), loaded(loaded),
    agents_viewed(agents_viewed),
    plants_viewed(plants_viewed) {}

Action::Action(Type type, const Point &data, const Arguments &arguments) : type(type), data(data), arguments(arguments) {}

//----------------------------------------------------------------

Action Action::spawn(const Point &dest, const Arguments &arguments) { return Action(SPAWN,dest,arguments); }
Action Action::moveTo(const Point &dest) { return Action(MOVE,dest); }
Action Action::attack(const Point &dest) { return Action(ATTACK,dest); }
Action Action::eat() { return Action(EAT); }
Action Action::lift() { return Action(LIFT); }
Action Action::drop() { return Action(DROP); }
Action Action::doNothing() { return Action(DONOTHING); }

void Mind::initData(const Agents &agents, const Plants &plants, const MapFloat &energy_map) const {}
