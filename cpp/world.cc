#include "world.h"

#include <cstdio>
#include <algorithm>
#include <vector>

World::Plant::Plant(const Point &position,float eff) : position(position), eff(eff) {}

World::Player::Agent::Agent(const Point &position, const Arguments &arguments, const World::Player *player) : position(position), loaded(false), arguments(arguments), player(player) {}

World::Player::Data::Data(const Point &agent_position, const Arguments &agent_arguments, float agent_energy, bool agent_loaded, int world_width, int world_height) :
    agent_position(agent_position), agent_arguments(agent_arguments), agent_energy(agent_energy), agent_loaded(agent_loaded),
    world_width(world_width), world_height(world_height) {}

World::Player::Player(const std::string &name, unsigned int color, GetAction action) : name(name), color(color), messages_inbox(new Messages), messages_outbox(new Messages), action(action) {}

World::Player::~Player()
{
    delete messages_inbox;
    delete messages_outbox;
}

World::World(int width, int height, int nplants) : width(width), height(height), altitude(width,height), energy(width,height), occupied(width,height)
{
    Map<float> altitude_tmp(width+2,height+2);
    for (int i=0; i<altitude_tmp.size; i++) { altitude_tmp.flat[i] = random_uniform(0,5); }

    for (int x=0; x<width; x++) for (int y=0; y<height; y++) {
        float mean = altitude_tmp.get(x+1,y)   + altitude_tmp.get(x,y)   + altitude_tmp.get(x+2,y) 
            + altitude_tmp.get(x+1,y+1) + altitude_tmp.get(x,y+1) + altitude_tmp.get(x+2,y+1) 
            + altitude_tmp.get(x+1,y+2) + altitude_tmp.get(x,y+2) + altitude_tmp.get(x+2,y+2);
        mean /= 9;
        altitude.get(x,y) = mean;
    }

    for (int i=0; i<nplants; i++) {
        Plant *plant = new Plant(Point::random(width,height),random_uniform(5,11));
        plants.insert(plant);
    }

    for (int i=0; i<energy.size; i++) { energy.flat[i] = random_uniform(0,10); }

    for (int i=0; i<occupied.size; i++) { occupied.flat[i] = false; }
}

World::~World()
{
    for (Plants::const_iterator i=plants.begin(); i!=plants.end(); i++) { delete *i; }
    for (Players::const_iterator i=players.begin(); i!=players.end(); i++) { delete *i; }
    for (AgentEnergies::const_iterator i=energies.begin(); i!=energies.end(); i++) { delete i->first; }
}

void World::addPlayer(const std::string &name, unsigned int color, Player::GetAction action)
{
    Point initial_position(-1,-1);
    for (Plants::const_iterator i=plants.begin(); i!=plants.end(); i++) if (occupied.get((*i)->position) == false) {
        initial_position = (*i)->position;
    }

    Player *player = new Player(name,color,action);
    players.insert(player);
    spawnAgent(initial_position,Player::Arguments(),player);
}

void World::spawnAgent(const Point &position, const Player::Arguments &arguments, Player *player)
{
    assert(players.find(player) != players.end());
    assert(occupied.get(position) == false);
    occupied.get(position) = true;
    Player::Agent *agent = new Player::Agent(position,arguments,player);
    player->agents.insert(agent);
    energies[agent] = 25.;
}

void World::printReport() const
{
    printf("size=%dx%d\n",width,height);
    printf("nplants=%d\n",plants.size());
    printf("nplayers=%d nagents=%d\n",players.size(),energies.size());
    for (Players::const_iterator i=players.begin(); i!=players.end(); i++) {
        const Player *player = *i;
        printf("\tname=%s nagents=%d\n",player->name.c_str(),player->agents.size());
    }
}

World::Player::Action::Action() : type(PASS) {}

World::Player::Action World::Player::Action::spawn(const Point &dest, const World::Player::Arguments &arguments)
{
    Action action;
    action.type = SPAWN;
    action.data = dest;
    action.arguments = arguments;
    return action;
}

