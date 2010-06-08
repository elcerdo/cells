#include "minds.h"

#include <Python.h>
#include <iostream>
using std::cout;
using std::endl;

//********************************************************************************
// CPP MINDS
//********************************************************************************

World::Player::Action mind_test1(const World::Player::Data &data) {
    return World::Player::Action::moveTo(Point(10,10));
}

World::Player::Action mind_test2(const World::Player::Data &data) {
    if (data.agent_arguments.empty()) {
        int nennemies = 0;
        for (World::Player::ViewedAgents::const_iterator i=data.agents_viewed.begin(); i!=data.agents_viewed.end(); i++) if (i->player_name!=data.player_name) nennemies++;
        cout<<data.player_name<<" views "<<data.agents_viewed.size()<<"/"<<nennemies<<" agents/enemies "<<data.plants_viewed.size()<<" plants"<<endl;

        if (data.agent_energy>70) {
            Point target = Point::random(data.world_width,data.world_height);
            World::Player::Arguments args;
            args.push_back(target.x);
            args.push_back(target.y);
            return World::Player::Action::spawn(target,args);
        }
        return World::Player::Action::eat();
    }


    return World::Player::Action::moveTo(Point(data.agent_arguments[0],data.agent_arguments[1]));
}

//********************************************************************************
// PYTHON MINDS
//********************************************************************************

namespace PythonMinds
{

struct Mind
{
    Mind() : module(NULL), function(NULL) {}
    PyObject *module;
    PyObject *function;
};

typedef std::map<std::string,Mind> Minds;
static Minds minds;
static Names names;
static bool minds_init = false;


void init()
{
    assert(minds_init==false);
    minds_init = true;

    Py_Initialize();

    addPythonPath("..");
    addPythonPath("../minds");
    addPythonPath("../../minds");
}

void destroy()
{
    assert(minds_init);
    minds_init = false;

    for (Minds::const_iterator i=minds.begin(); i!=minds.end(); i++) {
        Py_DECREF(i->second.function);
        Py_DECREF(i->second.module);
    }
    minds.clear();

    Py_Finalize();
}

const Names &getLoadedMindNames()
{
    return names;
}

void addPythonPath(const std::string &path)
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

bool loadMind(const std::string &player_name, const std::string &module_name)
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
    minds[player_name] = mind;

    names.push_back(std::make_pair(player_name,module_name));

    return true;
}

World::Player::Action mind(const World::Player::Data &data)
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
        PyObject *agents_viewed = PyList_New(0);
        for (World::Player::ViewedAgents::const_iterator i=data.agents_viewed.begin(); i!=data.agents_viewed.end(); i++) {
            PyObject *agent_viewed = Py_BuildValue("(s,(i,i))",
                i->player_name.c_str(),
                i->position.x,i->position.y);
            PyList_Append(agents_viewed,agent_viewed);
            Py_DECREF(agent_viewed);
        }
        PyObject *plants_viewed = PyList_New(0);
        for (World::Plants::const_iterator i=data.plants_viewed.begin(); i!=data.plants_viewed.end(); i++) {
            PyObject *plant_viewed = Py_BuildValue("((i,i),f)",
                (*i)->position.x,(*i)->position.y,
                (*i)->eff);
            PyList_Append(plants_viewed,plant_viewed);
            Py_DECREF(plant_viewed);
        }
        call_args = Py_BuildValue("(s,(i,i),O,f,i,O,O,i,i)",
            data.player_name.c_str(),
            data.agent_position.x,data.agent_position.y,
            agent_args,
            data.agent_energy,data.agent_loaded ? 1:0,
            agents_viewed, plants_viewed,
            data.world_width,data.world_height);
        Py_DECREF(agent_args);
        Py_DECREF(agents_viewed);
        Py_DECREF(plants_viewed);
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
    } else if (return_args.size()>=3 and return_args[0]==World::Player::Action::ATTACK) {
        Point dest(return_args[1],return_args[2]);
        return World::Player::Action::attack(dest);
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

}
