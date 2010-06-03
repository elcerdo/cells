#ifndef __WORLD_H__
#define __WORLD_H__

#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include "common.h"

struct World
{
    struct Plant {
        Plant(const Point &position,float eff);
        const Point position;
        const float eff;
    };
    typedef std::set<Plant*> Plants;

    struct Player
    {
        typedef std::string Message;
        typedef std::list<Message> Messages;
        typedef std::vector<int> Arguments;

        struct Data {
            Data(const Point &agent_position, const Arguments &arguments, float agent_energy, bool agent_loaded, int world_width, int world_height);
            const Point &agent_position;
            const Arguments &agent_arguments;
            const float agent_energy;
            const float agent_loaded;
            const int world_width,world_height;
        };

        struct Action {
            static Action moveTo(const Point &dest);
            static Action pass();
            static Action spawn(const Point &dest, const Arguments &arguments);
            static Action eat();
            enum Type {PASS, SPAWN, MOVE, EAT, ATTACK, LIFT, DROP};
            Type type;
            Point data;
            Arguments arguments;
        private:
            Action();
        };
        typedef Action (*GetAction)(Data &data);

        struct Agent
        {
            Agent(const Point &position, const Arguments &arguments, const Player *player);
            Point position;
            bool loaded;
            const Arguments arguments;
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
    void tick();
    void spawnAgent(const Point &position, const Player::Arguments &arguments, Player *player);
    void printReport() const;
    bool isPositionValid(const Point &point) const;

    const int width,height;
    Map<float> altitude;
    Map<float> energy;
    Map<bool> occupied;
    Plants plants;
    Players players;
    AgentEnergies energies;
};

#endif
