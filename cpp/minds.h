#ifndef __MINDS_H__
#define __MINDS_H__

#include "interface.h"

namespace Python
{
    typedef std::pair<std::string,std::string> MindName;
    typedef std::vector<MindName> MindNames;

    void init();
    void destroy();

    Action mind(const AgentMe &data);

    bool loadMind(const std::string &player_name, const std::string &module_name);
    void addPythonPath(const std::string &path);

    MindName getMindName(int index);
    MindNames getMindNames();
    int getMindCount();
};

Action mind_test1(const AgentMe &data); // debug mind: print args, do nothing
Action mind_test2(const AgentMe &data); // spawer mind: spawn agent from plant when it as enough energy
Action mind_test3(const AgentMe &data); // same as mind2 but doesnt print anything

#endif
