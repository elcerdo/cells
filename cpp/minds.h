#ifndef __MINDS_H__
#define __MINDS_H__

#include "world.h"

namespace PythonMinds
{
    typedef std::vector< std::pair<std::string,std::string> > Names;

    void init();
    void destroy();

    bool loadMind(const std::string &player_name, const std::string &module_name);
    void addPythonPath(const std::string &path);
    World::Player::Action mind(const World::Player::Data &data);
    const Names &getLoadedMindNames();
};

World::Player::Action mind_test1(const World::Player::Data &data); // debug mind: print args, do nothing
World::Player::Action mind_test2(const World::Player::Data &data); // spawer mind: spawn agent from plant when it as enough energy

#endif
