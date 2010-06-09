#include "engine.h"

#include <algorithm>
#include <iostream>

Player::AgentInternal::AgentInternal(Player *player, const Point &position, const Arguments &arguments) : player(player), position(position), loaded(false), energy(25), arguments(arguments) {}

PlantInternal::PlantInternal(const Point &position, float eff) : position(position), eff(eff) {}

Player::Player(const std::string &name, unsigned int color, Mind *mind) : name(name), color(color), deathtick(-1), mind(mind) {}

World::World(int width, int height, int nplants) : deadPlayer(NULL), callbackData(NULL), width(width), height(height), nticks(0), altitude_map(width,height), energy_map(width,height), agents_map(width,height), plants_map(width,height) {
    // init altitude map
    MapFloat altitude_tmp(width+2,height+2);
    for (int i=0; i<altitude_tmp.size; i++) { altitude_tmp.flat[i] = random_uniform(0,5); }

    for (int x=0; x<width; x++) for (int y=0; y<height; y++) {
        float mean = altitude_tmp.get(x+1,y)   + altitude_tmp.get(x,y)   + altitude_tmp.get(x+2,y) 
            + altitude_tmp.get(x+1,y+1) + altitude_tmp.get(x,y+1) + altitude_tmp.get(x+2,y+1) 
            + altitude_tmp.get(x+1,y+2) + altitude_tmp.get(x,y+2) + altitude_tmp.get(x+2,y+2);
        mean /= 9;
        altitude_map.get(x,y) = mean;
    }

    // init energy map
    for (int i=0; i<energy_map.size; i++) { energy_map.flat[i] = random_uniform(0,10); }

    // init plant map
    for (int i=0; i<plants_map.size; i++) { plants_map.flat[i] = NULL; }
    for (int i=0; i<nplants; i++) {
        Point pt;
        do { pt = Point::random(width,height); } while(getPlant(pt)!=NULL);
        PlantInternal *plant = new PlantInternal(pt,random_uniform(5,11));
        plants.insert(plant);
        plants_map.get(pt) = plant;
    }

    // init agent map
    for (int i=0; i<agents_map.size; i++) { agents_map.flat[i] = NULL; }
}

World::~World() {
    for (Players::const_iterator i=players.begin(); i!=players.end(); i++) { delete *i; }
    for (Player::AgentInternals::const_iterator i=agents.begin(); i!=agents.end(); i++) { delete *i; }
    for (PlantInternals::const_iterator i=plants.begin(); i!=plants.end(); i++) { delete *i; }
}

//-----------------------------------------------------------------

bool World::isGameFinished() const {
    size_t total_agents = agents.size();
    if (total_agents) return true; // no agent
    for (Players::const_iterator i=players.begin(); i!=players.end(); i++) if ((*i)->agents.size() == total_agents) return true; // all agent belong to one player
    return false;
}

void World::addPlayer(const std::string &name, unsigned int color, Mind *mind) {
    Point initial_position;
    for (PlantInternals::const_iterator i=plants.begin(); i!=plants.end(); i++) if (getAgent((*i)->position)==NULL) {
        initial_position = (*i)->position;
        break;
    }

    Player *player = new Player(name,color,mind);
    players.insert(player);

    spawnAgent(player,initial_position,Arguments());
}

void World::spawnAgent(Player *player, const Point &position, const Arguments &arguments) {
    assert(players.find(player) != players.end()); // player is known
    assert(getAgent(position)==NULL); // there is nobody here

    Player::AgentInternal *agent = new Player::AgentInternal(player,position,arguments);
    agents_map.get(position) = agent;
    player->agents.insert(agent);
    agents.insert(agent);
}

void Player::print(std::ostream &os) const {
    os<<name<<" ";
    if (deathtick>=0) os<<"dead deathtick="<<deathtick;
    else os<<"alive nagents="<<agents.size();
}

struct PlayerSorter {
    bool operator()(const Player *a,const Player *b) {
        if (a->deathtick != b->deathtick) return a->deathtick<b->deathtick;
        if (a->agents.size() != b->agents.size()) return a->agents.size()>b->agents.size();
        return a<b;
    }
};

void World::print(std::ostream &os) const
{
    os<<"finished="<<isGameFinished()<<std::endl;
    os<<"size="<<width<<"x"<<height<<std::endl;
    os<<"nticks="<<nticks<<std::endl;
    os<<"nplants="<<plants.size()<<std::endl;
    os<<"nplayers="<<players.size()<<" nagents="<<agents.size()<<std::endl;

    os<<std::endl;

    typedef std::vector<Player*> VectorPlayers;
    VectorPlayers sortable(players.size());
    std::copy(players.begin(),players.end(),sortable.begin());

    PlayerSorter sorter;
    std::sort(sortable.begin(),sortable.end(),sorter);

    int k=0;
    for (VectorPlayers::const_iterator i=sortable.begin(); i!=sortable.end(); i++) {
        const Player *player = *i;
        os<<(++k)<<". ";
        player->print(os);
        os<<std::endl;
    }
}

Player::AgentInternal *World::getAgent(const Point &point) const {
    if (not agents_map.isValid(point)) return NULL;
    return agents_map.get(point);
}

PlantInternal *World::getPlant(const Point &point) const {
    if (not plants_map.isValid(point)) return NULL;
    return plants_map.get(point);
}

