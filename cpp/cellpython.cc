#include <iostream>
#include <Python.h>
#include "minds.h"

using std::endl;
using std::cout;

void Py_AddToPythonPath(const std::string &path)
{
    PyObject *psys = PyImport_ImportModule("sys");
    PyObject *ppythonpath = PyObject_GetAttrString(psys,"path");
    Py_DECREF(psys);
    PyObject *ppath = PyString_FromString(path.c_str());
    PyList_Append(ppythonpath,ppath);
    Py_DECREF(ppath);
    Py_DECREF(ppythonpath);
    if (PyErr_Occurred()) PyErr_Print();
}

struct PythonMinds
{
    struct Mind
    {
        Mind() : module(NULL), function(NULL) {}
        PyObject *module;
        PyObject *function;
    };
    typedef std::map<std::string,Mind> Minds;

    static void init();
    static void destroy();
    static PythonMinds *get();
    static World::Player::Action mind(const World::Player::Data &data);

    bool loadPythonMind(const std::string &player_name, const std::string &module_name);
    World::Player::Action act(const World::Player::Data &data);

    Minds minds;
private:
    PythonMinds();
    ~PythonMinds();
};

static PythonMinds *instance = NULL;

PythonMinds::PythonMinds()
{
    Py_Initialize();
    Py_AddToPythonPath("..");
    Py_AddToPythonPath("../minds");
}

void PythonMinds::init()
{
    assert(not instance);
    instance = new PythonMinds();
}

void PythonMinds::destroy()
{
    assert(instance);
    delete instance;
}

PythonMinds* PythonMinds::get()
{
    assert(instance);
    return instance;
}

PythonMinds::~PythonMinds()
{
    for (Minds::const_iterator i=minds.begin(); i!=minds.end(); i++) {
        Py_DECREF(i->second.function);
        Py_DECREF(i->second.module);
    }
    PythonMinds::destroy();
}

bool PythonMinds::loadPythonMind(const std::string &player_name, const std::string &module_name)
{
    if (minds.find(player_name)!=minds.end()) {
        std::cerr<<"duplicate player name"<<endl;
        return false;
    }

    PyObject *pmodule = PyImport_ImportModule(module_name.c_str());
    if (not pmodule) {
        if (PyErr_Occurred()) PyErr_Print();
        std::cerr<<"cant import module"<<endl;
        return false;
    }
        
    PyObject *pfunc   = PyObject_GetAttrString(pmodule,"act");

    if (not pfunc) {
        Py_DECREF(pmodule);
        if (PyErr_Occurred()) PyErr_Print();
        std::cerr<<"no act object in module"<<endl;
        return false;
    }

    if (not PyCallable_Check(pfunc)) {
        Py_DECREF(pmodule);
        Py_DECREF(pfunc);
        if (PyErr_Occurred()) PyErr_Print();
        std::cerr<<"act is not callable"<<endl;
        Py_DECREF(pfunc);
        return false;
    }

    Mind mind;
    mind.module   = pmodule;
    mind.function = pfunc;
    minds[player_name]=mind;

    return true;
}

World::Player::Action PythonMinds::act(const World::Player::Data &data)
{
    const World::Player::Action defaultAction = World::Player::Action::doNothing();

    PyObject *function = NULL;
    { // check if a mind is loaded for corresponding player
        Minds::const_iterator imind = minds.find(data.player_name);
        if (imind==minds.end()) {
            std::cerr<<"cant find python mind for player "<<data.player_name<<endl;
            return defaultAction;
        }
        function = imind->second.function;
    }

    PyObject *call_args = NULL;
    { // build callargs
        PyObject *agent_args = PyList_New(0);
        for (World::Player::Arguments::const_iterator i=data.agent_arguments.begin(); i!=data.agent_arguments.end(); i++) {
            PyObject *agent_arg = Py_BuildValue("i",*i);
            PyList_Append(agent_args,agent_arg);
            Py_DECREF(agent_arg);
        }
        call_args = Py_BuildValue("(s,(i,i),O,f,i,i,i)",
            data.player_name.c_str(),
            data.agent_position.x,data.agent_position.y,
            agent_args,
            data.agent_energy,data.agent_loaded ? 1:0,
            data.world_width,data.world_height);
        Py_DECREF(agent_args);
    }

    World::Player::Arguments return_args;
    { // call the function and parse result
        assert(function and call_args);
        PyObject *value = PyObject_CallObject(function,call_args);
        Py_DECREF(call_args);

        if (not value) {
            if (PyErr_Occurred()) PyErr_Print();
            std::cerr<<"call to function failed"<<endl;
            return defaultAction;
        }

        if (not PyList_Check(value)) {
            Py_DECREF(value);
            if (PyErr_Occurred()) PyErr_Print();
            std::cerr<<"return value is not a list"<<endl;
            return defaultAction;
        }

        for (int i=0; i<PyList_Size(value); i++) {
            PyObject *pitem = PyList_GetItem(value,i);
            int item = PyInt_AsLong(pitem);

            if (PyErr_Occurred()) {
                Py_DECREF(value);
                PyErr_Print();
                std::cerr<<"return list item is not a int"<<endl;
                return defaultAction;
            }

            return_args.push_back(item);
        }
        Py_DECREF(value);
    }

           if (return_args.size()>=1 and return_args[0]==World::Player::Action::DONOTHING) {
        return World::Player::Action::doNothing();
    } else if (return_args.size()>=3 and return_args[0]==World::Player::Action::SPAWN) {
        Point dest(return_args[1],return_args[2]);
        World::Player::Arguments spawn_args(return_args.size()-3);
        std::copy(return_args.begin()+3,return_args.end(),spawn_args.begin());
        return World::Player::Action::spawn(dest,spawn_args);
    } else if (return_args.size()>=3 and return_args[0]==World::Player::Action::MOVE) {
        Point dest(return_args[1],return_args[2]);
        return World::Player::Action::moveTo(dest);
    } else if (return_args.size()>=1 and return_args[0]==World::Player::Action::EAT) {
        return World::Player::Action::eat();
    } else if (return_args.size()>=1 and return_args[0]==World::Player::Action::LIFT) {
        return World::Player::Action::lift();
    } else if (return_args.size()>=1 and return_args[0]==World::Player::Action::DROP) {
        return World::Player::Action::drop();
    }

    std::cerr<<"cant figure out action"<<endl;
    return defaultAction;
}

World::Player::Action PythonMinds::mind(const World::Player::Data &data)
{
    return PythonMinds::get()->act(data);
}

int main(int argc, char *argv[])
{
    PythonMinds::init();

    World world(300,300);
    world.addPlayer("dummy",0,mind_test1);
    world.addPlayer("spawner",0,mind_test2);
    //if (PythonMinds::get()->loadPythonMind("player1","mind1")) { world.addPlayer("player1",0,PythonMinds::mind); } 
    if (PythonMinds::get()->loadPythonMind("player2","mind2")) { world.addPlayer("player2",0,PythonMinds::mind); } 

    for (int i=0; i<1000; i++) world.tick();
    world.print(std::cout);

    Py_Finalize();
    return 0;
}

