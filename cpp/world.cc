#include "world.h"

#include <cstdio>

World::Plant::Plant(const Point &position,float eff) : position(position), eff(eff) {}

World::Player::Agent::Agent(const Point &position, const World::Player *player) : position(position), player(player) {}

World::Player::Data::Data(const Point &agent_position, float agent_energy, int world_width, int world_height) : agent_position(agent_position), agent_energy(agent_energy),
                                                                                                                world_width(world_width), world_height(world_height) {}

World::Player::Player(const std::string &name, unsigned int color, GetAction action) : name(name), color(color), messages_inbox(new Messages), messages_outbox(new Messages), action(action) {}

World::Player::~Player()
{
    delete messages_inbox;
    delete messages_outbox;
}

World::World(int width, int height, int nplants) : width(width), height(height), altitude(width,height), occupied(width,height)
{
    Map<float> altitude_tmp(width+2,height+2);
    for (int i=0; i<altitude_tmp.size; i++) { altitude_tmp.flat[i] = random_uniform(); }

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
    spawnAgent(player,initial_position);
}

void World::spawnAgent(Player *player, const Point &position)
{
    assert(players.find(player) != players.end());
    assert(occupied.get(position) == false);
    occupied.get(position) = true;
    Player::Agent *agent = new Player::Agent(position,player);
    player->agents.insert(agent);
    energies[agent] = 50.;
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

