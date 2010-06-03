#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <cstdio>
#include <cassert>
#include <list>
#include <map>
#include <set>

float random_uniform(float min=0, float max=1) { return min+(max-min)*static_cast<float>(rand())/RAND_MAX; }

struct Point {
    Point(int x, int y) : x(x), y(y) {}
    static Point random(int width, int height) { return Point(rand()%width,rand()%height); }
    int x,y;
};

template <typename T>
struct Map
{
    Map(int width, int height) : width(width), height(height), size(width*height), flat(NULL) {
        flat = new T[size];
    }
    ~Map() {
        delete [] flat;
    }

    T &get(int x, int y) {
        assert(x>=0 and x<width);
        assert(y>=0 and y<height);
        return flat[x*height+y];
    }

    T &get(const Point &point) { return get(point.x,point.y); }

    const T &get(int x, int y) const {
        assert(x>=0 and x<width);
        assert(y>=0 and y<height);
        return flat[x*height+y];
    }
    
    const T &get(const Point &point) const { return get(point.x,point.y); }

    const int width,height,size;
    T *flat;
};

struct World : public QObject
{
Q_OBJECT
    struct Plant {
        Plant(const Point &position,float eff) : position(position), eff(eff) {}
        Point position;
        float eff;
    };
    typedef std::set<Plant*> Plants;

    struct Player
    {
        typedef std::string Message;
        typedef std::list<Message> Messages;

        struct Agent
        {
            Agent(const Point &position, const Player *player) : position(position), player(player) {}

            Point position;
            const Player *player;
        };
        typedef std::set<Agent*> Agents;

        struct Data {
            Data(const Point &agent_position, float agent_energy, int world_width, int world_height) : agent_position(agent_position), agent_energy(agent_energy),
                                                                                                       world_width(world_width), world_height(world_height) {}
            const Point &agent_position;
            const float agent_energy;
            const int world_width,world_height;
        };

        enum Action {SPAWN, MOVE, EAT, ATTACK, LIFT, DROP};
        typedef Action (*GetAction)(Data &data);

        Player(const std::string &name, unsigned int color, GetAction action) : name(name), color(color), messages_inbox(new Messages), messages_outbox(new Messages), action(action) {}
        ~Player() {
            delete messages_inbox;
            delete messages_outbox;
        }

        const std::string name;
        const unsigned int color;
        Messages *messages_inbox;
        Messages *messages_outbox;
        GetAction action;
        Agents agents;
    };
    typedef std::set<Player*> Players;
    typedef std::map<Player::Agent*,float> AgentEnergies;


    World(int width=30, int height=30, int nplants=12) : width(width), height(height), altitude(width,height), occupied(width,height) {
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

    ~World()
    {
        for (Plants::const_iterator i=plants.begin(); i!=plants.end(); i++) { delete *i; }
        for (Players::const_iterator i=players.begin(); i!=players.end(); i++) { delete *i; }
        for (AgentEnergies::const_iterator i=energies.begin(); i!=energies.end(); i++) { delete i->first; }
    }

    void addPlayer(const std::string &name, unsigned int color, Player::GetAction action) {
        Point initial_position(-1,-1);
        for (Plants::const_iterator i=plants.begin(); i!=plants.end(); i++) if (occupied.get((*i)->position) == false) {
            initial_position = (*i)->position;
        }

        Player *player = new Player(name,color,action);
        players.insert(player);
        spawnAgent(player,initial_position);
    }

    void spawnAgent(Player *player, const Point &position) {
        assert(players.find(player) != players.end());
        assert(occupied.get(position) == false);
        occupied.get(position) = true;
        Player::Agent *agent = new Player::Agent(position,player);
        player->agents.insert(agent);
        energies[agent] = 50.;
    }

    void printReport() const {
        printf("size=%dx%d\n",width,height);
        printf("nplants=%d\n",plants.size());
        printf("nplayers=%d nagents=%d\n",players.size(),energies.size());
        for (Players::const_iterator i=players.begin(); i!=players.end(); i++) {
            const Player *player = *i;
            printf("\tname=%s nagents=%d\n",player->name.c_str(),player->agents.size());
        }
    }

    const int width,height;
    Map<float> altitude;
    Map<bool> occupied;
    Plants plants;
    Players players;
    AgentEnergies energies;

public slot:
    void tick() {
        qDebug("tick");
    }
};

class WorldWidget : public QWidget
{
public:
    WorldWidget(const World &world, QWidget *parent = NULL) : QWidget(parent), world(world), image_need_update(true), image(QSize(world.width,world.height),QImage::Format_RGB32) {
        setMinimumSize(300,300);
        resize(700,700);
    }

    void setImageNeedUpdate() {
        image_need_update = true;
        repaint();
    }

    virtual void paintEvent(QPaintEvent *event) {
        if (image_need_update) {
            for (int x=0; x<world.width; x++) for (int y=0; y<world.height; y++) {
                float altitude = world.altitude.get(x,y);
                uint color = qRgb(255*altitude,127*altitude,63*altitude);
                image.setPixel(x,y,color);
            }
            for (World::Plants::const_iterator i=world.plants.begin(); i!=world.plants.end(); i++) {
                const World::Plant *plant = *i;
                image.setPixel(plant->position.x,plant->position.y,qRgb(0,255,0));
            }
            for (World::Players::const_iterator i=world.players.begin(); i!=world.players.end(); i++) {
                const World::Player *player = *i;
                for (World::Player::Agents::const_iterator j=player->agents.begin(); j!=player->agents.end(); j++) {
                    const World::Player::Agent *agent = *j;
                    image.setPixel(agent->position.x,agent->position.y,player->color);
                }
            }
            image_need_update = false;
        }

        Q_UNUSED(event);
        QPainter painter(this);
        painter.translate(rect().center());
        painter.scale(20,20);
        painter.translate(-image.width()/2.,-image.height()/2.);
        painter.drawImage(QPoint(),image);

    }

protected:
    const World &world;
    bool image_need_update;
    QImage image;
};


World::Player::Action action_player1(World::Player::Data &data) {
    return World::Player::SPAWN;
}

World::Player::Action action_player2(World::Player::Data &data) {
    return World::Player::EAT;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication app(argc,argv);
    World world;
    world.addPlayer("player1",qRgb(255,0,0),action_player1);
    world.addPlayer("player2",qRgb(0,0,255),action_player2);
    world.printReport();
    WorldWidget widget(world);

    widget.show();

    return app.exec();
}

