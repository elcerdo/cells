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

PythonMinds::PythonMinds() {}

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
    for (Minds::const_iterator i=minds.begin(); i!=minds.end(); i++) Py_DECREF(i->second.function);
}

bool PythonMinds::loadPythonMind(const std::string &player_name, const std::string &module_name)
{
    PyObject *pmodule = PyImport_ImportModule(module_name.c_str());
    if (not pmodule) {
        if (PyErr_Occurred()) PyErr_Print();
        std::cerr<<"cant import module"<<endl;
        return false;
    }
        
    PyObject *pfunc   = PyObject_GetAttrString(pmodule,"act");
    Py_DECREF(pmodule);

    if (not pmodule) {
        if (PyErr_Occurred()) PyErr_Print();
        std::cerr<<"no act object in module"<<endl;
        return false;
    }

    if (not PyCallable_Check(pfunc)) {
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
    Minds::const_iterator imind = minds.find(data.player_name);
    if (imind==minds.end()) {
        std::cerr<<"cant find python mind for player "<<data.player_name<<endl;
        return World::Player::Action::pass();
    }

    PyObject *agent_args = PyList_New(0);
    for (World::Player::Arguments::const_iterator i=data.agent_arguments.begin(); i!=data.agent_arguments.end(); i++) {
        PyObject *agent_arg = Py_BuildValue("i",*i);
        PyList_Append(agent_args,agent_arg);
        Py_DECREF(agent_arg);
    }
    PyObject *callargs = Py_BuildValue("(s,(i,i),O,f,i,i,i)",
        data.player_name.c_str(),
        data.agent_position.x,data.agent_position.y,
        agent_args,
        data.agent_energy,data.agent_loaded,
        data.world_width,data.world_height);
    Py_DECREF(agent_args);

    PyObject *value = PyObject_CallObject(imind->second.function,callargs);
    Py_DECREF(callargs);

    if (not value) {
        if (PyErr_Occurred()) PyErr_Print();
        std::cerr<<"call to function failed"<<endl;
        return World::Player::Action::pass();
    }

    Py_DECREF(value);
    
    return World::Player::Action::eat();
}

World::Player::Action PythonMinds::mind(const World::Player::Data &data)
{
    return PythonMinds::get()->act(data);
}

int main(int argc, char *argv[])
{
    Py_Initialize();
    Py_AddToPythonPath("..");
    Py_AddToPythonPath("../minds");
    PythonMinds::init();

    World world(300,300);
    world.addPlayer("dummy",0,mind_test1);
    world.addPlayer("spawner",0,mind_test2);
    if (PythonMinds::get()->loadPythonMind("player1","mind1")) { world.addPlayer("player1",0,PythonMinds::mind); } 
    if (PythonMinds::get()->loadPythonMind("player2","mind1")) { world.addPlayer("player2",0,PythonMinds::mind); } 

    for (int i=0; i<1000; i++) world.tick();
    world.print(std::cout);

    PythonMinds::destroy();
    Py_Finalize();
    return 0;
}

