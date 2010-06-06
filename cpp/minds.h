#ifndef __MINDS_H__
#define __MINDS_H__

#include "world.h"

World::Player::Action mind_test1(const World::Player::Data &data); // debug mind: print args, do nothing
World::Player::Action mind_test2(const World::Player::Data &data); // spawer mind: spawn agent from plant when it as enough energy

#endif
