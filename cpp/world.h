#ifndef __WORLD_H__
#define __WORLD_H__

#include <set>
#include <list>
#include <map>
#include <string>
#include "common.h"

struct World
{
    struct Plant {
        Plant(const Point &position,float eff);
        Point position;
        float eff;
    };
    typedef std::set<Plant*> Plants;

    struct Player
    {
        typedef std::string Message;
        typedef std::list<Message> Messages;

        struct Data {
            Data(const Point &agent_position, float agent_energy, int world_width, int world_height);
            const Point &agent_position;
            const float agent_energy;
            const int world_width,world_height;
        };

        struct Action {
            enum Type {SPAWN, MOVE, EAT, ATTACK, LIFT, DROP};
            Type type;
            int datax;
            int datay;
        };
        typedef Action (*GetAction)(Data &data);

        struct Agent
        {
            Agent(const Point &position, const Player *player);
            Point position;
            const Player *player;
        };
        typedef std::set<Agent*> Agents;

        Player(const std::string &name, unsigned int color, GetAction action);
        ~Player();

        const std::string name;
        const unsigned int color;
        Messages *messages_inbox;
        Messages *messages_outbox;
        GetAction action;
        Agents agents;
    };
    typedef std::set<Player*> Players;
    typedef std::map<Player::Agent*,float> AgentEnergies;


    World(int width=30, int height=30, int nplants=12);
    ~World();

    void addPlayer(const std::string &name, unsigned int color, Player::GetAction action); 
    float getEnergy(Player::Agent *agent) const;
    void tick();
    void spawnAgent(Player *player, const Point &position);
    void printReport() const;

    const int width,height;
    Map<float> altitude;
    Map<bool> occupied;
    Plants plants;
    Players players;
    AgentEnergies energies;
};

#endif
