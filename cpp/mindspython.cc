#include "mindspython.h"

#include <map>
#include <iostream>

using std::cerr;
using std::endl;

namespace Python {

struct MindModule {
    std::string module_name;
    PyObject *module;
    PyObject *function;
};
typedef std::map<std::string,MindModule> MindModules;

static MindModules mind_modules;
static bool minds_init = false;

void init() {
    assert(not minds_init);
    minds_init = true;

    Py_Initialize();

    addPythonPath("..");
    addPythonPath("../minds");
    addPythonPath("../../minds");
}

void destroy() {
    assert(minds_init);
    minds_init = false;

    for (MindModules::const_iterator i=mind_modules.begin(); i!=mind_modules.end(); i++) {
        Py_DECREF(i->second.module);
        Py_DECREF(i->second.function);
    }

    Py_Finalize();
}

int getMindCount() {
    assert(minds_init);
    return mind_modules.size();
}

MindName getMindName(int index) {
    assert(minds_init);
    assert(index>=0);

    for (MindModules::const_iterator i=mind_modules.begin(); i!=mind_modules.end(); i++) {
        if (index==0) { return std::make_pair(i->first,i->second.module_name); }
        index--;
    }

    return std::make_pair("invalid player","invalid module");
}

MindNames getMindNames() {
    assert(minds_init);
    MindNames names;
    for (MindModules::const_iterator i=mind_modules.begin(); i!=mind_modules.end(); i++) {
        names.push_back(std::make_pair(i->first,i->second.module_name));
    }
    return names;
}

void addPythonPath(const std::string &path) {
    assert(minds_init);
    PyObject *psys = PyImport_ImportModule("sys");
    PyObject *ppythonpath = PyObject_GetAttrString(psys,"path");
    PyObject *ppath = PyString_FromString(path.c_str());
    PyList_Append(ppythonpath,ppath);
    Py_DECREF(ppath);
    Py_DECREF(ppythonpath);
    Py_DECREF(psys);
    if (PyErr_Occurred()) PyErr_Print();
}

bool loadMind(const std::string &player_name, const std::string &module_name) {
    assert(minds_init);
    if (mind_modules.find(player_name)!=mind_modules.end()) {
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

    MindModule mind;
    mind.module   = pmodule;
    mind.function = pfunc;
    mind.module_name = module_name;
    mind_modules[player_name] = mind;

    return true;
}

MindPython::MindPython(const std::string &player_name) : function(NULL) {
    assert(minds_init);
    
    // check if a mind is loaded for corresponding player
    MindModules::const_iterator imind = mind_modules.find(player_name);
    if (imind==mind_modules.end()) {
        std::cerr<<"cant find python mind for player "<<player_name<<endl;
        return;
    }

    function = imind->second.function;
}

static PyObject *agents_viewed_python = NULL;
static PyObject *plants_viewed_python = NULL;
static PyObject *energy_map_python    = NULL;

Action MindPython::act(const AgentMe &me) const {
    const Action defaultAction = Action::doNothing();

    PyObject *call_args = NULL;
    { // build callargs
        PyObject *agent_args = PyList_New(0);
        for (Arguments::const_iterator i=me.arguments.begin(); i!=me.arguments.end(); i++) {
            PyObject *agent_arg = Py_BuildValue("i",*i);
            PyList_Append(agent_args,agent_arg);
            Py_DECREF(agent_arg);
        }
        call_args = Py_BuildValue("(s,(i,i),O,f,i,O,O,O,i,i)",
            me.player.c_str(),
            me.position.x,me.position.y,
            agent_args,
            me.energy,me.loaded ? 1:0,
            agents_viewed_python, plants_viewed_python, energy_map_python,
            me.energy_map.width,me.energy_map.height);
        Py_DECREF(agent_args);
    }

    Arguments return_args;
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
            int item = 0;
            if (PyFloat_Check(pitem)) item = PyFloat_AsDouble(pitem);
            else item = PyInt_AsLong(pitem);

            if (PyErr_Occurred()) {
                PyObject_Print(value,stdout,0);
                Py_DECREF(value);
                PyErr_Print();
                std::cerr<<"return list item is not numeric"<<endl;
                return defaultAction;
            }

            return_args.push_back(item);
        }
        Py_DECREF(value);
    }

           if (return_args.size()>=1 and return_args[0]==Action::DONOTHING) {
        return Action::doNothing();
    } else if (return_args.size()>=3 and return_args[0]==Action::SPAWN) {
        Point dest(return_args[1],return_args[2]);
        Arguments spawn_args(return_args.size()-3);
        std::copy(return_args.begin()+3,return_args.end(),spawn_args.begin());
        return Action::spawn(dest,spawn_args);
    } else if (return_args.size()>=3 and return_args[0]==Action::MOVE) {
        Point dest(return_args[1],return_args[2]);
        return Action::moveTo(dest);
    } else if (return_args.size()>=3 and return_args[0]==Action::ATTACK) {
        Point dest(return_args[1],return_args[2]);
        return Action::attack(dest);
    } else if (return_args.size()>=1 and return_args[0]==Action::EAT) {
        return Action::eat();
    } else if (return_args.size()>=1 and return_args[0]==Action::LIFT) {
        return Action::lift();
    } else if (return_args.size()>=1 and return_args[0]==Action::DROP) {
        return Action::drop();
    }

    std::cerr<<"cant figure out action"<<endl;
    return defaultAction;
}

void buildViewedData(const Agents &agents, const Plants &plants) {
    if (not minds_init) return;
    //cout<<"rebuilding agents"<<endl;
    Py_XDECREF(agents_viewed_python);
    agents_viewed_python = PyList_New(0);
    for (Agents::const_iterator i=agents.begin(); i!=agents.end(); i++) {
        PyObject *item = Py_BuildValue("(s,(i,i))",
            i->player.c_str(),
            i->position.x,i->position.y);
        PyList_Append(agents_viewed_python,item);
        Py_DECREF(item);
    }

    //cout<<"rebuilding plants"<<endl;
    Py_XDECREF(plants_viewed_python);
    plants_viewed_python = PyList_New(0);
    for (Plants::const_iterator i=plants.begin(); i!=plants.end(); i++) {
        PyObject *item = Py_BuildValue("((i,i),f)",
            i->position.x,i->position.y,
            i->eff);
        PyList_Append(plants_viewed_python,item);
        Py_DECREF(item);
    }
}

void buildEnergyMap(const MapFloat &energy_map) {
    if (not minds_init) return;
    //cout<<"rebuilding energymap"<<endl;
    Py_XDECREF(energy_map_python);
    energy_map_python = PyList_New(0);
    for (int k=0; k<energy_map.size; k++) {
        PyObject *item = Py_BuildValue("f",energy_map.flat[k]);
        PyList_Append(energy_map_python,item);
        Py_DECREF(item);
    }
}

}