void World::checkPosition(const Point &position, Agents &agents_viewed, Plants &plants_viewed) const {
    const Player::AgentInternal *agent = getAgent(position);
    if (agent) agents_viewed.insert(Agent(agent->player->name,agent->position));
    const PlantInternal *plant = getPlant(position);
    if (plant) plants_viewed.insert(Plant(plant->position,plant->eff));
}

void World::tick() {
    typedef std::vector< std::pair<Player::AgentInternal*,Action> > Actions; // vector for shuffle

    Actions actions;
    { // getting actions
        unsigned int nagents = 0;
        for (Players::const_iterator iplayer=players.begin(); iplayer!=players.end(); iplayer++) {
            Player *player = *iplayer;
            nagents += player->agents.size();
            
            // build views
            Agents agents_viewed;
            Plants plants_viewed;
            for (Player::AgentInternals::const_iterator iagent=player->agents.begin(); iagent!=player->agents.end(); iagent++) { 
                const Player::AgentInternal *agent = *iagent;
                assert(agent->player==player);

                checkPosition(agent->position,agents_viewed,plants_viewed);
                checkPosition(agent->position.left(),agents_viewed,plants_viewed);
                checkPosition(agent->position.right(),agents_viewed,plants_viewed);
                checkPosition(agent->position.top(),agents_viewed,plants_viewed);
                checkPosition(agent->position.down(),agents_viewed,plants_viewed);
            }

            // query mind
            for (Player::AgentInternals::const_iterator iagent=player->agents.begin(); iagent!=player->agents.end(); iagent++) {
                Player::AgentInternal *agent = *iagent;
                AgentMe view(player->name,agent->position,agent->arguments,agent->energy,agent->loaded,agents_viewed,plants_viewed,energy_map);
                actions.push_back(std::make_pair(agent,player->mind(view)));
            }
        }
        assert(actions.size() == nagents);
        assert(actions.size() == agents.size());
    }

    // shuffle actions    
    std::random_shuffle(actions.begin(),actions.end());

    // parsing actions
    for (Actions::const_iterator iaction=actions.begin(); iaction!=actions.end(); iaction++) {
        Player::AgentInternal *agent = iaction->first;
        const Action &action = iaction->second;
        
        agent->energy--;

        if (action.type == Action::MOVE) {
            Point position_new = agent->position.getNewPositionToDestination(action.data);
            if (agents_map.isValid(position_new) and getAgent(position_new)==NULL and abs(altitude_map.get(agent->position)-altitude_map.get(position_new))<4.) {
                assert(agents_map.get(agent->position)==agent);
                agents_map.get(agent->position) = NULL;
                agent->position = position_new;
                agents_map.get(position_new) = agent;
            }
        } else if (action.type == Action::ATTACK) {
            Point position_new = agent->position.getNewPositionToDestination(action.data);
            Player::AgentInternal *target = getAgent(position_new);
            if (target and position_new.x==action.data.x and position_new.y==action.data.y) {
                energy_map.get(target->position) += target->energy+25;
                target->energy = 0;
            }
        } else if (action.type == Action::SPAWN) {
            Point position_new = agent->position.getNewPositionToDestination(action.data);
            if (agents_map.isValid(position_new) and getAgent(position_new)==NULL and agent->energy>=50) {
                spawnAgent(agent->player,position_new,action.arguments);
                agent->energy -= 50;
            }
        } else if (action.type == Action::EAT) {
            float meal = energy_map.get(agent->position);
            energy_map.get(agent->position) = 0;
            agent->energy += meal;
        } else if (action.type == Action::LIFT) { //FIXME untested
            if (not agent->loaded and altitude_map.get(agent->position)>1) {
                agent->loaded = true;
                altitude_map.get(agent->position)--;
            }
        } else if (action.type == Action::DROP) { //FIXME untested
            if (agent->loaded) {
                agent->loaded = false;
                altitude_map.get(agent->position)++;
            }
        } else if (action.type == Action::DONOTHING) {
        } else {
            std::cerr<<"unknow action "<<action.type<<std::endl;
        }
    }

    // killing agents
    for (Player::AgentInternals::iterator ipair=agents.begin(); ipair!=agents.end();) {
        Player::AgentInternals::iterator ipair_copy = ipair++;
        Player::AgentInternal *agent = *ipair_copy;
        if (agent->energy<=0) {
            assert(agents_map.get(agent->position)==agent);
            agents_map.get(agent->position) = NULL;
            agent->player->agents.erase(agent); //FIXME dirty hack
            agents.erase(ipair_copy);
            delete agent;
        }
    }

    // flagging dead player
    for (Players::const_iterator iplayer=players.begin(); iplayer!=players.end(); iplayer++) if ((*iplayer)->deathtick<0 and (*iplayer)->agents.empty()) {
        Player *player = *iplayer;
        player->deathtick = nticks;
        if (deadPlayer) deadPlayer(*player,callbackData);
    }

    // updating plants
    for (PlantInternals::const_iterator iplant=plants.begin(); iplant!=plants.end(); iplant++) {
        const PlantInternal *plant = *iplant;
        for (int dx=-1; dx<2; dx++) for (int dy=-1; dy<2; dy++) {
            Point position = plant->position;
            position.x += dx;
            position.y += dy;
            energy_map.get(position) += plant->eff;
        }
    }

    nticks++;
}
