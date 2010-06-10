#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "common.h"
#include "interface.h"

struct Player {
    struct AgentInternal {
        AgentInternal(Player *player, const Point &position, const Arguments &arguments);
        Player *player;    
        Point position;
        bool loaded;
        float energy;
        const Arguments arguments;
    };
    typedef std::set<AgentInternal*> AgentInternals;
    typedef Map<AgentInternal*> AgentInternalsMap;

    Player(const std::string &name, unsigned int color, Mind *mind);
    ~Player();

    void print(std::ostream &os) const;

    const std::string name;
    const unsigned int color;
    int deathtick;
    Mind *mind;
    AgentInternals agents;
};
typedef std::set<Player*> Players;

struct PlantInternal {
    PlantInternal(const Point &position, float eff);
    Point position;
    float eff;
};
typedef std::set<PlantInternal*> PlantInternals;
typedef Map<PlantInternal*> PlantInternalMap;

struct World {
    World(int width, int height, int nplants);
    ~World();

    void tick();
    void print(std::ostream &os) const;
    void addPlayer(const std::string &name, unsigned int color, Mind *mind); 
    void spawnAgent(Player *player, const Point &position, const Arguments &arguments=Arguments());
    bool isGameFinished() const;

    bool isAttackable(const Point &point) const;
    Player::AgentInternal *getAgent(const Point &point) const;
    PlantInternal *getPlant(const Point &point) const;
    void checkPosition(const Point &position, Agents &agents_viewed, Plants &plants_viewed) const;
    
    typedef void Callback(const Player &player, void *callbackData);
    Callback *deadPlayer;
    void *callbackData;

    const int width,height;
    int nticks;

    MapFloat altitude_map;
    MapFloat energy_map;

    Player::AgentInternals agents;
    Player::AgentInternalsMap agents_map;

    PlantInternals plants;
    PlantInternalMap plants_map;

    Players players;
};

#endif