World::Player::Action World::Player::Action::moveTo(const Point &dest)
{
    Action action;
    action.type = MOVE;
    action.data = dest;
    return action;
}

World::Player::Action World::Player::Action::eat()
{
    Action action;
    action.type = EAT;
    return action;
}

World::Player::Action World::Player::Action::pass()
{
    Action action;
    action.type = PASS;
    return action;
}

bool World::isPositionValid(const Point &point) const
{
    if (not occupied.isValid(point)) return false;
    return not occupied.get(point);
}

void World::tick() {
    typedef std::vector< std::pair<Player::Agent*,Player::Action> > Actions;

    // getting actions
    Actions actions;
    {
        unsigned int nagents = 0;
        for (Players::const_iterator iplayer=players.begin(); iplayer!=players.end(); iplayer++) {
            Player *player = *iplayer;
            nagents += player->agents.size();
            for (Player::Agents::const_iterator iagent=player->agents.begin(); iagent!=player->agents.end(); iagent++) {
                Player::Agent *agent = *iagent;
                Player::Data data(agent->position,agent->arguments,energies[agent],agent->loaded,width,height);
                actions.push_back(std::make_pair(agent,player->action(data)));
            }
        }
        std::random_shuffle(actions.begin(),actions.end());
        assert(actions.size() == nagents);
        assert(actions.size() == energies.size());
    }

    // parsing actions
    for (Actions::const_iterator iaction=actions.begin(); iaction!=actions.end(); iaction++) {
        Player::Agent *agent = iaction->first;
        const Player::Action &action = iaction->second;
        
        energies[agent]--;

        //FIXME attack not implemented
        if (action.type == Player::Action::MOVE) {
            Point position_new = agent->position.getNewPositionToDestination(action.data);
            if (isPositionValid(position_new) and abs(altitude.get(agent->position)-altitude.get(position_new))<4.) {
                occupied.get(agent->position) = false;
                agent->position = position_new;
                occupied.get(agent->position) = true;
            }
        } else if (action.type == Player::Action::SPAWN) {
            Point position_new = agent->position.getNewPositionToDestination(action.data);
            if (isPositionValid(position_new)) {
                energies[agent] -= 50;
                spawnAgent(position_new,action.arguments,const_cast<Player*>(agent->player)); //FIXME dirty hack
            }
        } else if (action.type == Player::Action::EAT) {
            float meal = energy.get(agent->position);
            energy.get(agent->position) = 0;
            energies[agent] += meal;
        } else if (action.type == Player::Action::LIFT) { //FIXME untested
            if (not agent->loaded and altitude.get(agent->position)>1) {
                agent->loaded = true;
                altitude.get(agent->position)--;
            }
        } else if (action.type == Player::Action::DROP) { //FIXME untested
            if (agent->loaded) {
                agent->loaded = false;
                altitude.get(agent->position)++;
            }
        } else if (action.type == Player::Action::PASS) {
        } else {
            printf("unknow action %d\n",action.type);
        }
    }

    // killing agents
    for (AgentEnergies::iterator ipair=energies.begin(); ipair!= energies.end();) {
        AgentEnergies::iterator ipair_copy = ipair++;
        if (ipair_copy->second<0) {
            occupied.get(ipair_copy->first->position) = false;
            delete ipair_copy->first;
            const_cast<Player*>(ipair_copy->first->player)->agents.erase(ipair_copy->first); //FIXME dirty hack
            energies.erase(ipair_copy);
        }
    }

    // updating plants
    for (World::Plants::const_iterator iplant=plants.begin(); iplant!=plants.end(); iplant++) {
        const Plant *plant = *iplant;
        for (int dx=-1; dx<2; dx++) for (int dy=-1; dy<2; dy++) {
            Point position = plant->position;
            position.x += dx;
            position.y += dy;
            if (energy.isValid(position)) energy.get(position) += plant->eff;
        }
    }

}
