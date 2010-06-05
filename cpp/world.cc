#include "world.h"

#include <algorithm>
#include <vector>

World::Plant::Plant(const Point &position,float eff) : position(position), eff(eff) {}

World::Player::Agent::Agent(const Point &position, const Arguments &arguments, const World::Player *player) : position(position), loaded(false), arguments(arguments), player(player) {}

World::Player::Data::Data(const std::string &player_name, const Point &agent_position, const Arguments &agent_arguments, float agent_energy, bool agent_loaded, int world_width, int world_height) :
    player_name(player_name),
    agent_position(agent_position), agent_arguments(agent_arguments), agent_energy(agent_energy), agent_loaded(agent_loaded),
    world_width(world_width), world_height(world_height) {}

World::Player::Player(const std::string &name, unsigned int color, Mind *action) : name(name), color(color), deathtick(-1), messages_inbox(new Messages), messages_outbox(new Messages), action(action) {}

World::Player::~Player()
{
    delete messages_inbox;
    delete messages_outbox;
}

World::World(int width, int height, int nplants) : deadPlayer(NULL), callbackData(NULL), width(width), height(height), nticks(0), altitude(width,height), energy(width,height), occupied(width,height)
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

bool World::isGameFinished() const
{
    if (energies.empty()) return true; // no agent
    for (Players::const_iterator i=players.begin(); i!=players.end(); i++) if ((*i)->agents.size() == energies.size()) return true; // all agent belong to one player
    return false;
}


void World::addPlayer(const std::string &name, unsigned int color, Player::Mind *action)
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

void World::Player::print(std::ostream &os) const
{
    os<<name<<" "<<(deathtick<0 ? "alive" : "dead");
    if (deathtick>=0) os<<" deathtick="<<deathtick;
    else os<<" nagents="<<agents.size();
}

struct PlayerSorter
{
    bool operator()(const World::Player *a,const World::Player *b) {
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
    os<<"nplayers="<<players.size()<<" nagents="<<energies.size()<<std::endl;

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

World::Player::Action::Action() : type(DONOTHING) {}

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

World::Player::Action World::Player::Action::attack(const Point &dest)
{
    Action action;
    action.type = ATTACK;
    action.data = dest;
    return action;
}

World::Player::Action World::Player::Action::eat()
{
    Action action;
    action.type = EAT;
    return action;
}

World::Player::Action World::Player::Action::lift()
{
    Action action;
    action.type = LIFT;
    return action;
}

World::Player::Action World::Player::Action::drop()
{
    Action action;
    action.type = DROP;
    return action;
}

World::Player::Action World::Player::Action::doNothing()
{
    Action action;
    action.type = DONOTHING;
    return action;
}

bool World::isPositionValid(const Point &point) const
{
    if (not occupied.isValid(point)) return false;
    return not occupied.get(point);
}

bool World::isAttackable(const Point &point) const
{
    if (not occupied.isValid(point)) return false;
    return occupied.get(point);
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
                Player::Data data(player->name,agent->position,agent->arguments,energies[agent],agent->loaded,width,height);
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
        } else if (action.type == Player::Action::ATTACK) {
            Point position_new = agent->position.getNewPositionToDestination(action.data);
            if (isAttackable(position_new) and position_new.x==action.data.x and position_new.y==action.data.y) {
                AgentEnergies::const_iterator itarget = energies.begin();
                while (itarget!=energies.end()) {
                    if (itarget->first->position.x==position_new.x and itarget->first->position.y==position_new.y) { break; }
                    itarget++;
                }
                assert(itarget != energies.end());
                energy.get(itarget->first->position) += itarget->second+25;
                energies[itarget->first] = 0;
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
        } else if (action.type == Player::Action::DONOTHING) {
        } else {
            std::cerr<<"unknow action "<<action.type<<std::endl;
        }
    }

    // killing agents
    for (AgentEnergies::iterator ipair=energies.begin(); ipair!= energies.end();) {
        AgentEnergies::iterator ipair_copy = ipair++;
        if (ipair_copy->second<=0) {
            Player::Agent *agent = ipair_copy->first;
            occupied.get(agent->position) = false;
            const_cast<Player*>(agent->player)->agents.erase(agent); //FIXME dirty hack
            energies.erase(ipair_copy);
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
    for (World::Plants::const_iterator iplant=plants.begin(); iplant!=plants.end(); iplant++) {
        const Plant *plant = *iplant;
        for (int dx=-1; dx<2; dx++) for (int dy=-1; dy<2; dy++) {
            Point position = plant->position;
            position.x += dx;
            position.y += dy;
            if (energy.isValid(position)) energy.get(position) += plant->eff;
        }
    }

    nticks++;
}
