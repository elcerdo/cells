#ifndef __MINDSPYTHON_H__
#define __MINDSPYTHON_H__

#include <Python.h>
#include "interface.h"

namespace Python {
    typedef std::pair<std::string,std::string> MindName;
    typedef std::vector<MindName> MindNames;

    void init();
    void destroy();

    bool loadMind(const std::string &player_name, const std::string &module_name);
    void addPythonPath(const std::string &path);

    MindName getMindName(int index);
    MindNames getMindNames();
    int getMindCount();

    void buildViewedData(const Agents &agents, const Plants &plants);
    void buildEnergyMap(const MapFloat &energy_map);
    
    struct MindPython : public Mind {
        MindPython(const std::string &player_name);
        virtual Action act(const AgentMe &me) const;
        PyObject *function;
    };

}
#endif

