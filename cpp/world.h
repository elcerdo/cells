#ifndef __WORLD_H__
#define __WORLD_H__

#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <iostream>
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
            static Action lift();
            static Action drop(); 
            enum Type {PASS, SPAWN, MOVE, EAT, ATTACK, LIFT, DROP};
            Type type;
            Point data;
            Arguments arguments;
        private:
            Action();
        };
        typedef Action Mind(Data &data);

        struct Agent
        {
            Agent(const Point &position, const Arguments &arguments, const Player *player);
            Point position;
            bool loaded;
            const Arguments arguments;
            const Player *player;
        };
        typedef std::set<Agent*> Agents;

        Player(const std::string &name, unsigned int color, Mind *action);
        ~Player();

        void print(std::ostream &os) const;

        const std::string name;
        const unsigned int color;
        int deathtick;
        Messages *messages_inbox;
        Messages *messages_outbox;
        Mind *action;
        Agents agents;
    };
    typedef std::set<Player*> Players;
    typedef std::map<Player::Agent*,float> AgentEnergies;


    World(int width=30, int height=30, int nplants=12);
    ~World();

    void tick();
    void print(std::ostream &os) const;
    void addPlayer(const std::string &name, unsigned int color, Player::Mind *action); 
    void spawnAgent(const Point &position, const Player::Arguments &arguments, Player *player);
    bool isGameFinished() const;

    bool isPositionValid(const Point &point) const;
    
    typedef void Callback(const Player &player, void *callbackData);
    Callback *deadPlayer;
    void *callbackData;

    const int width,height;
    int nticks;
    Map<float> altitude;
    Map<float> energy;
    Map<bool> occupied;
    Plants plants;
    Players players;
    AgentEnergies energies;
};

#endif
